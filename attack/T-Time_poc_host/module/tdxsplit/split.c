#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <linux/rcupdate.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/kvm_host.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel module to manage TDX split pages");

static char *target_name = "qemu-system-x86_64";
static int pid;
struct mm_struct *mm;

struct tdx_module_args td_argu;

u64 tdh_mem_range_block_woomin(hpa_t tdr, gpa_t gpa, int level,
                                      struct tdx_module_args *out);
void tdx_flush_tlb(struct kvm_vcpu *vcpu);
void tdx_track(struct kvm *kvm);
void woomin_split_all_pages(struct kvm_vcpu *vcpu, int target_level);

static int pid_tdx(void)
{
    struct task_struct *task;
    int found = 0;

    for_each_process(task) {
        struct path exe_path;
        char exe_buf[512];

        if (task->mm && task->mm->exe_file) {
            exe_path = task->mm->exe_file->f_path;
            path_get(&exe_path);
            char *path_name = d_path(&exe_path, exe_buf, sizeof(exe_buf));

            if (!IS_ERR(path_name) && strstr(path_name, target_name) != NULL) {
                pr_info("Process found: Name = %s, PID = %d, Path = %s\n", task->comm, task->pid, path_name);
                found = 1;
                pid = task->pid;
                path_put(&exe_path);
                break;
            }
            path_put(&exe_path);
        }
    }

    if (!found) {
        pr_info("No process found with path containing '%s'\n", target_name);
        return -ESRCH;
    }

    return 0;
}

static int __init tdx_split_init(void)
{
    struct task_struct *task;
    struct file *file = NULL;
    struct fdtable *fdt;
    int fd;
    struct files_struct *files;
    struct kvm *kvm = NULL;
    struct kvm_vcpu *vcpu = NULL;
    bool found = false;

    if (pid_tdx() < 0)
        return -ESRCH;

    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    mm = task ? task->mm : NULL;
    rcu_read_unlock();

    if (!task || !mm) {
        pr_err("Task not found for PID %d\n", pid);
        return -ESRCH;
    }

    task_lock(task);
    files = task->files;
    if (!files) {
        task_unlock(task);
        pr_err("No files structure for PID %d\n", pid);
        return -ENOENT;
    }

    fdt = files_fdtable(files);
    rcu_read_lock();
    for (fd = 0; fd < fdt->max_fds; fd++) {
        file = rcu_dereference(fdt->fd[fd]);
        if (file && file->f_op && file->f_op->owner &&
            strcmp(file->f_op->owner->name, "kvm") == 0) {
            found = true;
            break;
        }
    }
    rcu_read_unlock();
    task_unlock(task);

    if (!found) {
        pr_err("No KVM vCPU file descriptor found for PID %d\n", pid);
        return -ENOENT;
    }

    if (file && file->private_data) {
        vcpu = file->private_data;
        kvm = vcpu->kvm;

        tdx_flush_tlb(vcpu);
        __flush_tlb_all();
        tdx_track(kvm);

        woomin_split_all_pages(vcpu, PG_LEVEL_4K);

        tdx_flush_tlb(vcpu);
        __flush_tlb_all();
        tdx_track(kvm);
    }

    return 0;
}

static void __exit tdx_split_exit(void)
{
    pr_info("Exiting TDX split module\n");
}

module_init(tdx_split_init);
module_exit(tdx_split_exit);

