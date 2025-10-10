#include "task_ops.h"
#include "date_utils.h"
#include "string_utils.h"
#include "../lib/sqlitedb/sqlitedb.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>


//************************************************************************************
//task functions
//************************************************************************************

int range_search(char *arg, int currentDay, char **sqlQuery, char ***input, uint8_t **type)
{
    char temp[3];
    sprintf(temp, "%d", currentDay);
    char *dateString;
    int dateCheck = input_where(temp, &dateString);
    if (dateCheck != 1)
    {
        if (dateCheck == -2) printf("ERROR - invaild date\n");
        else printf("ERROR - Invalid date string\n");
        return -1;
    }

    int range = atoi(arg);

    struct tm t = {0};
    sscanf(dateString, "%4d-%2d-%2d", &t.tm_year, &t.tm_mon, &t.tm_mday);
    t.tm_year -= 1900;  // struct tm years since 1900
    t.tm_mon -= 1;      // struct tm months 0-11
    // Convert to timestamp
    time_t timestamp = mktime(&t);
    t.tm_mday += 1 + range;                  // to get to whole day
    time_t timestamp1 = mktime(&t);  // to get the whole day

    *sqlQuery = strdup("SELECT * FROM tasks WHERE (date >= ? AND date < ?) OR date IS NULL ORDER BY date, time");

    *input = malloc(2 * sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }
    char buffer1[32];
    char buffer2[32];
    snprintf(buffer1, sizeof(buffer1), "%ld", timestamp);
    snprintf(buffer2, sizeof(buffer2), "%ld", timestamp1);

    (*input)[0] = strdup(buffer1);
    (*input)[1] = strdup(buffer2);

    *type = malloc(2 * sizeof(uint8_t));
    if (*type == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*type)[0] = 2;
    (*type)[1] = 2;

    free(dateString);

    return 2;
}

int day_search(char *arg, char **sqlQuery, char ***input, uint8_t **type)
{
    char *token = strtok(arg, "|");
    trim(token);

    char *argString = malloc(strlen(token) + 1);
    strcpy(argString, token);

    token = strtok(NULL, "|");
    char *dateString;
    int dateCheck = input_where(argString, &dateString);
    if (dateCheck != 1)
    {
        if (dateCheck == -2) printf("ERROR - invaild date\n");
        else printf("ERROR - Invalid date string\n");

        return -1;
    }

    struct tm t = {0};
    sscanf(dateString, "%4d-%2d-%2d", &t.tm_year, &t.tm_mon, &t.tm_mday);
    t.tm_year -= 1900;  // struct tm years since 1900
    t.tm_mon -= 1;      // struct tm months 0-11
    // Convert to timestamp

    time_t timestamp = mktime(&t);
    t.tm_mday += 1;                  // to get to whole day
    time_t timestamp1 = mktime(&t);  // to get the whole day


    *sqlQuery = strdup("SELECT * FROM tasks WHERE date >= ? AND date < ? ORDER BY time");

    *input = malloc(2 * sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }
    char buffer1[32];
    char buffer2[32];
    snprintf(buffer1, sizeof(buffer1), "%ld", timestamp);
    snprintf(buffer2, sizeof(buffer2), "%ld", timestamp1);

    (*input)[0] = strdup(buffer1);
    (*input)[1] = strdup(buffer2);

    *type = malloc(2 * sizeof(uint8_t));
    if (*type == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*type)[0] = 2;
    (*type)[1] = 2;

    free(dateString);

    return 2;
}

int appointment_search(char *arg, char **sqlQuery, char ***input, uint8_t **type)
{
    char *token = strtok(arg, "|");
    trim(token);

    char *argString = malloc(strlen(token) + 1);
    strcpy(argString, token);

    char *dateString;
    int dateCheck = input_where(argString, &dateString);
    if (dateCheck != 1)
    {
        if (dateCheck == -2) printf("ERROR - invaild date\n");
        else printf("ERROR - Invalid date string\n");

        return -1;
    }

    struct tm t = {0};
    sscanf(dateString, "%4d-%2d-%2d", &t.tm_year, &t.tm_mon, &t.tm_mday);
    t.tm_year -= 1900;  // struct tm years since 1900
    t.tm_mon -= 1;      // struct tm months 0-11
    // Convert to timestamp
    time_t timestamp = mktime(&t);
    t.tm_mday += 1;                  // to get to whole day
    time_t timestamp1 = mktime(&t);  // to get the whole day


    *sqlQuery = strdup("SELECT * FROM tasks WHERE date >= ? AND date < ? AND time NOT NULL ORDER BY time");

    *input = malloc(2 * sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }
    char buffer1[32];
    char buffer2[32];
    snprintf(buffer1, sizeof(buffer1), "%ld", timestamp);
    snprintf(buffer2, sizeof(buffer2), "%ld", timestamp1);

    (*input)[0] = strdup(buffer1);
    (*input)[1] = strdup(buffer2);

    *type = malloc(2 * sizeof(uint8_t));
    if (*type == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*type)[0] = 2;
    (*type)[1] = 2;

    free(dateString);

    return 2;
}

int reminder_search(int currentDay, int currentMonth, int currentYear, char **sqlQuery, char ***input, uint8_t **type)
{
    struct tm t = {0};
    t.tm_mon = currentMonth;
    t.tm_mday = currentDay;
    t.tm_year = currentYear;
    time_t timestamp = mktime(&t);
    t.tm_mday += 1;                  // to get to whole day
    time_t timestamp1 = mktime(&t);  // to get the whole day

    // Get current time in seconds since midnight
    time_t now = time(NULL);
    struct tm *now_tm = localtime(&now);
    int seconds_now =
        now_tm->tm_hour * 3600 + now_tm->tm_min * 60 + now_tm->tm_sec;
    int seconds_next = seconds_now + 15 * 60;

    *sqlQuery = strdup("SELECT * FROM tasks WHERE date >= ? AND date < ? AND time >= ? AND time < ? AND done = 0");

    *input = malloc(4 * sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }
    char buffer1[32];
    char buffer2[32];
    char buffer3[32];
    char buffer4[32];
    snprintf(buffer1, sizeof(buffer1), "%ld", timestamp);
    snprintf(buffer2, sizeof(buffer2), "%ld", timestamp1);
    snprintf(buffer3, sizeof(buffer3), "%d", seconds_now);
    snprintf(buffer4, sizeof(buffer4), "%d", seconds_next);

    (*input)[0] = strdup(buffer1);
    (*input)[1] = strdup(buffer2);
    (*input)[2] = strdup(buffer3);
    (*input)[3] = strdup(buffer4);

    *type = malloc(4 * sizeof(uint8_t));
    if (*type == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*type)[0] = 2;
    (*type)[1] = 2;
    (*type)[2] = 2;
    (*type)[3] = 2;

    return 4;


}

