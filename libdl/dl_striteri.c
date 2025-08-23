#include "libdl.h"

void dl_striteri(char *s, void (*f)(unsigned int, char*))
{
    int i;
    
    if (!s || !f)
        return;
    
    i = 0;
    while (s[i] != '\0')
    {
        f(i, &s[i]);
        i++;
    }
}
