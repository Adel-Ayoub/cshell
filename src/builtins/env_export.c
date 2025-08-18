#include "cshell.h"

int builtin_export(char **args)
{
    int i;
    char *equal_sign;
    char *name;
    char *value;

    if (!args[1])
    {
        // Display all exported variables
        display_exported_variables();
        return (EXIT_SUCCESS);
    }
    
    i = 1;
    while (args[i])
    {
        equal_sign = dl_strchr(args[i], '=');
        if (equal_sign)
        {
            // Extract name and value
            *equal_sign = '\0';
            name = args[i];
            value = equal_sign + 1;
            
            // Validate variable name
            if (!is_valid_variable_name(name))
            {
                print_error("export", "not a valid identifier");
                *equal_sign = '=';  // Restore original string
                i++;
                continue;
            }
            
            // Add or update environment variable
            if (add_environment_variable(name, value) != 0)
            {
                print_error("export", "failed to set variable");
                *equal_sign = '=';
                i++;
                continue;
            }
            
            *equal_sign = '=';  // Restore original string
        }
        else
        {
            // Just export existing variable
            if (!is_valid_variable_name(args[i]))
            {
                print_error("export", "not a valid identifier");
                i++;
                continue;
            }
            
            // Check if variable exists
            if (!get_environment_variable(args[i]))
            {
                // Add empty variable
                add_environment_variable(args[i], "");
            }
        }
        i++;
    }
    
    return (EXIT_SUCCESS);
}

void display_exported_variables(void)
{
    int i;
    char *equal_sign;
    char *name;
    char *value;

    i = 0;
    while (g_data.env[i])
    {
        equal_sign = dl_strchr(g_data.env[i], '=');
        if (equal_sign)
        {
            *equal_sign = '\0';
            name = g_data.env[i];
            value = equal_sign + 1;
            
            dl_putstr_fd("declare -x ", STDOUT_FILENO);
            dl_putstr_fd(name, STDOUT_FILENO);
            dl_putstr_fd("=\"", STDOUT_FILENO);
            dl_putstr_fd(value, STDOUT_FILENO);
            dl_putstr_fd("\"\n", STDOUT_FILENO);
            
            *equal_sign = '=';
        }
        i++;
    }
}

int is_valid_variable_name(char *name)
{
    int i;

    if (!name || !*name)
        return (0);
    
    // First character must be letter or underscore
    if (!dl_isalpha(*name) && *name != '_')
        return (0);
    
    i = 1;
    while (name[i])
    {
        if (!dl_isalnum(name[i]) && name[i] != '_')
            return (0);
        i++;
    }
    
    return (1);
}

int add_environment_variable(char *name, char *value)
{
    int i;
    char *new_var;
    char **new_env;

    if (!name)
        return (-1);
    
    // Create new variable string
    if (value)
        new_var = dl_strjoin(dl_strjoin(name, "="), value);
    else
        new_var = dl_strjoin(name, "=");
    
    if (!new_var)
        return (-1);
    
    // Check if variable already exists
    i = 0;
    while (g_data.env[i])
    {
        if (dl_strncmp(g_data.env[i], name, dl_strlen(name)) == 0 &&
            g_data.env[i][dl_strlen(name)] == '=')
        {
            // Update existing variable
            free(g_data.env[i]);
            g_data.env[i] = new_var;
            return (0);
        }
        i++;
    }
    
    // Add new variable
    new_env = (char **)dl_calloc(g_data.env_size + 2, sizeof(char *));
    if (!new_env)
    {
        free(new_var);
        return (-1);
    }
    
    // Copy existing variables
    i = 0;
    while (g_data.env[i])
    {
        new_env[i] = g_data.env[i];
        i++;
    }
    
    // Add new variable
    new_env[i] = new_var;
    new_env[i + 1] = NULL;
    
    // Replace old environment
    free(g_data.env);
    g_data.env = new_env;
    g_data.env_size++;
    
    return (0);
}
