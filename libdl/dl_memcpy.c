#include "libdl.h"

void *dl_memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *d;
    const unsigned char *s;

    if (dst == src || n == 0)
        return (dst);
    d = (unsigned char *)dst;
    s = (const unsigned char *)src;
    while (n > 0)
    {
        *d = *s;
        d++;
        s++;
        n--;
    }
    return (dst);
}
