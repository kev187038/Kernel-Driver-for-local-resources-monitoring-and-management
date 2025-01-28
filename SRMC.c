#include <linux/module.h>
#include <linux/init.h>
#include "sched.h"
#include <linux/kernel.h>
#include <linux/sched.h>
#include "utilities.h"
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/timer.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gabriele Matini");
MODULE_DESCRIPTION("Module that tracks resource usage and prioritizes processes");

//These static variables are visible only to the kernel module (globally)
static struct mock_task_struct * tasks;
static struct workqueue_struct *wq;
static struct delayed_work update_work;

//Show function that writes on the file
static int srmc_show(struct seq_file *m, void *v) {

    struct mock_task_struct * t = tasks;

    seq_printf(m, "PID\tName\tCPU (User) (ms)\tCPU (Kernel) (ms)\tMEM(kb)\tDISK I/O Read(kb)\tDISK I/O Write(kb)\tState\tTGID\tPriority\n");

    while(t != NULL){

        seq_printf(m, "%d\t%s\t%lld\t%lld\t%d\t%lld\t%lld\t%ld\t%d\t%d\n", 
            t->pid, t->comm, t->utime / 100000, t->stime / 100000, t->vmemory, t->io_rbytes / 100, t->io_wbytes / 100, t->state, t->tgid, t->prio);
        t = t->next;
    }
    return 0;
}

//Open function for the file
static int srmc_open(struct inode *inode, struct file *file) {
    return single_open(file, srmc_show, NULL);
}

//Struct that defines callbacks of p_ops
static const struct proc_ops p_ops = {
    .proc_open = srmc_open,
    .proc_read = seq_read,
    .proc_release = single_release,
};

//Refresh the file
static void update_mock_tasks(struct work_struct *work) {
    free_mock_tasks(tasks);      
    tasks = create_mock_tasks(); 

    if (!tasks) {
        printk(KERN_ERR "SRMC - Failed to update mock tasks!\n");
    }

    //Rebuild srmc file
    remove_proc_entry("srmc", NULL);
    proc_create("srmc", 0, NULL, &p_ops);

    queue_delayed_work(wq, &update_work, msecs_to_jiffies(1000));
}

//StartFlow of execution
static int __init __SRMC_init(void){

    printk(KERN_INFO "SRMC - Initializing Kernel Module!\n");

    tasks = create_mock_tasks();
    
    if (tasks == NULL) {
        printk(KERN_ERR "SRMC - Failed to create mock task list!\n");
        return -ENOMEM;  //Return error code if task list creation fails
    }

    printk(KERN_INFO "SRMC - Mock task list created successfully!\n");

    //Expose data to user space under /proc/srmc proc_create will call the srmc_open and srmc_show
    proc_create("srmc", 0, NULL, &p_ops);

    //Create workqueue to update the task table every second
    wq = create_singlethread_workqueue("srmc_wq");
    INIT_DELAYED_WORK(&update_work, update_mock_tasks);
    queue_delayed_work(wq, &update_work, msecs_to_jiffies(1000));

    return 0;
}

//Called when remove module is called
static void __exit __SRMC_exit(void){
    
    cancel_delayed_work_sync(&update_work);
    destroy_workqueue(wq);
    free_mock_tasks(tasks);
    remove_proc_entry("srmc", NULL);
    printk(KERN_INFO "SRMC - Goodbye Kernel!\n");
}

module_init(__SRMC_init);
module_exit(__SRMC_exit);