#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <stdbool.h>

// Date/time utilities
void set_time_date();
int daysInMonth(int day, int month, int year);
bool isLeapYear(int year);
void find_date_from_day(char *day);
char *input_where(char *when);

#endif // DATE_UTILS_H
#include <stdlib.h>
