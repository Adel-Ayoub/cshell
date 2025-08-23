#include "cshell.h"

// Check if a string contains logical operators or background operator
int has_logical_operators(const char *str)
{
    if (!str)
        return (0);
    
    for (int i = 0; str[i]; i++)
    {
        if (str[i] == '&' && str[i + 1] == '&')
            return (1);
        if (str[i] == '|' && str[i + 1] == '|')
            return (1);
        if (str[i] == '&' && str[i + 1] != '&')
            return (1);  // Single & for background jobs
    }
    return (0);
}

// Find the position of the first logical operator or background operator
int find_logical_operator(const char *str, int *operator_type)
{
    if (!str || !operator_type)
        return (-1);
    
    for (int i = 0; str[i]; i++)
    {
        if (str[i] == '&' && str[i + 1] == '&')
        {
            *operator_type = 1; // AND operator
            return (i);
        }
        if (str[i] == '|' && str[i + 1] == '|')
        {
            *operator_type = 2; // OR operator
            return (i);
        }
        if (str[i] == '&' && str[i + 1] != '&')
        {
            *operator_type = 3; // Background operator
            return (i);
        }
    }
    return (-1);
}

// Parse a string into left and right parts based on logical operator
int parse_logical_expression(const char *str, char **left, char **right, int *operator_type)
{
    int op_pos;
    
    if (!str || !left || !right || !operator_type)
        return (0);
    
    op_pos = find_logical_operator(str, operator_type);
    if (op_pos == -1)
        return (0);
    
    // Extract left part (before operator)
    *left = dl_calloc(op_pos + 1, sizeof(char));
    if (!*left)
        return (0);
    dl_strncpy(*left, str, op_pos);
    (*left)[op_pos] = '\0';
    
    // For background operator (&), there's no right part
    if (*operator_type == 3)
    {
        *right = NULL;
        return (1);
    }
    
    // Extract right part (after operator) for && and ||
    int skip_chars = (*operator_type == 1 || *operator_type == 2) ? 2 : 1;
    *right = dl_strdup(str + op_pos + skip_chars);
    if (!*right)
    {
        free(*left);
        *left = NULL;
        return (0);
    }
    
    return (1);
}

// Clean up logical expression parsing
void cleanup_logical_expression(char **left, char **right)
{
    if (left && *left)
    {
        free(*left);
        *left = NULL;
    }
    if (right && *right)
    {
        free(*right);
        *right = NULL;
    }
}
