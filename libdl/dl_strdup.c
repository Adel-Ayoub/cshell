#include "libdl.h"

char *dl_strdup(const char *s1)
{
    char *dup;
    size_t len;

    len = dl_strlen(s1) + 1;
    dup = (char *)malloc(len);
    if (dup)
        dl_memcpy(dup, s1, len);
    return (dup);
}
