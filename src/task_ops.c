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
                //field 5 is date added - currently not used
                completion = (buffer[i+1] == '0' ) ? false : true; //field 6

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


void pop_up(char *result)
{
    char *buffer = strdup(result);

    char *line = strtok(buffer, "\n");

    while (line != NULL)
    {
        char id[8];
        char *task = process_line(line, id);
        size_t len = strlen(task);
        size_t cmdSize = len + 60;
        char *cmd = malloc(cmdSize);
        snprintf(cmd, cmdSize, "xmessage -buttons 'Okay:0,Delay:1' 'Termin in 15 minutes.\n%s'", task);
        printf("%s", cmd);
        if (cmd != NULL)
        {
            int message = system(cmd);
            {
                if (message == 1)
                {
                    // need to build system("alacritty --command taskman -delay");
                }

                else
                {
                    task_complete(id);
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

void task_show(char *arg, char option)
{
//return for n how many inputs for freeing in task_show
    uint32_t n;
    time_t t;
    struct tm tm;
    set_time_date();
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
        sqlQuery = strdup("SELECT * FROM tasks WHERE date IS NULL");
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
        char *result = db_stmt_build_execute_string_return(n, input, type, sqlQuery, "tasks.db");

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
    set_time_date();
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
        db_stmt_build_execute(1, input, type, sqlQuery, "tasks.db");

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
                db_stmt_build_execute(4, input, type, sqlQuery, "tasks.db");
            }

        }
        else
        {
            char *sqlQuery = "INSERT INTO tasks(date, task, date_added) VALUES(strftime('%s', "
                             "?, 'start of day'), ?, strftime('%s', 'now'))";

            const char *input[] = { dateString, taskString };
            uint8_t type[] = { 1, 1 };
            db_stmt_build_execute(2, input, type, sqlQuery, "tasks.db");
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

    db_stmt_build_execute(1, input, type, sqlQuery, "tasks.db");

    task_show(id, '!');
}


void task_postpone(char *id, char *dateAndTimeArg)
{
    bool updated;
    set_time_date();
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
                db_stmt_build_execute(4, input, type, sqlQuery, "tasks.db");

                free(timeString);
            }
        }
        else
        {
            char *sqlQuery = "UPDATE tasks SET date = strftime('%s', ?, 'start of day') WHERE id = ?";

            const char *input[] = { dateString, id };
            uint8_t type[] = { 1, 2 };
            db_stmt_build_execute(2, input, type, sqlQuery, "tasks.db");
            updated = true;
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
    db_stmt_build_execute(1, input, type, sqlQuery, "tasks.db");
}

//************************************************************************************
//recurring tasks
//************************************************************************************

char *validate_recurring_task(char *line, char option, bool *valid, char *id)
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
    *valid = true;

    int size = snprintf(NULL, 0, "id:%s -> %s [%s] first occurence: %s, frequency: %s, expires: %s\n", taskID, taskString, strcmp(timeString, "NULL") == 0 ? "task" : timeString, firstOccurence, frequency, strcmp(expires, "NULL") == 0 ? "never" : expires);
    char *result = malloc(size + 1);
    snprintf(result, size + 1, "id:%s -> %s [%s] first occurence: %s, frequency: %s, expires: %s\n", taskID, taskString, strcmp(timeString, "NULL") == 0 ? "task" : timeString, firstOccurence, frequency, strcmp(expires, "NULL") == 0 ? "never" : expires);
    free(buffer);
    return result;
}

void recurring_task_printer(char *result, char option)
{
    char *line = strtok(result, "\n");

    while(line != NULL)
    {
        char id[8];
        bool valid = false;
        char *task = validate_recurring_task(line, option, &valid, id);

        if(valid) printf("%s\n", task);

        free(task);
        line = strtok(NULL, "\n");
    }
}


void recurrring_task_show(char *arg, char option)
{
    char *sqlQuery = NULL;
    char **input = NULL;
    uint8_t *type = NULL;
    uint32_t n;
    uint32_t currentDateStamp;
    uint32_t currentTimeStamp;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    currentTimeStamp = (tm.tm_hour * 3600) + (tm.tm_min * 60) + tm.tm_sec;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    time_t now = mktime(&tm);
    currentDateStamp = now;




    switch (option)
    {
    case '.':
        sqlQuery = strdup("SELECT * FROM recurring_tasks ORDER BY date_begin");
        n = 0;
        break;
    case 'l':
        sqlQuery = strdup("SELECT * FROM recurring_tasks ORDER BY ID DESC LIMIT 1");
        n = 0;
        break;
    default:
        printf("ERROR - could not find options for recurring_task_print");
        return;
    }

    char *result = db_stmt_build_execute_string_return(n, input, type, sqlQuery, "tasks.db");

    printf("%s", result);

    recurring_task_printer(result, option);

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

void recurrring_task_insert(char *taskArgs)
{
    set_time_date();

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
    for (int j = 0; j < fieldNumber; j++) printf("Field %d: %s\n", j, field[j]);
    printf("fieldNumber: %d\n", fieldNumber);

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
                         "VALUES(?, ?, ?, strftime('%s', ?, 'start of day', 'localtime'))";

        const char *input[] = { frequency, occurenceString, taskString, firstOccurence };
        uint8_t type[] = { 1, 2, 1, 1 };
        db_stmt_build_execute(4, input, type, sqlQuery, "tasks.db");
        printf("%s\n", firstOccurence);
    }
    else if (fieldNumber == 4) // with end date
    {
        char *sqlQuery = "INSERT INTO recurring_tasks(frequency, date_occurence, task, date_begin, date_expires) "
                         "VALUES(?, ?, ?, strftime('%s', ?, 'start of day'), strftime('%s', ?, 'start of day'))";

        const char *input[] = { frequency, occurenceString, taskString, firstOccurence, endDateString };
        uint8_t type[] = { 1, 2, 1, 1, 1 };
        db_stmt_build_execute(5, input, type, sqlQuery, "tasks.db");

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
                             "VALUES(?, ?, strftime('%s', ?, ?) % 86400, ?, strftime('%s', ?, 'start of day'), strftime('%s', ?, 'start of day'))";

            const char *input[] = { frequency, occurenceString, firstOccurence, timeString, taskString, firstOccurence, endDateString };
            uint8_t type[] = { 1, 2, 1, 1, 1, 1, 1};
            db_stmt_build_execute(7, input, type, sqlQuery, "tasks.db");
        }
        else // entry for task with time but no endtime
        {
            char *sqlQuery = "INSERT INTO recurring_tasks(frequency, date_occurence, time, task, date_begin) "
                             "VALUES(?, ?, strftime('%s', ?, ?) % 86400, ?, strftime('%s', ?, 'start of day'))";

            const char *input[] = { frequency, occurenceString, firstOccurence, timeString, taskString, firstOccurence };
            uint8_t type[] = { 1, 2, 1, 1, 1, 1, };
            db_stmt_build_execute(6, input, type, sqlQuery, "tasks.db");
        }

        free(timeString);
    }



    recurrring_task_show(NULL, 'l');

    free(taskString);
    free(firstOccurence);
    free(frequency);
}

//#### Recurring task input helper, update and delete to be added ####

//************************************************************************************
//set up and outlook functions
//************************************************************************************

void task_outlook(char option)
{
    uint32_t range;
    char *outlook;



    switch (option)
    {
    case '!':
        range = 0;
        outlook = strdup("Today's");
        break;
    case 't':
        range = 1;
        outlook = strdup("Tomorrow's");
        break;
    case 'w':
        range = 7;
        outlook = strdup("Weekly");
        break;
    case 'f':
        range = 14;
        outlook = strdup("Fortnightly");
        break;
    case 'm':
        range = 31;
        outlook = strdup("Monthly");
        break;
    case 'q':
        range = 92;
        outlook = strdup("Quarterly");
        break;
    }

    printf("Welcome to Taskman - here is your %s outlook!\n", outlook);

    while (range > 1)
    {


        range--;
    }




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
    SqliteDB *db = sqlitedb_open("tasks.db");
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
        "date_begin INTEGER NOT NULL, "
        "date_expires INTEGER);";


    char *errmsg = NULL;
    int rc = sqlite3_exec(db->db, sql_create, 0, 0, &errmsg);
    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
    }
    sqlitedb_close(db);
}


