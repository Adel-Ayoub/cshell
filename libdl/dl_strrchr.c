#include "libdl.h"

char *dl_strrchr(const char *s, int c)
{
    int i;
    char *last;

    last = NULL;
    i = 0;
    while (s[i])
    {
        if (s[i] == (char)c)
            last = (char *)&s[i];
        i++;
    }
    if (s[i] == (char)c)
        last = (char *)&s[i];
    return (last);
}
