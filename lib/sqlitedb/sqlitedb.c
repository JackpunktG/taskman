#include "sqlitedb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

SqliteDB *sqlitedb_open(const char *filename)
{
    SqliteDB *db_wrapper = malloc(sizeof(SqliteDB));
    if (!db_wrapper)
    {
        printf("ERROR - Memory allocation failed\n");
        return NULL;
    }

    db_wrapper->err_msg = NULL;
    db_wrapper->last_rc = sqlite3_open(filename, &db_wrapper->db);

    if (db_wrapper->last_rc != SQLITE_OK)
    {
        printf("ERROR - Cannot open database: %s\n", sqlite3_errmsg(db_wrapper->db));
        sqlite3_close(db_wrapper->db);
        free(db_wrapper);
        return NULL;
    }

    return db_wrapper;
}


void sqlitedb_close(SqliteDB *db_wrapper)
{
    if (db_wrapper)
    {
        if (db_wrapper->err_msg) //(NOT NULL)
        {
            sqlite3_free(db_wrapper->err_msg);
        }
        if (db_wrapper->db)
        {
            sqlite3_close(db_wrapper->db);
        }
        free(db_wrapper);
    }
}

int sqlitedb_execute_stmt(SqliteDB *db_wrapper, sqlite3_stmt *stmt)
{
    int rc = sqlite3_step(stmt);
    db_wrapper->last_rc = rc;
    sqlite3_reset(stmt);
    return rc == SQLITE_DONE ? SQLITE_OK : rc;

}


const char *sqlitedb_error(SqliteDB *db_wrapper)
{
    if (db_wrapper->err_msg)
    {
        return db_wrapper->err_msg;
    }
    return sqlite3_errmsg(db_wrapper->db);
}


typedef struct
{
    char *result;
    size_t size;
    size_t capacity;
} QueryResult;


char *sqlitedb_excute_stmt_result_as_string(SqliteDB *db_wrapper, sqlite3_stmt *stmt)
{
    QueryResult buf = { .result = malloc(1024), .size = 0, .capacity = 1024 };
    buf.result[0] = '\0';
    int cols = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        for (int i = 0; i < cols; i++)
        {
            const char *val = (const char *)sqlite3_column_text(stmt, i);
            if (!val) val = "NULL";
            size_t len = strlen(val);
            if (buf.capacity < buf.size + len + 2)
            {
                buf.capacity += 1024;
                buf.result = realloc(buf.result, buf.capacity);
            }
            memcpy(buf.result + buf.size, val, len);
            buf.size += len;
            buf.result[buf.size++] = (i < cols - 1) ? '|' : '\n';
        }
    }
    buf.result[buf.size] = '\0';
    sqlite3_reset(stmt);
    return buf.result;
}

int console_print(void *data, int argc, char **argv, char **col_names)
{
    for(int i = 0; i < argc; i++)
    {
        printf("%s = %s | ", col_names[i], argv[i]);
    }
    printf("\n");
    return 0;
}

char *db_stmt_build_execute_string_return(uint32_t n, const char **input, uint8_t *type, const char *sql)
{
    char *result = NULL;

    SqliteDB *db = sqlitedb_open("tasks.db");
    if(!db || db->last_rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", db ? sqlite3_errmsg(db->db) : "Failed to create pointer for db - Check Memory Allocation");
        return 0;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", sqlite3_errmsg(db->db));
        sqlitedb_close(db);
        return 0;
    }
    else
    {
        for(int i = 0; i < n; i++)
        {
            if (type[i] == 1)
            {
                sqlite3_bind_text(stmt, i+1, input[i], -1, SQLITE_STATIC);
            }
            else if (type[i] == 2)
            {
                int tmp = atoi(input[i]);
                sqlite3_bind_int(stmt, i+1, tmp);
            }
            else
                printf("ERROR: Unknown type %d for parameter %d\n", type[i], i+1);
        }
    }

    result = sqlitedb_excute_stmt_result_as_string(db, stmt);

    sqlite3_finalize(stmt);
    sqlitedb_close(db);
    return result;
}

void db_stmt_build_execute(uint32_t n, const char **input, uint8_t *type, const char *sql)
{

    SqliteDB *db = sqlitedb_open("tasks.db");
    if(!db || db->last_rc != SQLITE_OK)
    {
        printf("ERROR1: %s\n", db ? sqlite3_errmsg(db->db) : "Failed to create pointer for db - Check Memory Allocation");
        return;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db->db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        printf("ERROR2: %s (SQLite error code: %d)\n",
               sqlite3_errmsg(db->db), rc);
        sqlitedb_close(db);
        return;
    }
    else
    {
        for(int i = 0; i < n; i++)
        {
            if (type[i] == 1)
            {
                sqlite3_bind_text(stmt, i+1, input[i], -1, SQLITE_STATIC);
            }
            else if (type[i] == 2)
            {
                int tmp = atoi(input[i]);
                sqlite3_bind_int(stmt, i+1, tmp);
            }
            else
                printf("ERROR: Unknown type %d for parameter %d\n", type[i], i+1);
        }
    }

    rc = sqlitedb_execute_stmt(db, stmt);
    if (rc != SQLITE_OK)
    {
        printf("ERROR3: %s\n", sqlite3_errmsg(db->db));
    }
    sqlite3_finalize(stmt);
    sqlitedb_close(db);
}

