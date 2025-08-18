#include "libdl.h"

void *dl_memchr(const void *s, int c, size_t n)
{
    const unsigned char *str;
    unsigned char ch;

    str = (const unsigned char *)s;
    ch = (unsigned char)c;
    
    while (n--)
    {
        if (*str == ch)
            return ((void *)str);
        str++;
    }
    
    return (NULL);
}
