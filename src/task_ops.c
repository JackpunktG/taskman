#include "task_ops.h"
#include "date_utils.h"
#include "string_utils.h"
#include "../lib/sqlitedb/sqlitedb.h"
#include <sqlite3.h>
#include <stdio.h>



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

    const char *sql = "DELETE FROM task WHERE id = ?";
    char *input[] = { id };
    int type[] = { 2 };
    db_stmt_build_execute(1, input, type, sql);
}


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
