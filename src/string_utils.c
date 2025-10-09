#include "string_utils.h"
#include <string.h>
#include <ctype.h>

void trim(char *str)
{
    char *start = str;
    char *end;

    while (*start && isspace((unsigned char)*start))
    {
        start++;
    }

    if (*start == 0)
    {
        str[0] = '\0';
        return;
    }

    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end))
    {
        end--;
    }

    *(end + 1) = '\0';

    memmove(str, start, end - start + 2);  // +1 for '\0', +1 for end-start indexing
}
