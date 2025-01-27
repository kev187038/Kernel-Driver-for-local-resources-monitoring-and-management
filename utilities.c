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
    struct mock_task_struct * mock_task, * mock_task_ = NULL;

    int first = 1;

    for_each_process(task){

        mock_task_ = create_mock_task(task);

        if(!mock_task_){
            printk(KERN_ERR "Error! Task could not be retrieved!\n");
            return NULL;
        }

        if(first > 0){
            first = 0;
            mock_task = mock_task_;
        }

        mock_task_ = mock_task_->next;
    }

    return mock_task;
}