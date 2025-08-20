#include "cshell.h"

int expand_environment_variables(void)
{
    // Expand environment variables in command arguments
    // This will be called during command parsing to expand $VAR in arguments
    
    // For now, return success - the actual expansion will be done
    // when processing individual arguments in the parsing phase
    return (0);
}

// Expand environment variables in an array of strings
int expand_environment_array(char **array)
{
    if (!array)
        return (0);
    
    for (int i = 0; array[i]; i++)
    {
        if (array[i] && dl_strchr(array[i], '$') != NULL)
        {
            char *expanded = expand_environment_string(array[i]);
            if (expanded)
            {
                free(array[i]);
                array[i] = expanded;
            }
        }
    }
    
    return (0);
}

// Expand environment variables in a single argument
char *expand_argument(char *arg)
{
    if (!arg)
        return (NULL);
    
    // Check if argument contains environment variables
    if (dl_strchr(arg, '$') == NULL)
        return (dl_strdup(arg));
    
    return (expand_environment_string(arg));
}

char *expand_environment_string(char *str)
{
    char *result;
    char *var_start;
    char *var_end;
    char *var_name;
    char *var_value;
    int i;
    
    if (!str)
        return (NULL);
    
    // Check if string contains any environment variables
    if (dl_strchr(str, '$') == NULL)
        return (dl_strdup(str));
    
    result = dl_calloc(1, 1); // Start with empty string
    if (!result)
        return (NULL);
    
    i = 0;
    while (str[i])
    {
        if (str[i] == '$' && (i == 0 || str[i - 1] != '\\'))
        {
            // Find variable name
            var_start = str + i + 1;
            if (*var_start == '{')
            {
                var_start++;
                var_end = dl_strchr(var_start, '}');
                if (!var_end)
                {
                    // Invalid syntax, treat as literal
                    result = dl_strjoin(result, "$");
                    i++;
                    continue;
                }
                *var_end = '\0';
                var_name = dl_strdup(var_start);
                *var_end = '}';
            }
            else
            {
                var_end = var_start;
                while (*var_end && (dl_isalnum(*var_end) || *var_end == '_'))
                    var_end++;
                var_name = dl_strdup(var_start);
                if (var_end != var_start)
                    var_name[var_end - var_start] = '\0';
            }
            
            // Get variable value
            if (var_name && *var_name)
            {
                var_value = get_environment_variable(var_name);
                if (var_value)
                {
                    result = dl_strjoin(result, var_value);
                    free(var_value);
                }
                else
                {
                    // Variable not found - check for default value syntax
                    // ${VAR:-default} or ${VAR:=default}
                    char *colon_pos = dl_strchr(var_name, ':');
                    if (colon_pos && *(colon_pos + 1) == '-')
                    {
                        *colon_pos = '\0';
                        char *default_value = colon_pos + 2;
                        result = dl_strjoin(result, default_value);
                    }
                    else
                    {
                        // No default value, expand to empty string (like bash)
                        // result = dl_strjoin(result, ""); // No need to join empty string
                    }
                }
            }
            else
            {
                result = dl_strjoin(result, "$");
            }
            
            if (var_name)
                free(var_name);
            
            // Skip to end of variable
            if (*var_end == '}')
                i = (var_end - str) + 1;
            else
                i = (var_end - str);
        }
        else
        {
            // Copy literal character
            char temp[2] = {str[i], '\0'};
            result = dl_strjoin(result, temp);
            i++;
        }
    }
    
    return (result);
}
