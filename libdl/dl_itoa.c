#include "libdl.h"

static int count_digits(int n)
{
    int count;
    long num;

    count = 0;
    num = n;
    if (num < 0)
    {
        count++;
        num = -num;
    }
    if (num == 0)
        return (1);
    while (num > 0)
    {
        count++;
        num /= 10;
    }
    return (count);
}

char *dl_itoa(int n)
{
    char *str;
    int len;
    long num;
    int i;

    len = count_digits(n);
    str = (char *)dl_calloc(len + 1, sizeof(char));
    if (!str)
        return (NULL);
    
    num = n;
    if (num < 0)
    {
        str[0] = '-';
        num = -num;
    }
    
    i = len - 1;
    if (n == 0)
        str[0] = '0';
    else
    {
        while (num > 0)
        {
            str[i] = (num % 10) + '0';
            num /= 10;
            i--;
        }
    }
    
    return (str);
}
