#include "date_utils.h"
#include "string_utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

time_t t;
struct tm tm;
int currentDay;
int currentMonth;
int currentYear;
int currentHour;
int currentMin;
uint8_t dayInWeek;


void set_time_date()
{
    t = time(NULL);
    tm = *localtime(&t);
    currentDay = tm.tm_mday;
    currentMonth = tm.tm_mon + 1;
    currentYear = tm.tm_year + 1900;
    currentHour = tm.tm_hour;
    currentMin = tm.tm_min;
    dayInWeek = tm.tm_wday;
}

bool isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int day, int month, int year)
{
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2)
        return isLeapYear(year) ? 29 : 28;
    return days[month - 1];
}

void find_date_from_day(char *day)
{
    int adding = 0;
    if (strcmp(day, "tom") == 0)
        adding = 1;
    else
    {
        const char *days[] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
        for (int i = dayInWeek; i <= 6 ; )
        {
            adding++;
            if (strcmp(day, days[i]) == 0)
                break;
            if (i == 6)
                i = 0;
            else
                i++;
        }
        adding--; //to not count today
        if (adding == 0)
            adding = 7; //next week
    }
    while (adding != 0)
    {
        currentDay++;
        if (currentDay > daysInMonth(currentDay, currentMonth, currentYear))
        {
            currentDay = 1;
            currentMonth++;
            if (currentMonth > 12)
            {
                currentMonth = 1;
                currentYear++;
            }
        }
        adding--;
    }

}

bool date_validate(char *date)
{
    uint16_t year;
    uint8_t month;
    uint8_t day;

    sscanf(date, "%hu-%hhu-%hhu", &year, &month, &day);

    if (year < 2025) return false;
    if (month > 12) return false;
    if (day > daysInMonth(day, month, year)) return false;

    return true;
}

int input_where(char *when, char **result)
{
    trim(when);
    if (when[0] == '-')
    {
        return 2;
    }

    *result = malloc(sizeof(char) * 12);

    if (!isdigit(when[0]))
    {
        if (strcmp(when, "mon") == 0 || strcmp(when, "monday") == 0)
        {
            find_date_from_day("mon");

            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "tue") == 0 || strcmp(when, "tuesday") == 0)
        {
            find_date_from_day("tue");
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "wed") == 0 || strcmp(when, "wednesday") == 0)
        {
            find_date_from_day("wed");
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "thu") == 0 || strcmp(when, "thur") == 0 || strcmp(when, "thursday") == 0)
        {
            find_date_from_day("thu");
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "fri") == 0 || strcmp(when, "friday") == 0)
        {
            find_date_from_day("fri");
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "sat") == 0 || strcmp(when, "saturday") == 0)
        {
            find_date_from_day("sat");
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "sun") == 0 || strcmp(when, "sunday") == 0)
        {
            find_date_from_day("sun");
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "tomorrow") == 0 || strcmp(when, "tom") == 0)
        {
            find_date_from_day("tom");
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "today") == 0 || strcmp(when, "tod") == 0)
        {

            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;

        }
        else
        {
            return -1;
        }
    }
    else
    {
        char *token = strtok(when, ".");
        if (!token)
        {
            return -1;
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

            token = strtok(NULL, ".");
            if (token)
            {
                trim(token);
                year = atoi(token);
                snprintf(*result, 12, "%04d-%02d-%02d", year, month, day);
            }
            else
            {
                snprintf(*result, 12, "%04d-%02d-%02d", currentYear, month, day);
            }
        }
        else
        {
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     day);
        }

        if (!date_validate(*result))
        {
            return -2;
        }
        else return 1;


    }
}


