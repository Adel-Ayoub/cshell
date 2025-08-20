#include "libdl.h"

int dl_strcmp(const char *s1, const char *s2)
{
    if (!s1 && !s2)
        return (0);
    if (!s1)
        return (-1);
    if (!s2)
        return (1);
    
    while (*s1 && *s2 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    
    return ((unsigned char)*s1 - (unsigned char)*s2);
}
