#include "libdl.h"

char *dl_strmapi(char const *s, char (*f)(unsigned int, char))
{
    int i;
    char *result;
    
    if (!s || !f)
        return (NULL);
    
    i = 0;
    while (s[i] != '\0')
        i++;
    
    result = dl_calloc(i + 1, sizeof(char));
    if (!result)
        return (NULL);
    
    i = 0;
    while (s[i] != '\0')
    {
        result[i] = f(i, s[i]);
        i++;
    }
    
    result[i] = '\0';
    return (result);
}
