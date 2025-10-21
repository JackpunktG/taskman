#include "../include/taskman.h"
#include "task_ops.h"
#include "date_utils.h"
#include "../lib/sqlitedb/sqlitedb.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void run_args(char **args)
{
    //******************************************************************************
    //task Args
    //******************************************************************************
    if (strcmp(args[1], "-n") == 0)
    {
        task_insert(args[2]);
    }
    else if (strcmp(args[1], "-s") == 0)
    {
        task_show(args[2], TASK_SHOW_RANGE);
    }
    else if(strcmp(args[1], "-id") == 0)
    {
        task_show(args[2], TASK_SHOW_ID);
    }
    else if (strcmp(args[1], "-s!") == 0)
    {
        task_show(args[2], TASK_SHOW_SPECIFIC_DATE);
    }
    else if (strcmp(args[1], "-c") == 0)
    {
        task_complete(args[2]);
    }
    else if (strcmp(args[1], "-a") == 0)
    {
        task_show(args[2], TASK_SHOW_APPIONTMENT);
    }
    else if (strcmp(args[1], "-sa") == 0)
    {
        task_show(NULL, TASK_SHOW_ALL);
    }
    else if (strcmp(args[1], "-p") == 0)
    {
        task_postpone(args[2], args[3]);
    }
    else if (strcmp(args[1], "--delay") == 0)
    {
        if (args[2] == NULL && args[3] == NULL) printf("ERROR - Missing time and or ID arg\n");
        else delay_task(args[2], atoi(args[3]));
    }
    else if (strcmp(args[1], "-rm") == 0)
    {
        remove_task(args[2]);
    }
    else if(strcmp(args[1], "task") == 0 || strcmp(args[1], "tasks") == 0)
    {
        task_show(NULL, TASK_SHOW_GENERAL_TASKS);
    }
    //******************************************************************************
    //recurring task Args
    //******************************************************************************
    else if (strcmp(args[1], "-nr") == 0)
    {
        if (args[2] == NULL) recurring_task_input_guide();
        else recurring_task_insert(args[2]);
    }
    else if(strcmp(args[1], "-sr!") == 0)
    {
        recurring_task_show(args[2], TASK_SHOW_SPECIFIC_DATE);
    }
    else if (strcmp(args[1], "-rmr") == 0)
    {
        remove_recurring_task(args[2]);
    }
    else if(strcmp(args[1], "-idr") == 0)
    {
        recurring_task_show(args[2], TASK_SHOW_ID);
    }
    else if (strcmp(args[1], "-sra") == 0)
    {
        recurring_task_show(NULL, TASK_SHOW_ALL);
    }
    //******************************************************************************
    //Combination and outlook Args
    //******************************************************************************
    else if(strcmp(args[1], "today") == 0 || strcmp(args[1], "tdy") == 0 || strcmp(args[1], "t") == 0)
    {
        task_outlook(OUTLOOK_TODAY);
    }
    else if(strcmp(args[1], "tomorrow") == 0 || strcmp(args[1], "tom") == 0)
    {
        task_outlook(OUTLOOK_TOMORROW);
    }
    else if(strcmp(args[1], "week") == 0 || strcmp(args[1], "weekly") == 0 || strcmp(args[1], "wk") == 0)
    {
        task_outlook(OUTLOOK_WEEKLY);
    }
    else if(strcmp(args[1], "fort") == 0 || strcmp(args[1], "fortnightly") == 0)
    {
        task_outlook(OUTLOOK_FORTNIGHTLY);
    }
    else if(strcmp(args[1], "month") == 0 || strcmp(args[1], "monthly") == 0 || strcmp(args[1], "mth") == 0)
    {
        task_outlook(OUTLOOK_MONTHLY);
    }
    else if(strcmp(args[1], "quarterly") == 0 || strcmp(args[1], "quart") == 0)
    {
        task_outlook(OUTLOOK_QUARTERLY);
    }
    else if (strcmp(args[1], "-check") == 0)
    {
        task_show(NULL, TASK_SHOW_REMINDER);
        recurring_task_show("today", TASK_SHOW_REMINDER);
    }
    //******************************************************************************
    //misc. Args
    //******************************************************************************
    else if (strcmp(args[1], "-sql") == 0)
    {
        direct_sql(DB_PATH, args[2]);
    }
    else if (strcmp(args[1], "-h") == 0 || strcmp(args[1], "--help") == 0)
    {
        print_help();
    }
    else
    {
        printf("INVALID args\n");
    }
}

int main(int argc, char *args[])
{
    if (access(DB_PATH, F_OK) != 0)
    {
        printf("Cannot find database\nIf this is your first time, no worries. We'll create one :)\n");
        set_config();
    }

    if (argc > 1)
    {
        run_args(args);
    }
    else
    {
        printf("No args provided\n");
        printf("Use -h or --help for help\n");
    }

    return 0;
}
