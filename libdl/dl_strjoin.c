#include "libdl.h"

char *dl_strjoin(char const *s1, char const *s2)
{
    char *result;
    size_t len1;
    size_t len2;
    size_t i;
    size_t j;

    if (!s1 || !s2)
        return (NULL);
    
    len1 = dl_strlen(s1);
    len2 = dl_strlen(s2);
    
    result = (char *)dl_calloc(len1 + len2 + 1, sizeof(char));
    if (!result)
        return (NULL);
    
    i = 0;
    while (i < len1)
    {
        result[i] = s1[i];
        i++;
    }
    
    j = 0;
    while (j < len2)
    {
        result[i + j] = s2[j];
        j++;
    }
    
    return (result);
}
