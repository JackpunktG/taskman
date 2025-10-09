CC = gcc
CFLAGS = -Wall -Wextra -I./include
LDFLAGS = -lsqlite3

# Define source files
SQLITEDB_SRC = lib/sqlitedb/sqlitedb.c
TASKMAN_SRC = src/main.c src/task_ops.c src/date_utils.c src/string_utils.c

# Define object files
SQLITEDB_OBJ = $(SQLITEDB_SRC:.c=.o)
TASKMAN_OBJ = $(TASKMAN_SRC:.c=.o)

# db path
DB_PATH ?= tasks.db
CFLAGS += -DDB_PATH=\"$(DB_PATH)\"

# Define targets
.PHONY: all clean

OUT ?= taskman

all: $(SQLITEDB_OBJ) $(TASKMAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $(OUT) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SQLITEDB_OBJ) $(TASKMAN_OBJ) taskman
