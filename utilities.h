#ifndef UTILITIES_H
#define UTILITIES_H

#include "sched.h"

struct mock_task_struct *create_mock_task(struct task_struct *task);
struct mock_task_struct *create_mock_tasks(void);
void free_mock_tasks(struct mock_task_struct *task_list);

#endif
