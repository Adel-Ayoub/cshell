#include "cshell.h"

// Forward declarations
int builtin_pwd_wrapper(char **args);
int builtin_env_wrapper(char **args);
int builtin_history(char **args);
int builtin_type(char **args);
int builtin_help(char **args);
int builtin_jobs(char **args);
int builtin_fg(char **args);
int builtin_bg(char **args);

static char *builtin_names[] = {
    "echo",
    "cd",
    "pwd",
    "env",
    "export",
    "unset",
    "exit",
    "history",
    "type",
    "help",
    "jobs",
    "fg",
    "bg"
};

static int (*builtin_functions[])(char **) = {
    builtin_echo,
    builtin_cd,
    builtin_pwd_wrapper,  // pwd wrapper
    builtin_env_wrapper,  // env wrapper
    builtin_export,
    builtin_unset,
    builtin_exit,
    builtin_history,
    builtin_type,
    builtin_help,
    builtin_jobs,
    builtin_fg,
    builtin_bg
};

int builtin_pwd_wrapper(char **args)
{
    (void)args; // Suppress unused parameter warning
    return (builtin_pwd());
}

int builtin_env_wrapper(char **args)
{
    (void)args; // Suppress unused parameter warning
    return (builtin_env());
}

int is_builtin(char *command)
{
    int i;

    if (!command)
        return (0);
    
    i = 0;
    while (i < 13)
    {
        if (dl_strncmp(command, builtin_names[i], dl_strlen(builtin_names[i]) + 1) == 0)
            return (1);
        i++;
    }
    return (0);
}

int execute_builtin(char **args)
{
    int i;

    if (!args || !args[0])
        return (EXIT_FAILURE);
    
    i = 0;
    while (i < 13)
    {
        if (dl_strncmp(args[0], builtin_names[i], dl_strlen(builtin_names[i]) + 1) == 0)
        {
            return (builtin_functions[i](args));
        }
        i++;
    }
    return (EXIT_FAILURE);
}

int builtin_exit(char **args)
{
    int exit_code;
    int i;

    if (args[1])
    {
        // Check if argument is numeric
        i = 0;
        if (args[1][0] == '+' || args[1][0] == '-')
            i++;
        
        while (args[1][i])
        {
            if (!dl_isdigit(args[1][i]))
            {
                print_error("exit", "numeric argument required");
                g_data.last_exit_status = 255;
                return (255);
            }
            i++;
        }
        
        exit_code = dl_atoi(args[1]);
        
        // Handle exit status range (0-255, wrap around)
        if (exit_code < 0)
            exit_code = 256 + (exit_code % 256);
        else
            exit_code = exit_code % 256;
        
        g_data.exit_status = exit_code;
        g_data.last_exit_status = exit_code;
    }
    else
    {
        g_data.exit_status = g_data.last_exit_status;
    }
    
    // Print exit message if interactive
    if (g_data.interactive_mode)
        dl_putendl_fd("exit", STDOUT_FILENO);
    
    // Cleanup and exit
    cleanup_shell();
    exit(g_data.exit_status);
}
