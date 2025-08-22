#include "libdl.h"

char *dl_strcpy(char *dst, const char *src)
{
    if (!dst || !src)
        return (dst);
    
    char *ptr = dst;
    while (*src)
    {
        *ptr = *src;
        ptr++;
        src++;
    }
    *ptr = '\0';
    return (dst);
}
