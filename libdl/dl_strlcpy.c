#include "libdl.h"

size_t dl_strlcpy(char *dst, const char *src, size_t dstsize)
{
    size_t i;
    size_t len;
    
    i = 0;
    len = 0;
    
    while (src[len] != '\0')
        len++;
    
    if (dstsize == 0)
        return (len);
    
    while (i + 1 < dstsize && i <= len && src[i] != '\0')
    {
        dst[i] = src[i];
        i++;
    }
    
    dst[i] = '\0';
    return (len);
}