bool time_check(char *input)
{
    uint8_t hourDigit1 = input[0] - '0';
    uint8_t hourDigit2 = input[1] - '0';
    uint8_t minDigit1 = input[3] - '0';

    if (hourDigit1 > 2) return false;
    if (hourDigit1 == 2)
        if (hourDigit2 > 3) return false;

    if (minDigit1 > 6) return false;

    return true;


}

int time_input_helper(char *input)
{
    uint32_t length = strlen(input);
    bool doublePoint = false;

    for (int i = 0; i < length; i++)
    {
        if (input[i] == ':') doublePoint = true;
    }

    if (doublePoint)
    {
        if (length == 4 && input[1] == ':')
        {
            char fixedTime[6] = {0};
            fixedTime[0] = '0';
            fixedTime[1] = input[0];
            fixedTime[2] = input[1];
            fixedTime[3] = input[2];
            fixedTime[4] = input[3];
            fixedTime[5] = '\0';
            input = realloc(input, 6);
            strcpy(input, fixedTime);
            return time_check(input) ? 1 : -1;
        }
        else if (length == 5 && input[2] == ':')
            return time_check(input) ? 1 : -1;
        else
            return -1;
    }
    else
    {
        if (length == 1)
        {
            char fixedTime[6] = {0};
            fixedTime[0] = '0';
            fixedTime[1] = input[0];
            fixedTime[2] = ':';
            fixedTime[3] = '0';
            fixedTime[4] = '0';
            fixedTime[5] = '\0';
            input = realloc(input, 6);
            strcpy(input, fixedTime);
            return time_check(input) ? 1 : -1;
        }
        else if (length == 2)
        {
            char fixedTime[6] = {0};
            fixedTime[0] = input[0];
            fixedTime[1] = input[1];
            fixedTime[2] = ':';
            fixedTime[3] = '0';
            fixedTime[4] = '0';
            fixedTime[5] = '\0';
            input = realloc(input, 6);
            strcpy(input, fixedTime);
            return time_check(input) ? 1 : -1;
        }
        else
            return -1;
    }
}


char *time_string_from_timestamp(uint32_t timestamp)
{
    time_t ts = (time_t)timestamp;
    struct tm *tm_info = localtime(&ts);
    static char buffer[9];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", tm_info);
    return buffer;
}

char *date_string_from_timestamp(uint32_t timestamp)
{
    time_t ts = (time_t)timestamp;
    struct tm *tm_info = localtime(&ts);
    static char buffer[11];
    strftime(buffer, sizeof(buffer), "%d.%m.%Y", tm_info);
    return buffer;
}

char *date_and_time_string_from_timestamp(uint32_t timestamp)
{
    time_t ts = (time_t)timestamp;
    struct tm *tm_info = localtime(&ts);
    static char buffer[20];
    strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M:%S", tm_info);
    return buffer;
}


void direct_sql(char *dbPath, char *sql)
{
    SqliteDB *db = sqlitedb_open(dbPath);
    if (!db)
    {
        printf("ERROR - Cannot open database\n");
        return;
    }

    if (sqlite3_exec(db, sql, console_print, NULL, NULL)  != SQLITE_OK)
    {
        printf("SQL error: %s\n", sqlitedb_error(db));
    }
    else
    {
        printf("SQL executed successfully\n");
    }

    sqlitedb_close(db);
}
/*printf("===== DEBUG: SQL STATEMENT =====\n");
    printf("SQL: %s\n", sql);
    printf("Number of inputs: %u\n", n);

    for (uint32_t i = 0; i < n; i++)
    {
        char *type_str;
        switch(type[i])
        {
        case 1:
            type_str = "TEXT";
            break;
        case 2:
            type_str = "INT";
            break;
        case 3:
            type_str = "";
            break;
        case 4:
            type_str = "";
            break;
        case 5:
            type_str = "";
            break;
        default:
            type_str = "UNKNOWN";
            break;
        }

        printf("Input %u: [%s] = %s\n",
               i,
               type_str,
               input[i] ? input[i] : "NULL");
    }
    printf("===============================\n");
*/

