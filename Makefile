CC = gcc
CFLAGS = -Wall -Wextra -I./include
LDFLAGS = -lsqlite3

# Define source files
SQLITEDB_SRC = lib/sqlitedb/sqlitedb.c
TASKMAN_SRC = src/main.c src/task_ops.c src/date_utils.c src/string_utils.c

# Define object files
SQLITEDB_OBJ = $(SQLITEDB_SRC:.c=.o)
TASKMAN_OBJ = $(TASKMAN_SRC:.c=.o)

# Define targets
.PHONY: all clean

all: taskmanTesting

taskmanTesting: $(SQLITEDB_OBJ) $(TASKMAN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SQLITEDB_OBJ) $(TASKMAN_OBJ) taskman
