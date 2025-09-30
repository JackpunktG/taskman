#ifndef TASK_OPS_H
#define TASK_OPS_H

// Task operations
void task_insert(char *input);
void task_show(char *arg, char option);
void task_complete(char *id);
void task_postpone(char *id, char *whenSt);
void remove_task(char *id);
void set_config();

#endif // TASK_OPS_H
