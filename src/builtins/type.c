#include "cshell.h"

int builtin_type(char **args)
{
    int i;
    char *path;
    
    if (!args[1])
    {
        print_error("type", "usage: type name [name ...]");
        return (EXIT_FAILURE);
    }
    
    i = 1;
    while (args[i])
    {
        // Check if it's a builtin
        if (is_builtin(args[i]))
        {
            dl_putstr_fd(args[i], STDOUT_FILENO);
            dl_putendl_fd(" is a shell builtin", STDOUT_FILENO);
        }
        else
        {
            // Check if it's an external command
            path = find_command_path(args[i]);
            if (path)
            {
                dl_putstr_fd(args[i], STDOUT_FILENO);
                dl_putstr_fd(" is ", STDOUT_FILENO);
                dl_putendl_fd(path, STDOUT_FILENO);
                free(path);
            }
            else
            {
                dl_putstr_fd(args[i], STDOUT_FILENO);
                dl_putendl_fd(": not found", STDOUT_FILENO);
            }
        }
        i++;
    }
    
    return (EXIT_SUCCESS);
}

char *find_command_path(char *command)
{
    char *path_env;
    char **paths;
    char *full_path;
    int i;
    
    if (!command)
        return (NULL);
    
    // If command contains a slash, treat as absolute/relative path
    if (dl_strchr(command, '/'))
    {
        if (access(command, F_OK) == 0)
            return (dl_strdup(command));
        return (NULL);
    }
    
    // Get PATH environment variable
    path_env = get_environment_variable("PATH");
    if (!path_env)
        return (NULL);
    
    // Split PATH by ':'
    paths = dl_split(path_env, ':');
    free(path_env);
    
    if (!paths)
        return (NULL);
    
    // Check each path
    i = 0;
    while (paths[i])
    {
        full_path = dl_strjoin(dl_strjoin(paths[i], "/"), command);
        if (full_path && access(full_path, F_OK) == 0)
        {
            free_string_array(paths);
            return (full_path);
        }
        if (full_path)
            free(full_path);
        i++;
    }
    
    free_string_array(paths);
    return (NULL);
}
