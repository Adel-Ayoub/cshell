#include "cshell.h"

int exit_atoi(const char *str)
{
    long result;
    int sign;
    int i;

    result = 0;
    sign = 1;
    i = 0;
    
    // Skip whitespace
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' ||
           str[i] == '\v' || str[i] == '\f' || str[i] == '\r')
        i++;
    
    // Handle sign
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            sign = -1;
        i++;
    }
    
    // Convert digits
    while (str[i] >= '0' && str[i] <= '9')
    {
        result = result * 10 + (str[i] - '0');
        
        // Check for overflow
        if (result > 255)
        {
            if (sign == 1)
                return (255);
            else
                return (0);
        }
        
        i++;
    }
    
    // Check for non-digit characters
    if (str[i] != '\0')
        return (-1);
    
    return ((int)(result * sign));
}
