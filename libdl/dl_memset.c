#include "libdl.h"

void *dl_memset(void *b, int c, size_t len)
{
    unsigned char *ptr;
    unsigned char value;

    ptr = (unsigned char *)b;
    value = (unsigned char)c;
    while (len > 0)
    {
        *ptr = value;
        ptr++;
        len--;
    }
    return (b);
}
