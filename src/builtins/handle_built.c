#include "cshell.h"

int work_built(char **args)
{
    if (!args || !args[0])
        return (EXIT_FAILURE);
    
    return (execute_builtin(args));
}

int is_built_in(char *command)
{
    return (is_builtin(command));
}

int find_the_paths(char *command)
{
    char *path_copy;
    char *dir;
    char *full_path;
    char *path_env;

    if (!command)
        return (0);
    
    // Check if command is absolute path
    if (command[0] == '/')
        return (access(command, X_OK) == 0);
    
    // Check if command is relative path (contains /)
    if (dl_strchr(command, '/'))
        return (access(command, X_OK) == 0);
    
    // Search in PATH
    path_env = get_environment_variable("PATH");
    if (!path_env)
        return (0);
    
    path_copy = dl_strdup(path_env);
    if (!path_copy)
        return (0);
    
    // First call to strtok
    dir = dl_strtok(path_copy, ":");
    while (dir)
    {
        full_path = dl_strjoin(dl_strjoin(dir, "/"), command);
        if (full_path && access(full_path, X_OK) == 0)
        {
            free(full_path);
            free(path_copy);
            return (1);
        }
        if (full_path)
            free(full_path);
        
        // Continue with subsequent calls
        dir = dl_strtok(NULL, ":");
    }
    
    free(path_copy);
    return (0);
}
