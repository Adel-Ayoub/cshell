#include "libdl.h"

static char *strtok_save = NULL;

char *dl_strtok(char *str, const char *delim)
{
    char *token;
    char *end;

    if (str)
        strtok_save = str;
    else if (!strtok_save)
        return (NULL);
    
    // Skip leading delimiters
    while (*strtok_save && dl_strchr(delim, *strtok_save))
        strtok_save++;
    
    if (!*strtok_save)
    {
        strtok_save = NULL;
        return (NULL);
    }
    
    // Find end of token
    token = strtok_save;
    end = strtok_save;
    while (*end && !dl_strchr(delim, *end))
        end++;
    
    if (*end)
    {
        *end = '\0';
        strtok_save = end + 1;
    }
    else
    {
        strtok_save = NULL;
    }
    
    return (token);
}
