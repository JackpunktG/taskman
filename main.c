#include <sqlite3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>

uint8_t range;
time_t t;
struct tm tm;
int currentDay;
int currentMonth;
int currentYear;

void trim(char *str);
void set_time_date();

bool is_config()
{
    FILE *file = fopen(".config", "r");
    if (file == NULL)
    {
        return false;
    }

    size_t len = 0;
    char *line = NULL;
    int countline = 0;

    while (getline(&line, &len, file) != -1)
    {
        trim(line);
        if (line[0] == '/' && line[1] == '/')
            continue;

        countline++;

        switch (countline)
        {
        case 1:
            range = atoi(line);
            break;
        }
    }
    free(line);
    fclose(file);
    return true;
}

void set_config()
{
    bool input = false;
    while (!input)
    {
        printf("Enter your chosen range for outlook forecase: ");
        scanf("%hhu", &range);
        if (range > 0)
        {
            FILE *file = fopen(".config", "w");
            if (file == NULL)
            {
                printf("ERROR opening config file");
                return;
            }
            fprintf(file, "//Range\n%hhu\n", range);
            fclose(file);
            printf("Config file written -> .config\n");
            input = true;
        }
        else
            printf("INVALID Range input");
    }

    sqlite3 *db;
    char *err_msg = 0;

    // 1. Open database (creates file if it doesn't exist)
    int rc = sqlite3_open("tasks.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("ERROR - Cannot create database: %s\n", sqlite3_errmsg(db));
        return;
    }

    // 2. Create table
    const char *sql_create = "CREATE TABLE IF NOT EXISTS tasks ("
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
            snprintf(result, 12, "%04d-%02d-%02d", currentYear, currentMonth, day);
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
    int rc = sqlite3_open("tasks.db", &db);
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
        printf("%s -3\n", token);
        ;
        char sqlInsert[2048];
        snprintf(sqlInsert, sizeof(sqlInsert), "INSERT INTO tasks(date, time, task, date_added) VALUES(strftime('%%s', '%s', 'start of day'), strftime('%%s', '%s', '%s:00') %% 86400, '%s', strftime('%%s', 'now'))", when, when, time, task);
        rc = sqlite3_exec(db, sqlInsert, 0, 0, &err_msg);
        if (rc != SQLITE_OK)
        {
            printf("SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
        }
        free(time);
    }
    else
    {
        printf("%s - date\n%s - task\n", when, task);
        char sqlInsert[2048];
        snprintf(sqlInsert, sizeof(sqlInsert), "INSERT INTO tasks(date, task, date_added) VALUES(strftime('%%s', '%s', 'start of day'), '%s', strftime('%%s', 'now'))", when, task);
        rc = sqlite3_exec(db, sqlInsert, 0, 0, &err_msg);
        if (rc != SQLITE_OK)
        {
            printf("SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
        }
    }
    free(whenSt);
    free(when);
    free(task);
    sqlite3_close(db);
}

void task_show(char *arg)
{
    sqlite3 *db;
    int rc = sqlite3_open("tasks.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("ERROR - Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    char sqlSelect[1024];

    snprintf(sqlSelect, sizeof(sqlSelect), "SELECT * FROM tasks");

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

                printf("%s %s -> %s [%s] id:%d\n", date_str, time_str, task, done ? "COMPLETED" : "WORKING", id);
            }
            else
                printf("%s -> %s [%s] id:%d\n", date_str, task, done ? "COMPLETED" : "WORKING", id);
        }
    }
    sqlite3_finalize(res);

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
            end - start + 2); // +1 for '\0', +1 for end-start indexing
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
        task_show(args[2]);
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
}

int main(int argc, char *args[])
{

    if (!is_config())
    {
        set_config();
    }

    if (args[0] != NULL)
        run_args(args);

    /*
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open("tasks.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("ERROR - Cannot open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // 3. Insert data
    const char *sql_insert = "INSERT INTO tasks(description, done) VALUES('Buy milk', 0);";
    rc = sqlite3_exec(db, sql_insert, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    // 4. Read data
    const char *sql_select = "SELECT id, description, done FROM tasks;";
    sqlite3_stmt *res;
    rc = sqlite3_prepare_v2(db, sql_select, -1, &res, 0);
    if (rc == SQLITE_OK)
    {
        while (sqlite3_step(res) == SQLITE_ROW)
        {
            int id = sqlite3_column_int(res, 0);
            const unsigned char *desc = sqlite3_column_text(res, 1);
            int done = sqlite3_column_int(res, 2);
            printf("Task %d: %s [%s]\n", id, desc, done ? "Done" : "Not done");
        }
    }
    sqlite3_finalize(res);

    sqlite3_close(db);
    */
    return 0;
}