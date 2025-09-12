#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kvm_host.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <linux/rcupdate.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("TDX Flush Kernel Module");

static char *target_name = "qemu-system-x86_64";
static int pid;
static unsigned long gpa_track = 0x178030b1; // Default GPA to flush

module_param(gpa_track, ulong, 0644);
MODULE_PARM_DESC(gpa_track, "GPA address to flush");

struct mm_struct *mm;

// External function
void woomin_flush(struct kvm_vcpu *vcpu, gpa_t gpa);

static int find_vm_pid(void)
{
    struct task_struct *task;

    for_each_process(task) {
        struct path exe_path;
        char exe_buf[512];

        if (task->mm && task->mm->exe_file) {
            exe_path = task->mm->exe_file->f_path;
            path_get(&exe_path);
            char *path_name = d_path(&exe_path, exe_buf, sizeof(exe_buf));

            if (!IS_ERR(path_name) && strstr(path_name, target_name)) {
                pr_info("VM process found: %s (PID: %d)\n", task->comm, task->pid);
                pid = task->pid;
                path_put(&exe_path);
                return 0;
            }
            path_put(&exe_path);
        }
    }

    pr_info("No process found matching '%s'\n", target_name);
    return -ESRCH;
}

static int __init tdx_flush_init(void)
{
    struct task_struct *task;
    struct file *file = NULL;
    struct files_struct *files;
    struct fdtable *fdt;
    struct kvm *kvm = NULL;
    struct kvm_vcpu *vcpu = NULL;
    int fd;
    bool found = false;

    if (find_vm_pid() < 0) {
        return -ESRCH;
    }

    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    mm = task ? task->mm : NULL;
    rcu_read_unlock();

    if (!task || !mm) {
        pr_err("Failed to find task or mm_struct for PID %d\n", pid);
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

    if (!found || !file || !file->private_data) {
        pr_err("No valid KVM vCPU file descriptor found for PID %d\n", pid);
        return -ENOENT;
    }

    vcpu = file->private_data;
    kvm = vcpu->kvm;

    if (!vcpu || !kvm) {
        pr_err("Invalid vCPU or KVM structure for PID %d\n", pid);
        return -EINVAL;
    }

    pr_info("Flushing GPA 0x%lx\n", gpa_track);
    woomin_flush(vcpu, gpa_track);

    return 0;
}

static void __exit tdx_flush_exit(void)
{
    pr_info("Exiting TDX Flush Module\n");
}

module_init(tdx_flush_init);
module_exit(tdx_flush_exit);