int single_search_ID(char *arg, char **sqlQuery, char ***input, uint8_t **type)
{

    *sqlQuery = strdup("SELECT * FROM tasks WHERE id = ?");

    *input = malloc(sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*input)[0] = arg;

    *type = malloc(sizeof(uint8_t));
    if (*type == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*type)[0] = 2;

    return 1;
}

char *process_line(char *task, char *id)
{
    char *buffer = strdup(task);
    char taskID[8] = {0};
    char dateString[16] = {0};
    char timeString[16] = {0};
    char taskString[1024] = {0};
    bool completion;

    uint32_t field = 1;
    uint32_t letterCount = 0;

    for(int i = 0; buffer[i] != '\0'; i++)
    {
        if (buffer[i] == '|')
        {
            if (field == 1)
            {
                memcpy(taskID, buffer, i);
                taskID[i+1] = '\0';
                letterCount = i + 1;
                field++;
            }
            else if (field == 2)
            {
                memcpy(dateString, buffer + letterCount, i - letterCount);
                dateString[i-letterCount + 1] = '\0';
                letterCount = i + 1;
                field++;
            }
            else if (field == 3)
            {
                memcpy(timeString, buffer + letterCount, i - letterCount);
                timeString[i - letterCount + 1] = '\0';
                letterCount = i + 1;
                field++;
            }
            else if (field == 4)
            {
                memcpy(taskString, buffer + letterCount, i  - letterCount);
                taskString[i - letterCount +1] = '\0';
                letterCount = i + 1;
                field++;
            }
            else if (field == 5)
            {
                field++;
            }
            else if (field == 6)
            {
                completion = (buffer[i-1] == '0' ) ? false : true; //field 6

            }
        }
    }
    bool generalTask = false;
    if (strcmp(dateString, "NULL") == 0)
    {
        strcpy(dateString, "task");
        generalTask = true;
    }

    char *date;
    date = date_string_from_timestamp(atoi(dateString));

    int size;
    char *result;

    if (strcmp(timeString, "NULL") == 0)
    {
        size = snprintf(NULL, 0, "%.5s -> %s [%s] id:%s\n", generalTask ? dateString : date, taskString, completion ? "DONE" : "WORKING", taskID);
        result = malloc(size + 1);
        snprintf(result, size + 1, "%.5s -> %s [%s] id:%s\n", generalTask ? dateString : date, taskString, completion ? "DONE" : "WORKING", taskID);
    }
    else
    {
        uint32_t hour;
        uint32_t min;
        hour = atoi(timeString);
        min = (hour % 3600) / 60;
        hour = hour / 3600;
        size = snprintf(NULL, 0, "%.5s %d:%02d -> %s [%s] id:%s\n", generalTask ? dateString : date, hour, min, taskString, completion ? "DONE" : "APPOINTMENT", taskID);
        result = malloc(size + 1);
        snprintf(result, size + 1, "%.5s %d:%02d -> %s [%s] id:%s\n", generalTask ? dateString : date, hour, min, taskString, completion ? "DONE" : "APPOINTMENT", taskID);
    }
    free(buffer);
    trim(taskID);

    strcpy(id, taskID);
    return result;
}



void print_task(char *result)
{
    char *buffer = strdup(result);

    char *line = strtok(buffer, "\n");

    while (line != NULL)
    {
        char id [8];
        char *task = process_line(line, id);
        printf("%s", task);

        free(task);
        line = strtok(NULL, "\n");
    }

    free(buffer);

}


/*void pop_up(char *result)   //IN SOURCE CODE CALLING OF XMESSAGE
{
    char *buffer = strdup(result);

    char *line = strtok(buffer, "\n");

    while (line != NULL)
    {
        char id[8];
        char *task = process_line(line, id);
        size_t len = strlen(task);
        size_t cmdSize = snprintf(NULL, 0, "xmessage -buttons '-Complete-:0,-Delay5min-:5,-Delay10min-:10,-Delay15min-:15,-Delay30min-:30' 'Termin in 15 minutes!!\n\n%s'", task);
        char *cmd = malloc(cmdSize + len + 1);
        snprintf(cmd, cmdSize + len +1, "xmessage -buttons '-Complete-:0,-Delay5min-:5,-Delay10min-:10,-Delay15min-:15,-Delay30min-:30' 'Termin in 15 minutes!!\n\n%s'", task);
        if (cmd != NULL)
        {
            int message = system(cmd);
            {
                switch (message)
                {
                case 0:
                    task_complete(id);
                    break;
                case 1280:
                    delay_task(id, 5);
                    break;
                case 2560:
                    delay_task(id, 10);
                    break;
                case 3840:
                    delay_task(id, 15);
                    break;
                case 7680:
                    delay_task(id, 30);
                    break;
                default:
                    printf("No action taken\n");
                    break;
                }
            }
        }
        else
        {
            printf("ERROR - allocation of memory for pop up command");
        }

        free(task);
        line = strtok(NULL, "\n");
    }
    free(buffer);
}
*/
void pop_up(char *result)
{
    char *buffer = strdup(result);

    char *line = strtok(buffer, "\n");

    while (line != NULL)
    {
        char id[8];
        char *task = process_line(line, id);

        printf("Reminder!!!\nappointment in 15min: %s", task);
        int option;
        printf("Options:\n1. Complete task (0)\n2. Delay task by n minutes (n)\n");
        scanf("%d", &option);
        getchar();

        if (option > 0)
        {
            delay_task(id, option);
        }
        else
        {
            task_complete(id);
        }

        free(task);
        line = strtok(NULL, "\n");
    }
    free(buffer);
}

