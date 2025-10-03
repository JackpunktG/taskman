#ifndef SQLITEDB_H
#define SQLITEDB_H

#include <sqlite3.h>
#include <stdint.h>
typedef struct
{
    sqlite3 *db;
    char *err_msg;
    int last_rc;
} SqliteDB;

// Initialize a database connection and return a handle
// Input: filename - path to the SQLite database file
SqliteDB *sqlitedb_open(const char *filename);

// Close a database connection
void sqlitedb_close(SqliteDB *db_wrapper);

// Execute a preprapared *stmt SQL statement without results
int sqlitedb_execute_stmt(SqliteDB *db_wrapper, sqlite3_stmt *stmt);


// Get error message
const char *sqlitedb_error(SqliteDB *db);

// Execute a SQL query (*stmt) that returns a single string result - per line '|' delimited
// Must be freed by caller
char *sqlitedb_excute_stmt_result_as_string(SqliteDB *db_wrapper, sqlite3_stmt *stmt);

// Builds and executes an SQL query
// n - number of parameters
// input - array of parameter values (as strings)
// type - array of parameter types (1 = text, 2 = int)
// sql - SQL query with '?' placeholders for parameters
void db_stmt_build_execute(uint32_t n, const char **input, uint8_t *type, const char *sql);
// Single string result - 1 line per result '\n'. result with '|' delimited
// Must be freed by caller
char *db_stmt_build_execute_string_return(uint32_t n, const char **input, uint8_t *type, const char *sql);

// Direct SQL execution (for debugging/admin)
void direct_sql(char *dbPath, char *sql);

#endif
