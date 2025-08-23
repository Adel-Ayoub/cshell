#include "libdl.h"

char *dl_substr(char const *s, unsigned int start, size_t len)
{
    char *result;
    size_t size;
    
    if (!s)
        return (NULL);
    
    if (dl_strlen(s) < len)
        len = dl_strlen(s);
    
    result = dl_calloc(len + 1, sizeof(char));
    if (!result)
        return (NULL);
    
    if (start >= dl_strlen(s))
    {
        result[0] = '\0';
        return (result);
    }
    
    size = 0;
    while (s[start] != '\0' && size != len)
    {
        result[size] = s[start];
        size++;
        start++;
    }
    
    result[size] = '\0';
    return (result);
}
