#include "date_utils.h"
#include "string_utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
/*
time_t t;
struct tm tm;
int currentDay;
int currentMonth;
int currentYear;
int currentHour;
int currentMin;
uint8_t dayInWeek;
*/

void set_time_date(struct tm *t)
{
    time_t now = time(NULL);
    *t = *localtime(&now);
    t->tm_mon += 1;
    t->tm_year += 1900;
}


uint32_t seconds_since_midnight_minAccuracy()
{
    struct tm t = {0};
    set_time_date(&t);

    return (t.tm_hour * 3600 + t.tm_min * 60);
}

bool isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int month, int year)
{
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2)
        return isLeapYear(year) ? 29 : 28;
    return days[month - 1];
}

char *date_calculator_from_range(uint32_t range)
{

    struct tm t = {0};
    set_time_date(&t);


    char *dateString = malloc(11 * sizeof(char));
    if (dateString == NULL) return NULL;

    while (range != 0)
    {
        t.tm_mday++;
        if (t.tm_mday > daysInMonth(t.tm_mon, t.tm_year))
        {
            t.tm_mday = 1;
            t.tm_mon++;
            if (t.tm_mon > 12)
            {
                t.tm_mon = 1;
                t.tm_year++;
            }
        }
        range--;
    }

    snprintf(dateString, 11, "%02d-%02d-%04d", t.tm_mday, t.tm_mon, t.tm_year);
    return dateString;
}



void print_week_and_day(char *dateString)
{

    const char *days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    char daySt[3];
    char monSt[3];
    char yearSt[5];

    char weekStr[3];

    memcpy(daySt, dateString, 2);
    daySt[2] = '\0';
    memcpy(monSt, dateString + 3, 2);
    monSt[2] = '\0';
    memcpy(yearSt, dateString + 6, 4);
    yearSt[4] = '\0';

    uint8_t day = atoi(daySt);
    uint8_t mon = atoi(monSt);
    uint16_t year = atoi(yearSt);

    struct tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = mon - 1;
    tm.tm_mday = day;
    mktime(&tm);
    strftime(weekStr, sizeof(weekStr), "%U", &tm);

    uint8_t dayInWeek = tm.tm_wday;

    dayInWeek == 0 ? printf("\n\n\t\tWeek: %s        \n", weekStr) : printf("");
    printf("---- %s ----\n", days[dayInWeek]);
}


void find_date_from_day(char *day, int *dayInWeek, int *currentDay, int *currentMonth, int *currentYear)
{
    int adding = 0;
    if (strcmp(day, "tom") == 0)
        adding = 1;
    else
    {
        const char *days[] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
        for (int i = *dayInWeek; i <= 6 ; )
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
        (*currentDay)++;
        if (*currentDay > daysInMonth(*currentMonth, *currentYear))
        {
            *currentDay = 1;
            (*currentMonth)++;
            if (*currentMonth > 12)
            {
                *currentMonth = 1;
                (*currentYear)++;

            }
        }
        adding--;
    }

}



bool date_validate(char *date)
{
    struct tm t = {0};
    set_time_date(&t);

    uint16_t year;
    uint8_t month;
    uint8_t day;

    sscanf(date, "%hu-%hhu-%hhu", &year, &month, &day);

    if (year < t.tm_year) return false;
    if (month > 12) return false;
    if (day > daysInMonth(month, year)) return false;

    if (year == t.tm_year)
    {
        if (month < t.tm_mon) return false;
        if (month == t.tm_mon)
            if (day < t.tm_mday) return false;
    }

    return true;
}

