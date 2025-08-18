#include "cshell.h"

static char *builtin_names[] = {
    "echo",
    "cd",
    "pwd",
    "env",
    "export",
    "unset",
    "exit"
};

static int (*builtin_functions[])(char **) = {
    builtin_echo,
    builtin_cd,
    NULL,  // pwd takes no args
    NULL,  // env takes no args
    builtin_export,
    builtin_unset,
    builtin_exit
};

int is_builtin(char *command)
{
    int i;

    if (!command)
        return (0);
    
    i = 0;
    while (i < 7)
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
    while (i < 7)
    {
        if (dl_strncmp(args[0], builtin_names[i], dl_strlen(builtin_names[i]) + 1) == 0)
        {
            if (i == 2)  // pwd
                return (builtin_pwd());
            else if (i == 3)  // env
                return (builtin_env());
            else if (builtin_functions[i])
                return (builtin_functions[i](args));
        }
        i++;
    }
    return (EXIT_FAILURE);
}

int builtin_exit(char **args)
{
    int exit_code;

    if (args[1])
    {
        exit_code = dl_atoi(args[1]);
        if (exit_code == 0 && args[1][0] != '0')
        {
            print_error("exit", "numeric argument required");
            g_data.exit_status = 255;
            return (255);
        }
        g_data.exit_status = exit_code;
    }
    else
    {
        g_data.exit_status = EXIT_SUCCESS;
    }
    
    // Cleanup and exit
    cleanup_shell();
    exit(g_data.exit_status);
}
