#include "cshell.h"

int builtin_echo(char **args)
{
    int i;
    int newline;
    int in_quotes;
    char quote_char;

    if (!args[1])
    {
        dl_putchar_fd('\n', STDOUT_FILENO);
        return (EXIT_SUCCESS);
    }
    
    // Check for -n flag
    newline = 1;
    i = 1;
    while (args[i] && dl_strncmp(args[i], "-n", 3) == 0)
    {
        newline = 0;
        i++;
    }
    
    // Print arguments
    while (args[i])
    {
        in_quotes = 0;
        quote_char = 0;
        
        // Handle quotes
        if (args[i][0] == '"' || args[i][0] == '\'')
        {
            quote_char = args[i][0];
            in_quotes = 1;
            dl_putstr_fd(args[i] + 1, STDOUT_FILENO);
        }
        else if (args[i][dl_strlen(args[i]) - 1] == quote_char)
        {
            in_quotes = 0;
            // Remove closing quote
            char temp = args[i][dl_strlen(args[i]) - 1];
            args[i][dl_strlen(args[i]) - 1] = '\0';
            dl_putstr_fd(args[i], STDOUT_FILENO);
            args[i][dl_strlen(args[i])] = temp;
        }
        else
        {
            dl_putstr_fd(args[i], STDOUT_FILENO);
        }
        
        // Add space between arguments (except for the last one)
        if (args[i + 1])
            dl_putchar_fd(' ', STDOUT_FILENO);
        
        i++;
    }
    
    // Add newline if not suppressed
    if (newline)
        dl_putchar_fd('\n', STDOUT_FILENO);
    
    return (EXIT_SUCCESS);
}

int change_env_variable(char *name, char *value)
{
    int i;
    char *new_var;

    if (!name || !g_data.env)
        return (-1);
    
    // Create new variable string
    if (value)
        new_var = dl_strjoin(dl_strjoin(name, "="), value);
    else
        new_var = dl_strjoin(name, "=");
    
    if (!new_var)
        return (-1);
    
    // Find and update existing variable
    i = 0;
    while (g_data.env[i])
    {
        if (dl_strncmp(g_data.env[i], name, dl_strlen(name)) == 0 &&
            g_data.env[i][dl_strlen(name)] == '=')
        {
            free(g_data.env[i]);
            g_data.env[i] = new_var;
            return (0);
        }
        i++;
    }
    
    // Variable not found, add it
    free(new_var);
    return (add_environment_variable(name, value));
}

int find_sign(char *str)
{
    int i;

    if (!str)
        return (-1);
    
    i = 0;
    while (str[i])
    {
        if (str[i] == '=')
            return (i);
        i++;
    }
    
    return (-1);
}
