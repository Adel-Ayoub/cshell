#include "libdl.h"

char *dl_strncpy(char *dest, const char *src, size_t n)
{
    size_t i;
    
    if (!dest || !src)
        return (dest);
    
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    
    // Pad with null bytes if n > strlen(src)
    for (; i < n; i++)
        dest[i] = '\0';
    
    return (dest);
}
