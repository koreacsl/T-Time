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
MODULE_DESCRIPTION("TDX Tagging Kernel Module");

static char *target_name = "qemu-system-x86_64";
static int pid;
static int index = 0;
static char user_input[30] = "default";

// Module parameters
module_param(index, int, 0644);
MODULE_PARM_DESC(index, "Index to store the string");
module_param_string(user_input, user_input, sizeof(user_input), 0644);
MODULE_PARM_DESC(user_input, "String to be stored");

struct mm_struct *mm;

// External functions
int woomin_array_tag(int index, const char *str);

static int pid_find_vm(void)
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

            if (!IS_ERR(path_name) && strstr(path_name, target_name)) {
                pr_info("VM process found: Name = %s, PID = %d, Path = %s\n", task->comm, task->pid, path_name);
                pid = task->pid;
                found = 1;
                path_put(&exe_path);
                break;
            }
            path_put(&exe_path);
        }
    }

    return found ? 0 : -ESRCH;
}

static int __init tdx_tag_init(void)
{
    struct task_struct *task;
    struct file *file = NULL;
    struct files_struct *files;
    struct fdtable *fdt;
    struct kvm *kvm = NULL;
    struct kvm_vcpu *vcpu = NULL;
    int fd;
    bool found = false;

    if (pid_find_vm() < 0) {
        pr_err("Target VM not found.\n");
        return -ESRCH;
    }

    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    mm = task ? task->mm : NULL;
    rcu_read_unlock();

    if (!task || !mm) {
        pr_err("Task structure not found for PID %d\n", pid);
        return -ESRCH;
    }

    task_lock(task);
    files = task->files;
    if (!files) {
        task_unlock(task);
        pr_err("No file structure for PID %d\n", pid);
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
        pr_err("KVM vCPU file descriptor not found for PID %d\n", pid);
        return -ENOENT;
    }

    vcpu = file->private_data;
    kvm = vcpu->kvm;

    if (!vcpu || !kvm) {
        pr_err("Invalid vCPU or KVM structure for PID %d\n", pid);
        return -EINVAL;
    }

    woomin_array_tag(index, user_input);

    return 0;
}

static void __exit tdx_tag_exit(void)
{
    pr_info("Exiting TDX Tagging Module\n");
}

module_init(tdx_tag_init);
module_exit(tdx_tag_exit);

