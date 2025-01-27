#include "sched.h"
#include <linux/slab.h> // For kmalloc and kfree 
#include <linux/sched/signal.h>
#include "utilities.h"

struct mock_task_struct * create_mock_task(
    struct task_struct * task
) {
    struct mock_task_struct * my_task = kmalloc(sizeof(struct mock_task_struct), GFP_KERNEL);

    if (!my_task) {
        printk(KERN_ERR "Failed to allocate memory for mock_task_struct\n");
        return NULL;
    }

    my_task-> pid = task->pid;
    my_task-> tgid = task->tgid;
    my_task->prio = task->prio;
    my_task->state = task->__state;
    my_task->policy = task->policy;
    my_task-> comm = kmalloc(sizeof(task->comm), GFP_KERNEL);
    
    for(int i = 0; i<sizeof(task->comm); i++){
        my_task->comm[i] = task->comm[i];
    }

    my_task->comm[sizeof(task->comm) - 1] ='\0';
    my_task->utime = task->utime;
    my_task->stime = task->stime;

    if(task->mm){
        my_task->vmemory = (task->mm->total_vm << (PAGE_SHIFT - 10));
    }
    else{
        my_task->vmemory = 0;
    }

    my_task->io_rbytes = (unsigned int)task->ioac.read_bytes;
    my_task->io_wbytes = (unsigned int)task->ioac.write_bytes;

    my_task->next = NULL;

    return my_task;
}

void free_mock_tasks(struct mock_task_struct *task) {
    if (!task) return;
    free_mock_tasks(task->next);
    kfree(task);
}

struct mock_task_struct * create_mock_tasks(void){
    struct task_struct * task;
    struct mock_task_struct * mock_task_head = NULL, * mock_task_it = NULL;

    for_each_process(task){ //the macro already handles concurrency

        struct mock_task_struct *new_task = create_mock_task(task);

        if(!new_task){
            printk(KERN_ERR "Error! Task could not be retrieved!\n");
            return NULL;
        }


        if(!mock_task_head){
 
            mock_task_head = new_task;

        }
        else{
            mock_task_it->next = new_task;
        }

        mock_task_it = new_task;

    }

    return mock_task_head;
}