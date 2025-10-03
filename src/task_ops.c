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



int range_search(char *arg, int currentDay, char **sqlQuery, char ***input, uint8_t **type)
{
    char temp[3];
    sprintf(temp, "%d", currentDay);
    char *dateString = input_where(temp);
    int range = atoi(arg);

    struct tm t = {0};
    sscanf(dateString, "%4d-%2d-%2d", &t.tm_year, &t.tm_mon, &t.tm_mday);
    t.tm_year -= 1900;  // struct tm years since 1900
    t.tm_mon -= 1;      // struct tm months 0-11
    // Convert to timestamp
    time_t timestamp = mktime(&t);
    t.tm_mday += 1 + range;                  // to get to whole day
    time_t timestamp1 = mktime(&t);  // to get the whole day

    *sqlQuery = strdup("SELECT * FROM tasks WHERE date >= ? AND date < ?");

    *input = malloc(2 * sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return 0;
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
        return 0;
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
    char *dateString = input_where(argString);

    struct tm t = {0};
    sscanf(dateString, "%4d-%2d-%2d", &t.tm_year, &t.tm_mon, &t.tm_mday);
    t.tm_year -= 1900;  // struct tm years since 1900
    t.tm_mon -= 1;      // struct tm months 0-11
    // Convert to timestamp

    time_t timestamp = mktime(&t);
    t.tm_mday += 1;                  // to get to whole day
    time_t timestamp1 = mktime(&t);  // to get the whole day


    *sqlQuery = strdup("SELECT * FROM tasks WHERE date >= ? AND date < ?");

    *input = malloc(2 * sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return 0;
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
        return 0;
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

    char *dateString = input_where(argString);

    struct tm t = {0};
    sscanf(dateString, "%4d-%2d-%2d", &t.tm_year, &t.tm_mon, &t.tm_mday);
    t.tm_year -= 1900;  // struct tm years since 1900
    t.tm_mon -= 1;      // struct tm months 0-11
    // Convert to timestamp
    time_t timestamp = mktime(&t);
    t.tm_mday += 1;                  // to get to whole day
    time_t timestamp1 = mktime(&t);  // to get the whole day


    *sqlQuery = strdup("SELECT * FROM tasks WHERE date >= ? AND date < ? AND time NOT NULL");

    *input = malloc(2 * sizeof(char *));
    if (*input == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return 0;
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
        return 0;
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
        return 0;
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
        return 0;
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
        return 0;
    }

    (*input)[0] = arg;

    *type = malloc(sizeof(uint8_t));
    if (*type == NULL)
    {
        printf("ERROR - Memory allocation failed");
        return 0;
    }

    (*type)[0] = 2;

    return 1;
}

void print_task(char *result)
{
    char *line = NULL;
    size_t len = 0;
    while (getline)

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
    case '.':
        n = 0;
        sqlQuery = strdup("SELECT * FROM tasks"); //prints entire table
        break;
    default:
        printf("ERROR - could not find options for task_show");
        break;
    }

    char *result = db_stmt_build_execute_string_return(n, input, type, sqlQuery);

    // printf("%s", result);
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

    option[0] == 'r' ? pop_up(result) : print_task(result);

    free(result);

}

void task_insert(char *dateAndTimeArg)
{
    set_time_date();
    char *token = strtok(dateAndTimeArg, "|");

    trim(token);
    char *taskString = malloc(strlen(token) + 1);
    strcpy(taskString, token);

    token = strtok(NULL, "|");
    if (token == NULL)      //quick path to add task for today

    {
        char *dateString = input_where("today");
        char *sqlQuery = "INSERT INTO tasks(date, task, date_added) VALUES(strftime('%s', "
                         "?, 'start of day'), ?, strftime('%s', 'now'))";

        const char *input[] = { dateString, taskString };
        uint8_t type[] = { 1, 1 };
        db_stmt_build_execute(2, input, type, sqlQuery);

        // task_show(id, '!');
        printf("ADDED\n");
        free(taskString);
        free(dateString);
        return;
    }
    trim(token);
    char *temp = malloc(strlen(token) + 1);
    strcpy(temp, token);

    token = strtok(NULL, "|");
    char *dateString = input_where(temp);

    if (token != NULL)
    {
        trim(token);
        char *timeString = malloc(strlen(token) + 1);
        strcpy(timeString, token);
        char *sqlQuery = "INSERT INTO tasks(date, time, task, date_added) "
                         "VALUES(strftime('%s', ?, 'start of day'), strftime('%s', "
                         "?, ?) % 86400, ?, strftime('%s', 'now'))";

        const char *input[] = { dateString, dateString, timeString, taskString };
        uint8_t type[] = { 1, 1, 1, 1 };
        db_stmt_build_execute(4, input, type, sqlQuery);
        free(timeString);
    }
    else
    {
        char *sqlQuery = "INSERT INTO tasks(date, task, date_added) VALUES(strftime('%s', "
                         "?, 'start of day'), ?, strftime('%s', 'now'))";

        const char *input[] = { dateString, taskString };
        uint8_t type[] = { 1, 1 };
        db_stmt_build_execute(2, input, type, sqlQuery);
    }
    // task_show(id, '!');
    printf("ADDED\n");
    free(temp);
    free(dateString);
    free(taskString);
}

void task_complete(char *id)
{
    char *sqlQuery = "UPDATE tasks SET done = 1 WHERE id = ?";
    const char *input[] = { id };
    uint8_t type[] = { 2 };

    db_stmt_build_execute(1, input, type, sqlQuery);

    // task_show(id, '!');
}


void task_postpone(char *id, char *dateAndTimeArg)
{
    set_time_date();
    char *token = strtok(dateAndTimeArg, "|");
    trim(token);

    char *tmp = malloc(strlen(token) + 1);
    strcpy(tmp, token);

    token = strtok(NULL, "|"); //calling strtok before input_where to not modify *tmp
    char *dateString = input_where(tmp);

    if (token != NULL)
    {
        trim(token);
        char *timeString = malloc(strlen(token) + 1);
        strcpy(timeString, token);

        char *sqlQuery = "UPDATE tasks SET date = strftime('%s', ?, 'start of day'), "
                         "time = strftime('%s', ?, ?) % 86400 WHERE id = ?";

        const char *input[] = { dateString, dateString, timeString, id };
        uint8_t type[] = { 1, 1, 1, 2 };
        db_stmt_build_execute(4, input, type, sqlQuery);

        free(timeString);
    }
    else
    {
        char *sqlQuery = "UPDATE tasks SET date = strftime('%s', ?, 'start of day') WHERE id = ?";

        const char *input[] = { dateString, id };
        uint8_t type[] = { 1, 2 };
        db_stmt_build_execute(2, input, type, sqlQuery);
    }

    // task_show(id, '!');
    free(dateString);
    free(tmp);
}

void remove_task(char *id)
{
    printf("Are you sure you want to remove task:");
    // task_show(id, '!');
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
    db_stmt_build_execute(1, input, type, sqlQuery);
}


void set_config()
{
    printf("\nWelcome to taskman!\n");
    printf(
        "Lets create your database\n**NOTE** if this is not your first timeString, "
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
        "date INTEGER NOT NULL, "
        "time INTEGER,"
        "task TEXT NOT NULL,"
        "date_added INTEGER NOT NULL,"
        "done INTEGER DEFAULT 0);";


    int rc = sqlite3_prepare_v2(db->db, sql_create, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", db->err_msg);
        sqlitedb_close(db);
    }
    else
    {
        rc = sqlitedb_execute_stmt(db, stmt);
        if (rc != SQLITE_OK)
        {
            printf("ERROR: %s\n", db->err_msg);
        }
        sqlite3_finalize(stmt);

        sqlitedb_close(db);
    }

}
