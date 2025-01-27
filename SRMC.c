#include <linux/module.h>
#include <linux/init.h>
#include "sched.h"
#include <linux/kernel.h>
#include <linux/sched.h>
#include "utilities.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gabriele Matini");
MODULE_DESCRIPTION("Module that tracks resource usage and prioritizes processes");

static struct mock_task_struct * tasks;

static int __init __SRMC_init(void){

    printk("SRMC - Initializing Kernel Module!\n");
    tasks = create_mock_tasks();
    
    if (tasks == NULL) {
        printk(KERN_ERR "SRMC - Failed to create mock task list!\n");
        return -ENOMEM;  //Return error code if task list creation fails
    }

    printk(KERN_INFO "SRMC - Mock task list created successfully!\n");
    return 0;
}

static void __exit __SRMC_exit(void){
    
    free_mock_tasks(tasks);
    printk(KERN_INFO "SRMC - Goodbye Kernel!\n");
}


module_init(__SRMC_init);
module_exit(__SRMC_exit);