void task_show(char *arg, char option)
{
//return for n how many inputs for freeing in task_show
    uint32_t n;
    time_t t;
    struct tm tm;
    t = time(NULL);
    tm = *localtime(&t);


    char *sqlQuery = NULL;
    char **input = NULL;
    uint8_t *type = NULL;

    switch (option)
    {
    case 's':
        n = range_search(arg, tm.tm_mday, &sqlQuery, &input, &type); //outlook of arg days
        break;
    case 'o':
        n = day_search(arg, &sqlQuery, &input, &type); //specific day search
        break;
    case 'a':
        n = appointment_search(arg, &sqlQuery, &input, &type); //only getting tasks with times
        break;
    case 'r':
        n = reminder_search(tm.tm_mday, tm.tm_mon, tm.tm_year, &sqlQuery, &input, &type); //checking for appointments in the next 15mins
        break;
    case '!':
        n = single_search_ID(arg, &sqlQuery, &input, &type); //searching for task with given unique ID
        break;
    case 't':
        n = 0;
        sqlQuery = strdup("SELECT * FROM tasks WHERE date IS NULL AND done = 0");
        break;
    case '.':
        n = 0;
        sqlQuery = strdup("SELECT * FROM tasks ORDER BY date"); //prints entire table
        break;
    case 'l':
        n = 0;
        sqlQuery = strdup("SELECT * FROM tasks ORDER BY ID DESC LIMIT 1"); //prints last added task
        break;
    default:
        printf("ERROR - could not find options for task_show");
        break;
    }

    if (n >= 0)
    {
        char *result = db_stmt_build_execute_string_return(n, input, type, sqlQuery, DB_PATH);

        //printf("%s", result);
        option != 'r' ? print_task(result) : pop_up(result);

        if (n > 1)
        {
            for(int i = 0; i < n; i++)
            {
                free((input)[i]);
            }
        }

        free(input);
        free(type);
        free(sqlQuery);

        free(result);
    }
    else return;
}

void task_insert(char *dateAndTimeArg)
{
    char *token = strtok(dateAndTimeArg, "|");

    trim(token);
    char *taskString = malloc(strlen(token) + 1);
    strcpy(taskString, token);

    token = strtok(NULL, "|");
    if (token == NULL)      //quick path to add general task
    {
        const char *sqlQuery = "INSERT INTO tasks(task, date_added) VALUES(?, strftime('%s', 'now'))";
        const char *input[] = { taskString };
        uint8_t type[] = { 1 };
        db_stmt_build_execute(1, input, type, sqlQuery, DB_PATH);

        task_show(NULL, 'l');
        printf("ADDED\n");
        free(taskString);
        return;
    }

    trim(token);
    char *temp = malloc(strlen(token) + 1);
    strcpy(temp, token);

    token = strtok(NULL, "|");
    char *dateString;
    int dateCheck = input_where(temp, &dateString);
    if (dateCheck != 1)
    {
        if (dateCheck == -2) printf("ERROR - invaild date\n");
        else printf("ERROR - Invalid date string\n");

        free(temp);
        free(taskString);
        return;
    }
    else
    {
        if (token != NULL)
        {
            trim(token);
            char *timeString = malloc(strlen(token) + 1);

            strcpy(timeString, token);
            int timeCheck = time_input_helper(&timeString);

            if (timeCheck != 1)
            {
                printf("ERROR - Invalid time input\n");
                free(temp);
                free(taskString);
                free(dateString);
                return;
            }
            else
            {
                char *sqlQuery = "INSERT INTO tasks(date, time, task, date_added) "
                                 "VALUES(strftime('%s', ?, 'start of day'), strftime('%s', "
                                 "?, ?) % 86400, ?, strftime('%s', 'now'))";

                const char *input[] = { dateString, dateString, timeString, taskString };
                uint8_t type[] = { 1, 1, 1, 1 };
                db_stmt_build_execute(4, input, type, sqlQuery, DB_PATH);
            }

        }
        else
        {
            char *sqlQuery = "INSERT INTO tasks(date, task, date_added) VALUES(strftime('%s', "
                             "?, 'start of day'), ?, strftime('%s', 'now'))";

            const char *input[] = { dateString, taskString };
            uint8_t type[] = { 1, 1 };
            db_stmt_build_execute(2, input, type, sqlQuery, DB_PATH);
        }
    }

    task_show(NULL, 'l');
    printf("ADDED\n");

    free(temp);
    free(dateString);
    free(taskString);
}



void task_complete(char *id) //when postponing general tasks, they get the date of completion on the date
{
    char *sqlQuery = "UPDATE tasks SET done = 1, "
                     "date = CASE WHEN date IS NULL THEN strftime('%s', 'now') "
                     "ELSE date END "
                     "WHERE id = ?";
    const char *input[] = { id };
    uint8_t type[] = { 2 };

    db_stmt_build_execute(1, input, type, sqlQuery, DB_PATH);

    task_show(id, '!');
}


void delay_task(char *id, int timeInMinutes)
{
    uint32_t addition = timeInMinutes * 60;
    char additionStr[16] = {0};
    sprintf(additionStr, "%d", addition);
    char *sqlQuery = "UPDATE tasks SET time = time + ?, done = 0 WHERE id = ?";
    const char *input[] = { additionStr, id };
    uint8_t type[] = { 1, 2 };


    db_stmt_build_execute(2, input, type, sqlQuery, DB_PATH);

    task_show(id, '!');
    printf("DELAYED\n");
}

void task_postpone(char *id, char *dateAndTimeArg)
{
    char *token = strtok(dateAndTimeArg, "|");
    trim(token);

    char *tmp = malloc(strlen(token) + 1);
    strcpy(tmp, token);

    token = strtok(NULL, "|"); //calling strtok before input_where to not modify *tmp
    char *dateString;
    int dateCheck = input_where(tmp, &dateString);
    if (dateCheck != 1)
    {
        if (dateCheck == -2) printf("ERROR - invaild date\n");
        else printf("ERROR - Invalid date string\n");

        return;
    }
    else
    {
        if (token != NULL)
        {
            trim(token);
            char *timeString = malloc(strlen(token) + 1);

            strcpy(timeString, token);
            int timeCheck = time_input_helper(&timeString);
            printf("Time check: %d\n", timeCheck);
            printf("Time string: %s\n", timeString);

            if (timeCheck != 1)
            {
                printf("ERROR - Invalid time input\n");
                return;
            }
            else
            {
                char *sqlQuery = "UPDATE tasks SET date = strftime('%s', ?, 'start of day'), "
                                 "time = strftime('%s', ?, ?) % 86400 WHERE id = ?";

                const char *input[] = { dateString, dateString, timeString, id };
                uint8_t type[] = { 1, 1, 1, 2 };
                db_stmt_build_execute(4, input, type, sqlQuery, DB_PATH);

                free(timeString);
            }
        }
        else
        {
            char *sqlQuery = "UPDATE tasks SET date = strftime('%s', ?, 'start of day') WHERE id = ?";

            const char *input[] = { dateString, id };
            uint8_t type[] = { 1, 2 };
            db_stmt_build_execute(2, input, type, sqlQuery, DB_PATH);
        }
    }

    task_show(id, '!');
    printf("UPDATED\n");


    free(dateString);
    free(tmp);
}

