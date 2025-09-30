#include "sqlitedb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

/*
int append_row(void *data, int argc, char **argv, char **col_names)
{
    QueryResult *buf = (QueryResult *)data;

    for(int i = 0; i < argc; i++)
    {
        const char *val = argv[i] ? argv[i] : "NULL";
        size_t len = strlen(val);

        if (buf->capacity < buf->size + len + 2)
        {
            buf->capacity += 1024;
            buf->result = realloc(buf->result, buf->capacity);
        }

        memcpy(buf->result + buf->size, val, len);
        buf->size += len;
        buf->result[buf->size++] = (i < argc) ? '|' : '\n';

    }
    return 0;
}

char *sqlitedb_as_string(SqliteDB *db_wrapper, const char *sql)
{
    QueryResult buf = { .result = malloc(1024), .size = 0, .capacity = 1024 };
    buf.result[0] = '\0';
    db_wrapper->last_rc = sqlite3_exec(db_wrapper->db, sql, append_row, &buf, &db_wrapper->err_msg);
    buf.result[buf.size] = '\0';
    return buf.result;
}
*/

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


void db_stmt_build_execute(int n, char **input, int *type, const char *sql)
{

    SqliteDB *db = sqlitedb_open("tasks.db");
    if(!db || db->last_rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", db ? db->err_msg : "Failed to create pointer for db - Check Memory Allocation");
    }


    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", db->err_msg);
        sqlitedb_close(db);
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
                sqlite3_bind_int(stmt, i+1, 30);
            }
            else
                printf("ERROR: Unknown type %d for parameter %d\n", type[i], i+1);
        }
    }


    rc = sqlitedb_execute_stmt(db, stmt);
    if (rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", db->err_msg);
    }

    sqlite3_finalize(stmt);
    sqlitedb_close(db);
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
