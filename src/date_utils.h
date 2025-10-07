#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <stdbool.h>

// Date/time utilities
void set_time_date();
int daysInMonth(int day, int month, int year);
bool isLeapYear(int year);
void find_date_from_day(char *day);
// Parses input like "today", "tomorrow", "mon", "25.12.2023" and returns date in "YYYY-MM-DD" format
// Returns 1 on success, 0 on failure
int input_where(char *when, char **result);

#endif // DATE_UTILS_H
#include <stdlib.h>
