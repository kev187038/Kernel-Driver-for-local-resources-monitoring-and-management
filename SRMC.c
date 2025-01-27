#include <linux/module.h>
#include <linux/init.h>
#include "sched.h"
#include <linux/kernel.h>
#include <linux/sched.h>
#include "utilities.h"
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gabriele Matini");
MODULE_DESCRIPTION("Module that tracks resource usage and prioritizes processes");

static struct mock_task_struct * tasks;

static int srmc_show(struct seq_file *m, void *v) {
    seq_printf(m, "PID: %d, Name: %s, CPU (user): %lld, CPU(kernel): %lld, MEM: %d, DISK I/O Read: %d, DISK I/O Write: %d\n", 
        tasks->pid, tasks->comm, tasks->utime, tasks->stime, tasks->vmemory, tasks->io_rbytes, tasks->io_wbytes);
    return 0;
}

static int srmc_open(struct inode *inode, struct file *file) {
    return single_open(file, srmc_show, NULL);
}

static const struct proc_ops p_ops = {
    .proc_open = srmc_open,
    .proc_read = seq_read,
    .proc_release = single_release,
};

static int __init __SRMC_init(void){

    printk("SRMC - Initializing Kernel Module!\n");
    tasks = create_mock_tasks();
    
    if (tasks == NULL) {
        printk(KERN_ERR "SRMC - Failed to create mock task list!\n");
        return -ENOMEM;  //Return error code if task list creation fails
    }

    printk(KERN_INFO "SRMC - Mock task list created successfully!\n");

    //Expose data to user space under /proc/srmc
    proc_create("srmc", 0, NULL, &p_ops);

    return 0;
}

static void __exit __SRMC_exit(void){
    
    free_mock_tasks(tasks);
    remove_proc_entry("srmc", NULL);
    printk(KERN_INFO "SRMC - Goodbye Kernel!\n");
}

module_init(__SRMC_init);
module_exit(__SRMC_exit);