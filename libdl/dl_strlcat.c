#include "libdl.h"

size_t dl_strlcat(char *dst, const char *src, size_t dstsize)
{
    size_t src_len;
    size_t dst_len;
    
    src_len = dl_strlen(src);
    dst_len = 0;
    
    if (dstsize == 0)
        return (src_len);
    
    while (*dst && dstsize > 0)
    {
        dst_len++;
        dst++;
        dstsize--;
    }
    
    while (*src && dstsize > 1)
    {
        *dst = *src;
        dst++;
        src++;
        dstsize--;
    }
    
    if (dstsize == 1 || *src == 0)
        *dst = '\0';
    
    return (dst_len + src_len);
}
