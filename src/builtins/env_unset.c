#include "cshell.h"

int builtin_unset(char **args)
{
    int i;
    int result;

    if (!args[1])
    {
        // No arguments provided
        return (EXIT_SUCCESS);
    }
    
    result = EXIT_SUCCESS;
    i = 1;
    while (args[i])
    {
        if (!is_valid_variable_name(args[i]))
        {
            print_error("unset", "not a valid identifier");
            result = EXIT_FAILURE;
        }
        else
        {
            if (remove_environment_variable(args[i]) != 0)
            {
                print_error("unset", "failed to unset variable");
                result = EXIT_FAILURE;
            }
        }
        i++;
    }
    
    return (result);
}

int remove_environment_variable(char *name)
{
    int i;
    int j;
    char **new_env;

    if (!name || !g_data.env)
        return (-1);
    
    // Find variable to remove
    i = 0;
    while (g_data.env[i])
    {
        if (dl_strncmp(g_data.env[i], name, dl_strlen(name)) == 0 &&
            g_data.env[i][dl_strlen(name)] == '=')
        {
            // Create new environment array
            new_env = (char **)dl_calloc(g_data.env_size, sizeof(char *));
            if (!new_env)
                return (-1);
            
            // Copy variables before the one to remove
            j = 0;
            while (j < i)
            {
                new_env[j] = g_data.env[j];
                j++;
            }
            
            // Copy variables after the one to remove
            while (g_data.env[j + 1])
            {
                new_env[j] = g_data.env[j + 1];
                j++;
            }
            
            // Free the removed variable
            free(g_data.env[i]);
            
            // Replace old environment
            free(g_data.env);
            g_data.env = new_env;
            g_data.env_size--;
            
            return (0);
        }
        i++;
    }
    
    // Variable not found
    return (0);
}
