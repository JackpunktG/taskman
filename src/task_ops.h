#ifndef TASK_OPS_H
#define TASK_OPS_H

/* insert tasks are explained in the print_help function in the source code*/


enum TASK_SHOW_OPTION
{
    TASK_SHOW_RANGE,
    TASK_SHOW_SPECIFIC_DATE,
    TASK_SHOW_APPIONTMENT,
    TASK_SHOW_ID,
    TASK_SHOW_GENERAL_TASKS,
    TASK_SHOW_LAST_ENTRY,
    TASK_SHOW_REMINDER,
    TASK_SHOW_MONTHLY,
    TASK_SHOW_WEEKLY,
    TASK_SHOW_ALL
};

// Task operation functions
void task_insert(char *input);
void task_show(char *arg, enum TASK_SHOW_OPTION option);
void task_complete(char *id);
void task_postpone(char *id, char *whenSt);
void remove_task(char *id);

//will also set the completion back to 0 to be active again, in case of accidental completion
//works for both general and recurring tasks
void delay_task(char *id, int timeInMinutes);

//recuring task operation functions
void recurring_task_insert(char *taskArgs);
void recurring_task_input_guide();
void recurring_task_show(char *arg, enum TASK_SHOW_OPTION option);
void remove_recurring_task(char *id);


//outlook function
enum OUTLOOK_OPTION
{
    OUTLOOK_TODAY,
    OUTLOOK_TOMORROW,
    OUTLOOK_WEEKLY,
    OUTLOOK_FORTNIGHTLY,
    OUTLOOK_MONTHLY,
    OUTLOOK_QUARTERLY
};
void task_outlook(enum OUTLOOK_OPTION option);

//Setup and help functions
void set_config();
void print_help();

#endif // TASK_OPS_H
