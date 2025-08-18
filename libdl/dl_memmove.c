#include "libdl.h"

void *dl_memmove(void *dst, const void *src, size_t len)
{
    unsigned char *d;
    const unsigned char *s;

    if (dst == src || len == 0)
        return (dst);
    
    d = (unsigned char *)dst;
    s = (const unsigned char *)src;
    
    if (d < s)
    {
        while (len--)
            *d++ = *s++;
    }
    else
    {
        d += len;
        s += len;
        while (len--)
            *--d = *--s;
    }
    
    return (dst);
}
