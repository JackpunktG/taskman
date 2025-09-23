#include <ctype.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

uint8_t range;
time_t t;
struct tm tm;
int currentDay;
int currentMonth;
int currentYear;
int currentHour;
int currentMin;

void trim(char *str);
void set_time_date();
void task_complete(char *id);

void set_config()
{
    printf("\nWelcome to taskman!\n");
    printf(
        "Lets create your database\n**NOTE** if this is not your first time, "
        "your database is missing and it might overwrite your old database\n");
    printf("Are you sure you want to continue (y/n)?\n");
    char response = getchar();
    getchar();
    if (response != 'y' && response != 'Y')
    {
        printf("Database creation cancelled.\n");
        return;
    }
    sqlite3 *db;
    char *err_msg = 0;

    // 1. Open database (creates file if it doesn't exist)
    int rc = sqlite3_open("task.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("ERROR - Cannot create database: %s\n", sqlite3_errmsg(db));
        return;
    }

    // 2. Create table
    const char *sql_create =
        "CREATE TABLE IF NOT EXISTS tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "date INTEGER NOT NULL, "
        "time INTEGER,"
        "task TEXT NOT NULL,"
        "date_added INTEGER NOT NULL,"
        "done INTEGER DEFAULT 0);";
    rc = sqlite3_exec(db, sql_create, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    sqlite3_close(db);
}

char *find_date_from_day(char *day)
{
    // STILL NEEDED
    return NULL;
}

char *input_where(char *when)
{
    char *result = malloc(sizeof(char) * 12);
    if (!isdigit(when[0]))
    {
        if (strcmp(when, "mon") == 0 || strcmp(when, "monday") == 0)
            return find_date_from_day("mon");
        else if (strcmp(when, "tue") == 0 || strcmp(when, "tuesday") == 0)
            return find_date_from_day("tue");
        else if (strcmp(when, "wed") == 0 || strcmp(when, "wednesday") == 0)
            return find_date_from_day("wed");
        else if (strcmp(when, "thu") == 0 || strcmp(when, "thursday") == 0)
            return find_date_from_day("thu");
        else if (strcmp(when, "fri") == 0 || strcmp(when, "friday") == 0)
            return find_date_from_day("fri");
        else if (strcmp(when, "sat") == 0 || strcmp(when, "saturday") == 0)
            return find_date_from_day("sat");
        else if (strcmp(when, "sun") == 0 || strcmp(when, "sunday") == 0)
            return find_date_from_day("sun");
        else if (strcmp(when, "tomorrow") == 0 || strcmp(when, "tom") == 0)
            return find_date_from_day("tom");
        else
        {
            printf("ERROR when parsing due day with days\n");
            return 0;
        }
    }
    else
    {
        char *token = strtok(when, ".");
        if (!token)
        {
            printf("ERROR when parsing due day with date");
        }
        trim(token);
        int day = atoi(token);
        int month;
        int year;

        token = strtok(NULL, ".");
        if (token)
        {
            trim(token);
            month = atoi(token);
        }
        else
        {
            snprintf(result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     day);
            return result;
        }

        token = strtok(NULL, ".");
        if (token)
        {
            trim(token);
            year = atoi(token);
            snprintf(result, 12, "%04d-%02d-%02d", year, month, day);
            return result;
        }
        else
        {
            snprintf(result, 12, "%04d-%02d-%02d", currentYear, month, day);
            return result;
        }
    }
}

void task_insert(char *input)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open("task.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("ERROR - Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    set_time_date();
    char *token = strtok(input, "|");
    trim(token);
    char *task = malloc(strlen(token) + 1);
    strcpy(task, token);

    token = strtok(NULL, "|");
    trim(token);
    char *whenSt = malloc(strlen(token) + 1);
    strcpy(whenSt, token);

    token = strtok(NULL, "|");
    char *when = input_where(whenSt);

    if (token != NULL)
    {
        trim(token);
        char *time = malloc(strlen(token) + 1);
        strcpy(time, token);
        char sqlInsert[2048];
        snprintf(
            sqlInsert, sizeof(sqlInsert),
            "INSERT INTO tasks(date, time, task, date_added) "
            "VALUES(strftime('%%s', '%s', 'start of day'), strftime('%%s', "
            "'%s', '%s:00') %% 86400, '%s', strftime('%%s', 'now'))",
            when, when, time, task);
        rc = sqlite3_exec(db, sqlInsert, 0, 0, &err_msg);
        if (rc != SQLITE_OK)
        {
            printf("SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
        }
        else
            printf("Task: %s\nDate: %s %s\tADDED\n", task, when, time);
        free(time);
    }
    else
    {
        char sqlInsert[2048];
        snprintf(
            sqlInsert, sizeof(sqlInsert),
            "INSERT INTO tasks(date, task, date_added) VALUES(strftime('%%s', "
            "'%s', 'start of day'), '%s', strftime('%%s', 'now'))",
            when, task);
        rc = sqlite3_exec(db, sqlInsert, 0, 0, &err_msg);
        if (rc != SQLITE_OK)
        {
            printf("SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
        }
        else
            printf("Task: %s\nDate: %s\tADDED\n", task, when);
    }
    free(whenSt);
    free(when);
    free(task);
    sqlite3_close(db);
}

void task_show(char *arg, char option)
{
    sqlite3 *db;
    int rc = sqlite3_open("task.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("ERROR - Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    char sqlSelect[1024];

    switch (option)
    {
    case 's': //getting task within a range of days
    {
        set_time_date();
        char temp[3];
        sprintf(temp, "%d", currentDay);
        char *when = input_where(temp);
        int range = atoi(arg);

        struct tm t = {0};
        sscanf(when, "%4d-%2d-%2d", &t.tm_year, &t.tm_mon, &t.tm_mday);
        t.tm_year -= 1900;  // struct tm years since 1900
        t.tm_mon -= 1;      // struct tm months 0-11
        // Convert to timestamp
        time_t timestamp = mktime(&t);
        t.tm_mday += 1 + range;                  // to get to whole day
        time_t timestamp1 = mktime(&t);  // to get the whole day

        snprintf(sqlSelect, sizeof(sqlSelect),
                 "SELECT * FROM tasks WHERE date >= %ld AND date < %ld",
                 (long)timestamp, (long)timestamp1);

        free(when);
        break;
    }
    case 'o': //show only tasks for the given day -s!
    {
        set_time_date();
        char *token = strtok(arg, "|");
        trim(token);

        char *whenSt = malloc(strlen(token) + 1);
        strcpy(whenSt, token);

        token = strtok(NULL, "|");
        char *when = input_where(whenSt);

        struct tm t = {0};
        sscanf(when, "%4d-%2d-%2d", &t.tm_year, &t.tm_mon, &t.tm_mday);
        t.tm_year -= 1900;  // struct tm years since 1900
        t.tm_mon -= 1;      // struct tm months 0-11
        // Convert to timestamp

        time_t timestamp = mktime(&t);
        t.tm_mday += 1;                  // to get to whole day
        time_t timestamp1 = mktime(&t);  // to get the whole day

        snprintf(sqlSelect, sizeof(sqlSelect),
                 "SELECT * FROM tasks WHERE date >= %ld AND date < %ld",
                 (long)timestamp, (long)timestamp1);

        free(whenSt);
        free(when);
        break;
    }
    case 'a': //getting only tasks with time
    {
        set_time_date();
        char *token = strtok(arg, "|");
        trim(token);

        char *whenSt = malloc(strlen(token) + 1);
        strcpy(whenSt, token);

        char *when = input_where(whenSt);

        struct tm t = {0};
        sscanf(when, "%4d-%2d-%2d", &t.tm_year, &t.tm_mon, &t.tm_mday);
        t.tm_year -= 1900;  // struct tm years since 1900
        t.tm_mon -= 1;      // struct tm months 0-11
        // Convert to timestamp
        time_t timestamp = mktime(&t);
        t.tm_mday += 1;                  // to get to whole day
        time_t timestamp1 = mktime(&t);  // to get the whole day

        snprintf(sqlSelect, sizeof(sqlSelect),
                 "SELECT * FROM tasks WHERE date >= %ld AND date < %ld AND "
                 "time NOT NULL",
                 (long)timestamp, (long)timestamp1);

        free(whenSt);
        free(when);
        break;
    }
    case 'r': //reminder for tasks in next 15 minutes with xmessage popup
    {
        set_time_date();
        struct tm t = {0};
        t.tm_mon = currentMonth - 1;
        t.tm_mday = currentDay;
        t.tm_year = currentYear - 1900;

        time_t timestamp = mktime(&t);
        t.tm_mday += 1;                  // to get to whole day
        time_t timestamp1 = mktime(&t);  // to get the whole day

        // Get current time in seconds since midnight
        time_t now = time(NULL);
        struct tm *now_tm = localtime(&now);
        int seconds_now =
            now_tm->tm_hour * 3600 + now_tm->tm_min * 60 + now_tm->tm_sec;
        int seconds_next = seconds_now + 15 * 60;

        snprintf(sqlSelect, sizeof(sqlSelect),
                 "SELECT * FROM tasks WHERE date >= %ld AND date < %ld AND "
                 "time >= %d AND time < %d AND done = 0",
                 (long)timestamp, (long)timestamp1, seconds_now, seconds_next);
        break;
    }
    case '!': //show only task with given id
    {
        snprintf(sqlSelect, sizeof(sqlSelect),
                 "SELECT * FROM tasks WHERE id = %s", arg);

        break;
    }
    case '.': //show all tasks
    {
        snprintf(sqlSelect, sizeof(sqlSelect), "SELECT * FROM tasks");
        break;
    }
    }
    sqlite3_stmt *res;
    rc = sqlite3_prepare_v2(db, sqlSelect, -1, &res, 0);
    if (rc == SQLITE_OK)
    {
        while (sqlite3_step(res) == SQLITE_ROW)
        {
            int id = sqlite3_column_int(res, 0);
            int date = sqlite3_column_int(res, 1);
            int time = sqlite3_column_int(res, 2);
            const char *task = (const char *)sqlite3_column_text(res, 3);
            int done = sqlite3_column_int(res, 5);

            time_t timestamp = (time_t)date;
            struct tm *tm_info = localtime(&timestamp);
            char date_str[32];

            strftime(date_str, sizeof(date_str), "%d.%m.%Y", tm_info);

            if (time != 0)
            {
                int h = time / 3600;
                int m = (time % 3600) / 60;
                char time_str[16];
                snprintf(time_str, sizeof(time_str), "%02d:%02d", h, m);

                if (option == 'r')
                {
                    char cmd[1024];
                    snprintf(cmd, sizeof(cmd),
                             "xmessage -buttons 'Okay:0,Delay:1' 'Termin in 15 minutes. %s %s'",
                             task, time_str);
                    int message = system(cmd);
                    if (message == 1)
                    {
                        system("alacritty --command taskman -delay");

                    }
                    else
                    {
                        char idSt[12];
                        snprintf(idSt, sizeof(idSt), "%d", id);
                        task_complete(idSt);
                    }
                }
                else
                {
                    printf("%s %s -> %s [%s] id:%d\n", date_str, time_str, task,
                           done ? "COMPLETED" : "WORKING", id);
                }
            }
            else
                printf("%s -> %s [%s] id:%d\n", date_str, task,
                       done ? "COMPLETED" : "WORKING", id);
        }
        sqlite3_finalize(res);

        sqlite3_close(db);
    }
}

void task_postpone(char *id, char *whenSt)
{
    set_time_date();
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open("task.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("ERROR - Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }
    char *token = strtok(whenSt, "|");
    trim(token);

    char *temp = malloc(strlen(token) + 1);
    strcpy(temp, token);

    token = strtok(NULL, "|");
    char *whenDate = input_where(temp);
    if (token != NULL)
    {
        trim(token);
        char *time = malloc(strlen(token) + 1);
        strcpy(time, token);
        char sqlInsert[2048];
        snprintf(
            sqlInsert, sizeof(sqlInsert),
            "UPDATE tasks SET date = strftime('%%s', '%s', 'start of day'), "
            "time = strftime('%%s', '%s', '%s:00') %% 86400 WHERE id = %s",
            whenDate, whenDate, time, id);
        rc = sqlite3_exec(db, sqlInsert, 0, 0, &err_msg);
        if (rc != SQLITE_OK)
        {
            printf("SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
        }
        else
        {
            printf("Task postponed:\n");
            task_show(id, '!');
        }
        free(time);
    }
    else
    {
        char sqlUpdate[256];
        snprintf(sqlUpdate, sizeof(sqlUpdate),
                 "UPDATE tasks SET date = strftime('%%s', '%s', 'start of day') WHERE id = %s", whenDate, id);
        rc = sqlite3_exec(db, sqlUpdate, 0, 0, &err_msg);
        if (rc != SQLITE_OK)
        {
            printf("SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
        }
        else
        {
            printf("Task postponed:\n");
            task_show(id, '!');
        }
    }
    sqlite3_close(db);
    free(whenDate);
    free(temp);
}

void remove_task(char *id)
{
    printf("Are you sure you want to remove task:");
    task_show(id, '!');
    printf("(y/n)?\n");
    char response = getchar();
    getchar();
    if (response != 'y' && response != 'Y')
    {
        printf("Task removal cancelled.\n");
        return;
    }

    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open("task.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("ERROR - Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    char sqlDelete[256];
    snprintf(sqlDelete, sizeof(sqlDelete),
             "DELETE FROM tasks WHERE id = %s", id);
    rc = sqlite3_exec(db, sqlDelete, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    else
        printf("Task id:%s REMOVED\n", id);

    sqlite3_close(db);
}

void task_complete(char *id)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open("task.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("ERROR - Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    char sqlUpdate[256];
    snprintf(sqlUpdate, sizeof(sqlUpdate),
             "UPDATE tasks SET done = 1 WHERE id = %s", id);
    rc = sqlite3_exec(db, sqlUpdate, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    else
    {
        printf("Task id:%s COMPLETED\n", id);
        task_show(id, '!');
    }
    sqlite3_close(db);
}

void direct_sql(char *sql)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open("task.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("ERROR - Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    else
        printf("SQL executed successfully\n");

    sqlite3_close(db);
}

void trim(char *str)
{
    char *start = str;
    char *end;

    // Trim leading spaces
    while (*start && isspace((unsigned char)*start))
    {
        start++;
    }

    // All spaces?
    if (*start == 0)
    {
        str[0] = '\0';
        return;
    }

    // Trim trailing spaces
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end))
    {
        end--;
    }

    // Write new null terminator
    *(end + 1) = '\0';

    // Shift trimmed string to the beginning
    memmove(str, start,
            end - start + 2);  // +1 for '\0', +1 for end-start indexing
}

bool isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int day, int month, int year)
{
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2)
        return isLeapYear(tm.tm_year + 1900) ? 29 : 28;
    return days[month - 1];
}

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
    else if(strcmp(args[1], "today") ==0)
    {
        task_show("0", 's');
    }
    else if(strcmp(args[1], "tomorrow") ==0 || strcmp(args[1], "tom") ==0)
    {
        task_show("1", 's');
    }
    else if(strcmp(args[1], "week") ==0)
    {
        task_show("7", 's');
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
        task_show(args[2], '.');
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
        direct_sql(args[2]);
    }
    else
        printf("INVALID args\n");
}

void set_time_date()
{
    t = time(NULL);
    tm = *localtime(&t);
    currentDay = tm.tm_mday;
    currentMonth = tm.tm_mon + 1;
    currentYear = tm.tm_year + 1900;
    currentHour = tm.tm_hour;
    currentMin = tm.tm_min;
}

int main(int argc, char *args[])
{
    if (access("task.db", F_OK) != 0)
    {
        printf("Cannot find database\nIf this is your first time, no worries. we'll create one :)\n");
        set_config();
    }

    if (args[0] != NULL)
        run_args(args);

    return 0;
}

