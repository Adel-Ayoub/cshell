#include "libdl.h"

char *dl_strnstr(const char *haystack, const char *needle, size_t len)
{
    size_t i;
    size_t j;

    if (!*needle)
        return ((char *)haystack);
    
    i = 0;
    while (haystack[i] && i < len)
    {
        j = 0;
        while (needle[j] && haystack[i + j] && i + j < len)
        {
            if (haystack[i + j] != needle[j])
                break;
            j++;
        }
        if (!needle[j])
            return ((char *)&haystack[i]);
        i++;
    }
    return (NULL);
}
