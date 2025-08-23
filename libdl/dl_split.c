#include "libdl.h"

static int string_amount(char const *str, char c)
{
    int count;
    int i;
    
    count = 0;
    i = 0;
    
    while (str[i])
    {
        if (str[i] != c)
        {
            count++;
            while (str[i] && str[i] != c)
                i++;
        }
        else
            i++;
    }
    return (count);
}

static char **allocate_strings(char const *str, char c, int count)
{
    char **result;
    int i;
    int len;
    
    result = dl_calloc(count + 1, sizeof(char *));
    if (!result)
        return (NULL);
    
    i = 0;
    while (i < count)
    {
        len = 0;
        while (*str == c)
            str++;
        while (str[len] && str[len] != c)
            len++;
        
        result[i] = dl_calloc(len + 1, sizeof(char));
        if (!result[i])
        {
            // Cleanup on failure
            while (i > 0)
                free(result[--i]);
            free(result);
            return (NULL);
        }
        
        dl_strncpy(result[i], str, len);
        str += len;
        i++;
    }
    
    return (result);
}

char **dl_split(char const *s, char c)
{
    int count;
    
    if (!s)
        return (NULL);
    
    count = string_amount(s, c);
    return (allocate_strings(s, c, count));
}