void remove_task(char *id)
{
    printf("Are you sure you want to remove task:\n");
    task_show(id, '!');

    printf("(y/n)?\n");
    char response = getchar();
    getchar();
    if (response != 'y' && response != 'Y')
    {
        printf("Task removal cancelled.\n");
        return;
    }

    const char *sqlQuery = "DELETE FROM tasks WHERE id = ?";
    const char *input[] = { id };
    uint8_t type[] = { 2 };
    db_stmt_build_execute(1, input, type, sqlQuery, DB_PATH);
}

//************************************************************************************
//recurring tasks
//************************************************************************************


int recurring_day_search(char *dateString, char **sqlQuery, char ***input, uint8_t **type)
{
    *sqlQuery = strdup("SELECT * FROM recurring_tasks WHERE date_begin <= ? AND (date_expires >= ? OR date_expires IS NULL) ORDER BY time");

    *input = malloc(2 * sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*input)[0] = strdup(dateString);
    (*input)[1] = strdup(dateString);

    *type = malloc(sizeof(uint8_t));
    if (*type == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*type)[0] = 1;
    (*type)[1] = 1;

    return 2;
}

int recurring_timed_task_check(char *dateString, char **sqlQuery, char ***input, uint8_t **type)
{
    uint32_t time = seconds_since_midnight_minAccuracy();
    time = (time + 15 * 60) % 86400;
    char timeStr[16] = {0};
    sprintf(timeStr, "%d", time);

    *sqlQuery = strdup("SELECT * FROM recurring_tasks WHERE date_begin <= ? AND (date_expires >= ? OR date_expires IS NULL) AND time = ? ORDER BY time");

    *input = malloc(3 * sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*input)[0] = strdup(dateString);
    (*input)[1] = strdup(dateString);
    (*input)[2] = strdup(timeStr);

    *type = malloc(sizeof(uint8_t));
    if (*type == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*type)[0] = 1;
    (*type)[1] = 1;
    (*type)[2] = 2;

    return 3;
}

int recurring_single_search_ID(char *id, char **sqlQuery, char ***input, uint8_t **type)
{
    *sqlQuery = strdup("SELECT * FROM recurring_tasks WHERE id = ?");

    *input = malloc(sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }
    (*input)[0] = id;

    *type = malloc(sizeof(uint8_t));
    if (*type == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }
    (*type)[0] = 2;

    return 1;
}

int recurring_task_only_month(char *dateString, char **sqlQuery, char ***input, uint8_t **type)
{
    *sqlQuery = strdup("SELECT * FROM recurring_tasks WHERE frequency = 'monthly' AND date_begin <= ? AND (date_expires >= ? OR date_expires IS NULL) ORDER BY time");

    *input = malloc(2 * sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*input)[0] = strdup(dateString);
    (*input)[1] = strdup(dateString);

    *type = malloc(sizeof(uint8_t));
    if (*type == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*type)[0] = 1;
    (*type)[1] = 1;

    return 2;
}

int recurring_task_only_week(char *dateString, char **sqlQuery, char ***input, uint8_t **type)
{
    *sqlQuery = strdup("SELECT * FROM recurring_tasks WHERE frequency = 'weekly' AND date_begin <= ? AND (date_expires >= ? OR date_expires IS NULL) ORDER BY time");

    *input = malloc(2 * sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*input)[0] = strdup(dateString);
    (*input)[1] = strdup(dateString);

    *type = malloc(sizeof(uint8_t));
    if (*type == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return -1;
    }

    (*type)[0] = 1;
    (*type)[1] = 1;

    return 2;
}

/*void recurring_pop_up(char *result)       //IN SOURCE CODE CALLING OF XMESSAGE
{

    size_t len = strlen(result);
    size_t cmdSize = snprintf(NULL, 0, "xmessage -buttons '-Okay-:0' 'Standing date in 15 minutes!!\n\n%s'", result);
    char *cmd = malloc(cmdSize + len + 1);
    snprintf(cmd, cmdSize + len +1, "xmessage -buttons '-Okay-:0' 'Standing date in 15 minutes!!\n\n%s'", result);
    if (cmd != NULL)
    {
        int message = system(cmd);
        free(cmd);
    }
    else
    {
        printf("ERROR - allocation of memory for pop up command");
    }
}*/

void recurring_pop_up(char *result)
{
    printf("Reminder!!!\nstanding appointment in 15min: %s\n", result);
}

