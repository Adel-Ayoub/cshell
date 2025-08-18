#include "cshell.h"

int test_access(char *path)
{
    if (!path)
        return (0);
    
    return (access(path, X_OK) == 0);
}

int test_paths(char *command)
{
    char *path_env;
    char *path_copy;
    char *full_path;

    if (!command)
        return (0);
    
    // Check if command is absolute path
    if (command[0] == '/')
        return (test_access(command));
    
    // Check if command is relative path (contains /)
    if (dl_strchr(command, '/'))
        return (test_access(command));
    
    // Search in PATH
    path_env = get_environment_variable("PATH");
    if (!path_env)
        return (0);
    
    path_copy = dl_strdup(path_env);
    if (!path_copy)
    {
        free(path_env);
        return (0);
    }
    
    path_copy = dl_strtok(path_copy, ":");
    while (path_copy)
    {
        full_path = dl_strjoin(dl_strjoin(path_copy, "/"), command);
        if (full_path && test_access(full_path))
        {
            free(full_path);
            free(path_copy);
            free(path_env);
            return (1);
        }
        if (full_path)
            free(full_path);
        path_copy = dl_strtok(NULL, ":");
    }
    
    free(path_copy);
    free(path_env);
    return (0);
}

int find_execs(char *command)
{
    return (test_paths(command));
}

int arguing(char **args)
{
    int i;

    if (!args)
        return (0);
    
    i = 0;
    while (args[i])
        i++;
    
    return (i);
}
