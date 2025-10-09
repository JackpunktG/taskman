#ifndef TASK_OPS_H
#define TASK_OPS_H

// Task operations
void task_outlook(char option);
void task_insert(char *input);
void recurring_task_insert(char *taskArgs);
void recurring_task_input_guide();
void recurring_task_show(char *arg, char option);
void task_show(char *arg, char option);
void task_complete(char *id);
void task_postpone(char *id, char *whenSt);

void remove_recurring_task(char *id);
//will also set the completion back to 0 to be active again, in case of accidental completion
void delay_task(char *id, int timeInMinutes);
void remove_task(char *id);
void set_config();
void print_help();

#endif // TASK_OPS_H
