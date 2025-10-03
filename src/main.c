#include "../include/taskman.h"
#include "task_ops.h"
#include "date_utils.h"
#include "../lib/sqlitedb/sqlitedb.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void run_args(char **args)
{
    if (strcmp(args[1], "-n") == 0)
    {
        task_insert(args[2]);
    }
    else if (strcmp(args[1], "-s") == 0)
    {
        task_show(args[2], 's');
    }
    else if(strcmp(args[1], "today") == 0)
    {
        task_show("0", 's');
    }
    else if(strcmp(args[1], "tomorrow") == 0 || strcmp(args[1], "tom") == 0)
    {
        task_show("1", 's');
    }
    else if(strcmp(args[1], "week") == 0)
    {
        task_show("7", 's');
    }
    else if(strcmp(args[1], "-id") == 0)
    {
        task_show(args[2], '!');
    }
    else if (strcmp(args[1], "-s!") == 0)
    {
        task_show(args[2], 'o');
    }
    else if (strcmp(args[1], "-c") == 0)
    {
        task_complete(args[2]);
    }
    else if (strcmp(args[1], "-a") == 0)
    {
        task_show(args[2], 'a');
    }
    else if (strcmp(args[1], "-check") == 0)
    {
        task_show("", 'r');
    }
    else if (strcmp(args[1], "-sa") == 0)
    {
        task_show("", '.');
    }
    else if (strcmp(args[1], "-p") == 0)
    {
        task_postpone(args[2], args[3]);
    }
    else if (strcmp(args[1], "-rm") == 0)
    {
        remove_task(args[2]);
    }
    else if (strcmp(args[1], "-sql") == 0)
    {
        direct_sql("tasks.db", args[2]);
    }
    else
    {
        printf("INVALID args\n");
    }
}

int main(int argc, char *args[])
{
    if (access("tasks.db", F_OK) != 0)
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
        printf("Usage: taskman [options]\n");
        printf("Try 'taskman -s 0' to see today's tasks\n");
    }

    return 0;
}
