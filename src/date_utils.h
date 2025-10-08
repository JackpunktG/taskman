#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <stdbool.h>
#include <stdint.h>

// Date/time utilities
void set_time_date();
int daysInMonth(int day, int month, int year);
bool isLeapYear(int year);
char *date_calculator_from_range(uint32_t range);
void find_date_from_day(char *day);
// Parses input like "today", "tomorrow", "mon", "25.12.2023" and returns date in "YYYY-MM-DD" format
// Returns 1 on success, 0 on failure
int input_where(char *when, char **result);

// helper function for configuring input to HH:MM
// then checks to see if vaild time string. returns 1 for all good or -1 for inval
int time_input_helper(char **input);

void print_week_and_day(char *dateString);
#endif // DATE_UTILS_H
#include <stdlib.h>