bool is_recurring_task_valid_on_given_day(char *date, char *frequency, char *searchDate)
{
    //printf("\n------\ndate - %s\nfrequency - %s\narg - %s\n", date, frequency, searchDate);
    if (strcmp(frequency, "daily") == 0) return true;

    if (strcmp(frequency, "monthly") == 0)
    {
        uint16_t year;
        uint8_t month;
        uint8_t day;

        uint8_t dateTest = atoi(date);
        sscanf(searchDate, "%hu-%hhu-%hhu", &year, &month, &day);

        if (day == dateTest) return true;
        else return false;
    }

    if (strcmp(frequency, "weekly") == 0)
    {
        struct tm tm = {0};
        sscanf(searchDate, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        mktime(&tm);

        int weekday= atoi(date);

        if (tm.tm_wday == weekday) return true;
        else return false;

    }
    return false;
}

char *validate_recurring_task(char *line, char *arg, char option, char *searchDate, bool *valid, char *id)
{
    char *buffer = strdup(line);
    char taskID[8] = {0};
    char taskString[1024] = {0};
    char date[8] = {0};
    char frequency[16] = {0};
    char firstOccurence[12] = {0};
    char expires[12] = {0};
    char timeString[16] = {0};

    uint32_t field = 1;
    uint32_t letterCount = 0;

    for(int i = 0; buffer[i] != '\0'; i++)
    {
        if (buffer[i] == '|')
        {
            if (field == 1)
            {
                memcpy(taskID, buffer, i);
                taskID[i+1] = '\0';
                letterCount = i + 1;
                strcpy(id, taskID);
                field++;
            }
            else if (field == 2)
            {
                memcpy(frequency, buffer + letterCount, i - letterCount);
                frequency[i-letterCount + 1] = '\0';
                letterCount = i + 1;
                field++;
            }
            else if (field == 3)
            {
                memcpy(date, buffer + letterCount, i - letterCount);
                date[i - letterCount + 1] = '\0';
                letterCount = i + 1;
                field++;
            }
            else if (field == 4)
            {
                memcpy(timeString, buffer + letterCount, i  - letterCount);
                timeString[i - letterCount +1] = '\0';
                letterCount = i + 1;
                field++;
            }
            else if (field == 5)
            {
                memcpy(taskString, buffer + letterCount, i  - letterCount);
                taskString[i - letterCount +1] = '\0';
                letterCount = i + 1;
                field++;
            }
            else if (field == 6)
            {
                memcpy(firstOccurence, buffer + letterCount, i  - letterCount);
                firstOccurence[i - letterCount +1] = '\0';
                letterCount = i + 1;
                field++;
            }
            else if (field == 7)
            {
                memcpy(expires, buffer + letterCount, i  - letterCount);
                expires[i - letterCount +1] = '\0';
                letterCount = i + 1;
                field++;
            }
        }
    }


    char day[3];
    char month[3];
    if (option == 'o' || option == 'm' || option == 'w' || option == 'r')
        *valid = is_recurring_task_valid_on_given_day(date, frequency, searchDate) ? true : false;
    else *valid = true;


    char *result;
    if (valid)
    {
        if (option == '!' || option == 'l') //non day selected print out all of
        {
            memcpy(day, firstOccurence + 8, 2);
            memcpy(month, firstOccurence + 5, 2);

            if (strcmp(timeString, "NULL") == 0)
            {
                int size = snprintf(NULL, 0, "* [%s] -> %s {%s.%s} {%s} id:%s", frequency, taskString, day, month, strcmp(expires, "NULL") == 0 ? "indefinate" : expires, taskID);
                result = malloc(size + 1);
                snprintf(result, size +1, "* [%s] -> %s {%s.%s} {%s} id:%s", frequency, taskString, day, month, strcmp(expires, "NULL") == 0 ? "indefinate" : expires, taskID);
            }
            else
            {
                uint32_t hour;
                uint32_t min;
                hour = atoi(timeString);
                min = (hour % 3600) / 60;
                hour = hour / 3600;
                int size = snprintf(NULL, 0, "* [%s] @ %d:%02d -> %s {%s.%s} {%s} id:%s", frequency, hour, min, taskString, day, month, strcmp(expires, "NULL") == 0 ? "indefinate" : expires, taskID);
                result = malloc(size + 1);
                snprintf(result, size +1, "* [%s] @ %d:%02d -> %s {%s.%s} {%s} id:%s", frequency, hour, min, taskString, day, month, strcmp(expires, "NULL") == 0 ? "indefinate" : expires, taskID);
            }
        }
        else if (strcmp(timeString, "NULL") == 0)
        {
            memcpy(day, searchDate + 8, 2);
            memcpy(month, searchDate + 5, 2);

            int size = snprintf(NULL, 0, "*%s.%s -> %s [%s] {%s} id:%s", day, month, taskString, frequency, strcmp(expires, "NULL") == 0 ? "indefinate" : expires, taskID);
            result = malloc(size + 1);
            snprintf(result, size + 1, "*%s.%s -> %s [%s] {%s} id:%s", day, month, taskString, frequency, strcmp(expires, "NULL") == 0 ? "indefinate" : expires, taskID);
        }
        else
        {
            memcpy(day, searchDate + 8, 2);
            memcpy(month, searchDate + 5, 2);

            uint32_t hour;
            uint32_t min;
            hour = atoi(timeString);
            min = (hour % 3600) / 60;
            hour = hour / 3600;
            int size = snprintf(NULL, 0, "*%s.%s %d:%02d -> %s [%s] {%s} id:%s", day, month, hour, min, taskString, frequency, strcmp(expires, "NULL") == 0 ? "indefinate" : expires, taskID);
            result = malloc(size + 1);
            snprintf(result, size + 1, "*%s.%s %d:%02d -> %s [%s] {%s} id:%s", day, month, hour, min, taskString, frequency, strcmp(expires, "NULL") == 0 ? "indefinate" : expires, taskID);
        }
    }
    free(buffer);
    return result;
}

void recurring_task_printer(char *result, char *arg, char option, char *searchDate)
{
    //printf("HERE\n");
    //printf("!%s\n", result);
    char *line = strtok(result, "\n");

    while(line != NULL)
    {
        char id[8];
        bool valid;
        //printf("%s\n", line);
        char *task = validate_recurring_task(line, arg, option, searchDate, &valid,  id);

        if (valid) (option != 'r') ? printf("%s\n", task) : recurring_pop_up(task);


        free(task);
        line = strtok(NULL, "\n");
    }
    free(line);
}

void recurring_task_show(char *arg, char option)
{
    char *sqlQuery = NULL;
    char **input = NULL;
    uint8_t *type = NULL;
    int n;

    //printf("HERE %s %c\n", arg, option);
    char *searchDate;   //setting here to avoid strtok error in validation of task
    if(option == 'o' || option == 'm' || option == 'w' || option == 'r')
    {
        int dateCheck = input_where(arg, &searchDate);
        if (dateCheck != 1)
        {
            if (dateCheck == -2) printf("ERROR - invaild date\n");
            else printf("ERROR - Invalid date string\n");

            free(searchDate);
            return;
        }
    }


    switch (option)
    {
    case 'o' :
        n = recurring_day_search(searchDate, &sqlQuery, &input, &type); //specific day search
        break;
    case '!':
        n = recurring_single_search_ID(arg, &sqlQuery, &input, &type); //searching for task with given unique ID
        break;
    case 'm':
        n = recurring_task_only_month(searchDate, &sqlQuery, &input, &type); //searching only monthly task for outlook function
        break;
    case 'w':
        n = recurring_task_only_week(searchDate, &sqlQuery, &input, &type); //searching only monthly task for outlook function
        break;
    case 'r':
        n = recurring_timed_task_check(searchDate, &sqlQuery, &input, &type); //checking for recurring appointments in the next 15mins
        break;
    case '.':
        sqlQuery = strdup("SELECT * FROM recurring_tasks ORDER BY date_begin DESC");
        n = 0;
        break;
    case 'l':
        sqlQuery = strdup("SELECT * FROM recurring_tasks ORDER BY ID DESC LIMIT 1");
        n = 0;
        break;
    default:
        printf("ERROR - could not find options for recurring_task_print\n");
        return;
    }

    if (n >= 0)
    {
        char *result = db_stmt_build_execute_string_return(n, input, type, sqlQuery, DB_PATH);

        //printf("%s\n", result);

        if(strlen(result) > 1) recurring_task_printer(result, arg, option, searchDate);
        if (n > 1)
        {
            for(int i = 0; i < n; i++)
            {
                free((input)[i]);
            }
        }
        free(result);
    }
    free(input);
    free(type);
    free(sqlQuery);

}

int frequency_input_helper(char *occurance)
{
    if (strcmp(occurance, "daily") == 0 || strcmp(occurance, "weekly") == 0 || strcmp(occurance, "monthly") == 0)
        return 1;
    else
        return -1;
}

int occurence_input_helper(char *frequency, char *firstOccurence)
{
    uint8_t day, month;
    uint16_t year;
    sscanf(firstOccurence, "%4hu-%2hhu-%2hhu", &year, &month, &day);

    if (strcmp(frequency, "daily") == 0)
        return 0;
    else if (strcmp(frequency, "weekly") == 0)
    {
        struct tm tm = {0};
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_isdst = -1;
        mktime(&tm);

        return tm.tm_wday;
    }
    else if (strcmp(frequency, "monthly") == 0)
        return day;
    else
        return -1;

}

void recurring_task_insert(char *taskArgs)
{
    // task | frequency | first occurence | [end occurence] | [time]
    char *field[5];
    int fieldNumber = 0;
    char *token = strtok(taskArgs, "|");
    while (token != NULL && fieldNumber < 5)
    {
        trim(token);
        field[fieldNumber++] = strdup(token);
        token = strtok(NULL, "|");
    }
    //for (int j = 0; j < fieldNumber; j++) printf("Field %d: %s\n", j, field[j]);
    //printf("fieldNumber: %d\n", fieldNumber);

    if (fieldNumber < 3)
    {
        printf("ERROR - Missing required fields for recurring task input\n");
        return;
    }

    char *taskString = malloc(strlen(field[0]) + 1);
    strcpy(taskString, field[0]);

    char *frequency = malloc(strlen(field[1]) + 1);
    strcpy(frequency, field[1]);

    int frequencyCheck = frequency_input_helper(frequency);
    if (frequencyCheck != 1)
    {
        printf("ERROR - Invalid frequency input\n");
        free(taskString);
        free(frequency);
        return;
    }

    char *firstOccurence;
    int dateCheck = input_where(field[2], &firstOccurence);
    if (dateCheck != 1)
    {
        if (dateCheck == -2) printf("ERROR - invaild date\n");
        else printf("ERROR - Invalid date string\n");

        free(taskString);
        free(frequency);
        free(firstOccurence);
        return;
    }
    int occurence = occurence_input_helper(frequency, firstOccurence); //0 for daily, 0-6 for weekly, 1-31 for monthly
    if (occurence == -1)
    {
        printf("ERROR - Invalid first occurence and frequency combination\n");
        free(taskString);
        free(frequency);
        free(firstOccurence);
        return;
    }
    char occurenceString[3];
    sprintf(occurenceString, "%d", occurence);


    char *endDateString;
    bool hasEndDate;
    if (fieldNumber > 3)
    {
        int endDateCheck = input_where(field[3], &endDateString);

        if (endDateCheck == 2) hasEndDate = false;
        else if (endDateCheck == 1) hasEndDate = true;
        else if (endDateCheck != 1)
        {
            if (endDateCheck == -2) printf("ERROR - invaild date\n");
            else printf("ERROR - Invalid date string\n");

            free(taskString);
            free(firstOccurence);
            free(frequency);
            free(endDateString);
            return;
        }
    }

    if (fieldNumber == 3) // no end date or time
    {
        char *sqlQuery = "INSERT INTO recurring_tasks(frequency, date_occurence, task, date_begin) "
                         "VALUES(?, ?, ?, ?)";

        const char *input[] = { frequency, occurenceString, taskString, firstOccurence };
        uint8_t type[] = { 1, 2, 1, 1 };
        db_stmt_build_execute(4, input, type, sqlQuery, DB_PATH);
        printf("%s\n", firstOccurence);
    }
    else if (fieldNumber == 4) // with end date
    {
        char *sqlQuery = "INSERT INTO recurring_tasks(frequency, date_occurence, task, date_begin, date_expires) "
                         "VALUES(?, ?, ?, ?, ?)";

        const char *input[] = { frequency, occurenceString, taskString, firstOccurence, endDateString };
        uint8_t type[] = { 1, 2, 1, 1, 1 };
        db_stmt_build_execute(5, input, type, sqlQuery, DB_PATH);

        free(endDateString);
    }
    else if (fieldNumber == 5)
    {
        char *timeString = malloc(strlen(field[4]) + 1);
        strcpy(timeString, field[4]);

        int timeCheck = time_input_helper(&timeString);
        if (timeCheck != 1)
        {
            printf("ERROR - Invalid time input\n");
            free(taskString);
            free(firstOccurence);
            free(frequency);
            free(endDateString);
            free(timeString);
            return;
        }

        if (hasEndDate) // task with time and endtime
        {
            char *sqlQuery = "INSERT INTO recurring_tasks(frequency, date_occurence, time, task, date_begin, date_expires) "
                             "VALUES(?, ?, strftime('%s', ?, ?) % 86400, ?, ?, ?)";

            const char *input[] = { frequency, occurenceString, firstOccurence, timeString, taskString, firstOccurence, endDateString };
            uint8_t type[] = { 1, 2, 1, 1, 1, 1, 1};
            db_stmt_build_execute(7, input, type, sqlQuery, DB_PATH);
        }
        else // entry for task with time but no endtime
        {
            char *sqlQuery = "INSERT INTO recurring_tasks(frequency, date_occurence, time, task, date_begin) "
                             "VALUES(?, ?, strftime('%s', ?, ?) % 86400, ?, ?)";

            const char *input[] = { frequency, occurenceString, firstOccurence, timeString, taskString, firstOccurence };
            uint8_t type[] = { 1, 2, 1, 1, 1, 1, };
            db_stmt_build_execute(6, input, type, sqlQuery, DB_PATH);
        }

        free(timeString);
    }

    free(taskString);
    free(firstOccurence);
    free(frequency);
    for (int j = 0; j < fieldNumber; j++) free(field[j]);

    recurring_task_show(NULL, 'l');
    printf("Recurring task added!\n");
}

void recurring_task_input_guide()
{
    bool correct = false;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    char taskString[1024] = {0};
    char frequency[16] = {0};
    char firstOccurence[11] = {0};
    char endOccurence[11] = {0};
    char timeString[6] = {0};

    printf("\nRecurring task input formatter!\n\n");
    printf("task: ");

    read = getline(&line, &len, stdin);
    if (read == -1)
    {
        printf("ERROR - reading input\n");
        free(line);
        return;
    }
    trim(line);
    strcpy(taskString, line);

    do
    {
        printf("\nfrequency (daily / weekly / monthly): ");

        read = getline(&line, &len, stdin);
        if (read == -1)
        {
            printf("ERROR - reading input\n");
            free(line);
            return;
        }
        trim(line);
        strcpy(frequency, line);

        if (frequency_input_helper(frequency) != 1)
            printf("ERROR - Invalid frequency input\n");
        else
            correct = true;
    }
    while (!correct);


    do
    {
        printf("Start date DD.MM.YYYY: ");
        read = getline(&line, &len, stdin);
        if (read == -1)
        {
            printf("ERROR - reading input\n");
            free(line);
            return;
        }
        trim(line);

        char *tmpCheck;
        int dateCheck = input_where(line, &tmpCheck);
        if (dateCheck != 1)
        {
            if (dateCheck == -2) printf("ERROR - invaild date\n");
            else printf("ERROR - Invalid date string\n");
            correct = false;
            free(tmpCheck);
        }
        else
        {
            strcpy(firstOccurence, line);
            free(tmpCheck);
            correct = true;
        }
    }
    while(!correct);

    do
    {
        printf("End date DD.MM.YYYY (or press enter for no end date): ");
        read = getline(&line, &len, stdin);
        if (read == -1)
        {
            printf("ERROR - reading input\n");
            free(line);
            return;
        }
        trim(line);

        if (strlen(line) == 0)
        {
            strcpy(endOccurence, "-");
            correct = true;
        }
        else
        {
            char *tmpCheck;
            int dateCheck = input_where(line, &tmpCheck);
            if (dateCheck != 1)
            {
                if (dateCheck == -2) printf("ERROR - invaild date\n");
                else printf("ERROR - Invalid date string\n");
                correct = false;
                free(tmpCheck);
            }
            else
            {
                strcpy(endOccurence, line);
                free(tmpCheck);
                correct = true;
            }
        }

    }
    while(!correct);

    do
    {
        printf("Time HH:MM (or press enter for no time): ");
        read = getline(&line, &len, stdin);
        if (read == -1)
        {
            printf("ERROR - reading input\n");
            free(line);
            return;
        }
        trim(line);

        if (strlen(line) == 0)
        {
            strcpy(timeString, "NULL");
            correct = true;
        }
        else
        {
            char *tempCheck;
            strcpy(tempCheck, line);
            int timeCheck = time_input_helper(&tempCheck);
            if (timeCheck != 1)
            {
                correct = false;
                printf("ERROR - Invalid time input\n");
            }
            else
            {
                strcpy(timeString, line);
                correct = true;
            }
        }
    }
    while(!correct);

    char *inputArg;
    int size;
    if (strcmp(timeString, "NULL") == 0 && strcmp(endOccurence, "-") == 0)
    {
        size = snprintf(NULL, 0, "%s | %s | %s", taskString, frequency, firstOccurence);
        inputArg = malloc(size + 1);
        snprintf(inputArg, size + 1, "%s | %s | %s", taskString, frequency, firstOccurence);
    }
    else if (strcmp(timeString, "NULL") == 0 && strcmp(endOccurence, "-") != 0)
    {
        size = snprintf(NULL, 0, "%s | %s | %s | %s", taskString, frequency, firstOccurence, endOccurence);
        inputArg = malloc(size + 1);
        snprintf(inputArg, size + 1, "%s | %s | %s | %s", taskString, frequency, firstOccurence, endOccurence);
    }
    else if (strcmp(timeString, "NULL") != 0 && strcmp(endOccurence, "-") == 0)
    {
        size = snprintf(NULL, 0, "%s | %s | %s | %s | %s", taskString, frequency, firstOccurence, endOccurence, timeString);
        inputArg = malloc(size + 1);
        snprintf(inputArg, size + 1, "%s | %s | %s | %s | %s", taskString, frequency, firstOccurence, endOccurence, timeString);
    }
    else
    {
        size = snprintf(NULL, 0, "%s | %s | %s | %s | %s", taskString, frequency, firstOccurence, endOccurence, timeString);
        inputArg = malloc(size + 1);
        snprintf(inputArg, size + 1, "%s | %s | %s | %s | %s", taskString, frequency, firstOccurence, endOccurence, timeString);
    }

    //printf("%s\n", inputArg);
    recurring_task_insert(inputArg);

    free(line);
    free(inputArg);
    printf("Recurring task added!\n");
}

void remove_recurring_task(char *id)
{
    printf("Are you sure you want to remove recurring task:\n");
    recurring_task_show(id, '!');

    printf("(y/n)?\n");
    char response = getchar();
    getchar();
    if (response != 'y' && response != 'Y')
    {
        printf("Recurring task removal cancelled.\n");
        return;
    }

    const char *sqlQuery = "DELETE FROM recurring_tasks WHERE id = ?";
    const char *input[] = { id };
    uint8_t type[] = { 2 };
    db_stmt_build_execute(1, input, type, sqlQuery, DB_PATH);
}


//#### Recurring task input helper, update and delete to be added ####

//************************************************************************************
//set up and outlook functions
//************************************************************************************

void task_outlook(char option)
{
    uint32_t range;
    char *outlook;
    bool tomorrow = false;

    switch (option)
    {
    case '!':
        range = 0;
        outlook = strdup("Today's");
        break;
    case 't':
        range = 1;
        outlook = strdup("Tomorrow's");
        tomorrow = true;
        break;
    case 'w':
        range = 7;
        outlook = strdup("your Weekly");
        break;
    case 'f':
        range = 14;
        outlook = strdup("your Fortnightly");
        break;
    case 'm':
        range = 31;
        outlook = strdup("your Monthly");
        break;
    case 'q':
        range = 92;
        outlook = strdup("your Quarterly");
        break;
    }

    printf("Welcome to Taskman - here is %s outlook!\n\n", outlook);

    while (range > 1)
    {
        char *dateString = date_calculator_from_range(range);
        print_week_and_day(dateString);

        task_show(dateString, 'o');
        recurring_task_show(dateString, 'o');

        range--;
        printf("\n");
    }

    if (range == 1)
    {
        char *dateString = date_calculator_from_range(range);
        printf("\n---- Tomorrow ----\n");
        task_show(dateString, 'o');
        recurring_task_show(dateString, 'o');
        if(!tomorrow) range--;
    }

    if (range == 0)
    {
        char *dateString = date_calculator_from_range(range);
        printf("\n---- Today ----\n");
        task_show(dateString, 'o');
        recurring_task_show(dateString, 'o');
    }

    printf("\n---- tasks ----\n");
    task_show(NULL, 't');




    free(outlook);
}


void set_config()
{
    printf("\nWelcome to taskman!\n");
    printf(
        "Lets create your database\n**NOTE** if this is not your first entry, "
        "your database is missing and it might overwrite your old database\n");
    printf("Are you sure you want to continue (y/n)?\n");
    char response = getchar();
    getchar();
    if (response != 'y' && response != 'Y')
    {
        printf("Database creation cancelled.\n");
        return;
    }

    // 1. Open database (creates file if it doesn't exist)
    SqliteDB *db = sqlitedb_open(DB_PATH);
    if(!db || db->last_rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", db ? db->err_msg : "Failed to create pointer for db - Check Memory Allocation");
    }


    sqlite3_stmt *stmt;
    const char *sql_create =
        "CREATE TABLE IF NOT EXISTS tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "date INTEGER, "
        "time INTEGER, "
        "task TEXT NOT NULL, "
        "date_added INTEGER NOT NULL, "
        "done INTEGER DEFAULT 0);"
        "CREATE TABLE IF NOT EXISTS recurring_tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "frequency TEXT NOT NULL, "
        "date_occurence INTEGER NOT NULL, " // for weekly sun - sat (0 - 6), monthly (1 - 31)
        "time INTEGER, "
        "task TEXT NOT NULL, "
        "date_begin TEXT NOT NULL, "
        "date_expires TEXT);";


    char *errmsg = NULL;
    int rc = sqlite3_exec(db->db, sql_create, 0, 0, &errmsg);
    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
    }
    sqlitedb_close(db);
}


