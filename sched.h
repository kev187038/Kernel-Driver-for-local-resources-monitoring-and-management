#ifndef SCHED_H
#define SCHED_H

#include <linux/types.h> //For kernel-specific types like pid_t and u64
#include <linux/list.h>  

//Recreated most similar structure to task_struct used to extract the information

typedef struct mock_task_struct {
    //Process Identity 
    pid_t pid;         //Process ID
    pid_t tgid;        //Thread Group ID
    char *comm;     //Name of the process


    //CPU Scheduling
    long state;        //Current state of the process (running, sleeping, waiting)
    int prio;          //Process priority
    int policy;        //Scheduling policy (SCHED_FIFO for first-in-first-out, SCHED_RR for roundrobin)

    //Memory Usage physical and virtual
    unsigned int vmemory;

    //Resource Usage (times are in nanoseconds)
    u64 utime;         //Time spent in user mode
    u64 stime;         //Time spent in kernel mode 
    unsigned int io_rbytes; //Disk I/O statistics
    unsigned int io_wbytes;

    struct mock_task_struct * next;

};

#endif