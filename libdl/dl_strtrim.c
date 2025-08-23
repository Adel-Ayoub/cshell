#include "libdl.h"

static int is_in_set(char c, char const *set)
{
    while (*set)
    {
        if (c == *set)
            return (1);
        set++;
    }
    return (0);
}

static void trim_beg(char const *s, char const *set, int *beg)
{
    *beg = 0;
    while (s[*beg] && is_in_set(s[*beg], set))
        (*beg)++;
}

static void trim_end(char const *s, char const *set, int *len, int *end)
{
    *end = *len;
    while (*end > 0 && is_in_set(s[*end - 1], set))
        (*end)--;
}

char *dl_strtrim(char const *s1, char const *set)
{
    int beg;
    int end;
    int len;
    char *result;
    int i;
    
    if (!s1 || !set)
        return (NULL);
    
    len = dl_strlen(s1);
    trim_beg(s1, set, &beg);
    end = beg;
    trim_end(s1, set, &len, &end);
    
    len = end - beg;
    result = dl_calloc(len + 1, sizeof(char));
    if (!result)
        return (NULL);
    
    i = 0;
    while (beg < end)
    {
        result[i] = s1[beg];
        beg++;
        i++;
    }
    
    result[i] = '\0';
    return (result);
}
