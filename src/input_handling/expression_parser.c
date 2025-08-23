#include "cshell.h"

// Parse a complex expression with proper operator precedence
int parse_complex_expression(const char *str, char **left, char **right, int *operator_type)
{
    if (!str || !left || !right || !operator_type)
        return (0);
    
    int len = dl_strlen(str);
    int i = len - 1;
    
    // Start from the end and find the lowest precedence operator
    // Precedence: && > || > &
    
    // First, look for && (highest precedence)
    while (i > 0)
    {
        if (str[i] == '&' && str[i - 1] == '&')
        {
            *operator_type = 1; // AND operator
            
            // Extract left part (before &&)
            *left = dl_calloc(i, sizeof(char));
            if (!*left)
                return (0);
            dl_strncpy(*left, str, i - 1);
            (*left)[i - 1] = '\0';
            
            // Extract right part (after &&)
            *right = dl_strdup(str + i + 1);
            if (!*right)
            {
                free(*left);
                *left = NULL;
                return (0);
            }
            
            return (1);
        }
        i--;
    }
    
    // If no &&, look for || (medium precedence)
    i = len - 1;
    while (i > 0)
    {
        if (str[i] == '|' && str[i - 1] == '|')
        {
            *operator_type = 2; // OR operator
            
            // Extract left part (before ||)
            *left = dl_calloc(i, sizeof(char));
            if (!*left)
                return (0);
            dl_strncpy(*left, str, i - 1);
            (*left)[i - 1] = '\0';
            
            // Extract right part (after ||)
            *right = dl_strdup(str + i + 1);
            if (!*right)
            {
                free(*left);
                *left = NULL;
                return (0);
            }
            
            return (1);
        }
        i--;
    }
    
    // If no && or ||, look for single & (lowest precedence)
    i = len - 1;
    while (i >= 0)
    {
        if (str[i] == '&' && (i == 0 || str[i - 1] != '&'))
        {
            *operator_type = 3; // Background operator
            
            // Extract left part (before &)
            *left = dl_calloc(i + 1, sizeof(char));
            if (!*left)
                return (0);
            dl_strncpy(*left, str, i);
            (*left)[i] = '\0';
            
            // Background jobs don't have a right part
            *right = NULL;
            
            return (1);
        }
        i--;
    }
    
    // No operators found
    return (0);
}