int input_where(char *when, char **result)
{
    if (when[0] == ' ') trim(when);
    if (when[0] == '-')
    {
        return 2;
    }

    *result = malloc(sizeof(char) * 12);
    if (*result == NULL) return -1;

    struct tm t = {0};
    set_time_date(&t);
    int currentDay = t.tm_mday;
    int currentMonth = t.tm_mon;
    int currentYear = t.tm_year;
    int dayInWeek = t.tm_wday;

    if (!isdigit(when[0]))
    {
        if (strcmp(when, "mon") == 0 || strcmp(when, "monday") == 0)
        {
            find_date_from_day("mon", &dayInWeek, &currentDay, &currentMonth, &currentYear);

            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "tue") == 0 || strcmp(when, "tuesday") == 0)
        {
            find_date_from_day("tue", &dayInWeek, &currentDay, &currentMonth, &currentYear);
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "wed") == 0 || strcmp(when, "wednesday") == 0)
        {
            find_date_from_day("wed", &dayInWeek, &currentDay, &currentMonth, &currentYear);
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "thu") == 0 || strcmp(when, "thur") == 0 || strcmp(when, "thursday") == 0)
        {
            find_date_from_day("thu", &dayInWeek, &currentDay, &currentMonth, &currentYear);
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "fri") == 0 || strcmp(when, "friday") == 0)
        {
            find_date_from_day("fri", &dayInWeek, &currentDay, &currentMonth, &currentYear);
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "sat") == 0 || strcmp(when, "saturday") == 0)
        {
            find_date_from_day("sat", &dayInWeek, &currentDay, &currentMonth, &currentYear);
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "sun") == 0 || strcmp(when, "sunday") == 0)
        {
            find_date_from_day("sun", &dayInWeek, &currentDay, &currentMonth, &currentYear);
            snprintf(*result, 12, "%04d-%02d-%02d", currentYear, currentMonth,
                     currentDay);
            return 1;
        }
        else if (strcmp(when, "tomorrow") == 0 || strcmp(when, "tom") == 0)
        {
            find_date_from_day("tom", &dayInWeek, &currentDay, &currentMonth, &currentYear);
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
        char delimiter[2] = {0}; //allowing any date syntax to be processed
        int i = 0;
        while (isdigit(when[i]) && when[i] != '\0')
        {
            i++;
        }
        if (when[i] != '\0') delimiter[0] = when[i];
        else delimiter[0] = '.';

        char *token = strtok(when, delimiter);
        if (!token)
        {
            return -1;
        }
        trim(token);
        int day = atoi(token);
        int month;
        int year;

        token = strtok(NULL, delimiter);
        if (token)
        {
            trim(token);
            month = atoi(token);

            token = strtok(NULL, delimiter);
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

bool time_check(char *input)
{
    uint8_t hourDigit1 = input[0] - '0';
    uint8_t hourDigit2 = input[1] - '0';
    uint8_t minDigit1 = input[3] - '0';

    if (hourDigit1 > 2) return false;
    if (hourDigit1 == 2)
        if (hourDigit2 > 3) return false;

    if (minDigit1 > 5) return false;

    return true;
}

int time_input_helper(char **input)
{
    uint32_t length = strlen(*input);
    bool doublePoint = false;

    for (int i = 0; i < length; i++)
    {
        if ((*input)[i] == ':') doublePoint = true;
    }

    if (doublePoint)
    {
        if (length == 4 && (*input)[1] == ':')
        {
            char fixedTime[6] = {0};
            fixedTime[0] = '0';
            fixedTime[1] = (*input)[0];
            fixedTime[2] = (*input)[1];
            fixedTime[3] = (*input)[2];
            fixedTime[4] = (*input)[3];
            fixedTime[5] = '\0';

            *input = realloc(*input, 6);
            if (*input == NULL) return -1;
            strcpy(*input, fixedTime);
            return time_check(*input) ? 1 : -1;
        }
        else if (length == 5 && (*input)[2] == ':')
            return time_check(*input) ? 1 : -1;
        else
            return -1;
    }
    else
    {
        if (length == 1)
        {
            char fixedTime[6] = {0};
            fixedTime[0] = '0';
            fixedTime[1] = (*input)[0];
            fixedTime[2] = ':';
            fixedTime[3] = '0';
            fixedTime[4] = '0';
            fixedTime[5] = '\0';

            *input = realloc(*input, 6);
            if (*input == NULL) return -1;
            strcpy(*input, fixedTime);
            return time_check(*input) ? 1 : -1;
        }
        else if (length == 2)
        {
            char fixedTime[6] = {0};
            fixedTime[0] = (*input)[0];
            fixedTime[1] = (*input)[1];
            fixedTime[2] = ':';
            fixedTime[3] = '0';
            fixedTime[4] = '0';
            fixedTime[5] = '\0';

            *input = realloc(*input, 6);
            if (*input == NULL) return -1;
            strcpy(*input, fixedTime);
            return time_check(*input) ? 1 : -1;
        }
        else
            return -1;
    }
}