void print_help()
{
    printf("\n---------------------------------------------------------------------------------------------\n"
           "                           Welcome to the help page for Taskman\n"
           "---------------------------------------------------------------------------------------------\n\n"
           "Input a task:\n"
           "by using our formatted task string:  \"task [| date [| time]\"\t\t*24hr time\n\n"
           " [] = optional -> when time is include taskman automatically makes it a appointment\n"
           "                  when both time and date are excluded, the task become a general task\n\n"
           "Example: taskman -n \"Lazer-tag with the fam | 13.10 | 19\"\n"
           "         taskman -n \"Delivery coming | tue\"\n"
           "         taskman -n \"Work on the new function that does the thing... ;)\"\n"
           "                                                     *see Advanced inputs for date string details\n"

           "\n\n"
           "Input a recurring task:\n"
           "reccuring task string format: \"task | frequency | first Occurence [| final Occurence [| time]\"\n\n"
           " [] = optional -> to enter a task with a time and no finish date input \"-\" in field\n"
           "there is also a recurring task input helper function that guides you through the steps \n"
           "                                                 \"-nr\" without any further arguments\n\n"
           "Example: taskman -nr \"Monthly Review | monthly | 15.1\"\n"
           "         taskman -nr \"Beer walk with the gang | weekly | thur | - | 16:30\"\n"
           "         taskman -nr \"Daily exercise for the week | daily | mon | fri | 6:30\"\n"
           "                                                     *see Advanced inputs for date string details\n"
           "\n\n"
           "Basic Task Commands:\n"
           "\t-n <task string>      add a new task\n"
           "\t-nr <task string>     add new recurring task\n"
           "\t-s <n>                view tasks in a range of n days\n"
           "\t-s! <date>            show tasks on given date\n"
           "\t-sr! <date>           show recurring tasks on given date\n"
           "\t-c <id>               marks the given id task as completed\n"
           "\n\n"
           "Outlook functions:\n"
           "<outlook Arg>   just running a specific outlook arg will give you the given forcase of your tasks\n\n"
           "outlook Args: today/tdy/t        for today\n"
           "              tomorrow/tom       for tomorrow\n"
           "              week/weekly/wk     for weekly\n"
           "              fortnightly/fort   for fortnightly\n"
           "              month/monthly/mth  for monthly\n"
           "              quarterly/quart    for quarterly\n"
           "\n\n"
           "Using reminders:\n"
           " -check function runs a check of both task and recurring tasks coming up 15mins.\n"
           " --delay <id> <amount> delays giving id for amount of minutes\n"
           "set up your favorite pop-up manager i.e xmessage w/ Crontab and never been late for a meeting again :)\n"
           " *i have commeted out my implementation of xmessage directly from the source code, if you'd prefer that\n"
           "\n\n"
           "Advanced inputs:\n"
           "all date fields have special checkers to assume dates or use day string instead\n"
           "i.e. inputing just \"22\" for example will assume current month and year. Same go for \"22.10\" assumes current year\n"
           "date string like; today/tomorow will also work, along with days of the week mon, tue... etc \n"
           "                                              (will set for the next occurence of given day)\n"
           "\n\n"
           "Enjoy your tasks,\n"
           "Taskman."
           "\n");
}
