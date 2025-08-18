#include "libdl.h"

size_t dl_strlen(const char *s)
{
    size_t len;

    len = 0;
    while (s[len] != '\0')
        len++;
    return (len);
}
