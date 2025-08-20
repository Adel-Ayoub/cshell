#include "cshell.h"

int builtin_pwd(void)
{
    char *cwd;
    char *pwd_env;

    // First try to get PWD from environment
    pwd_env = get_environment_variable("PWD");
    if (pwd_env)
    {
        dl_putendl_fd(pwd_env, STDOUT_FILENO);
        free(pwd_env);
        return (EXIT_SUCCESS);
    }
    
    // Fallback to getcwd
    cwd = getcwd(NULL, 0);
    if (cwd)
    {
        dl_putendl_fd(cwd, STDOUT_FILENO);
        free(cwd);
        return (EXIT_SUCCESS);
    }
    
    print_error("pwd", "getcwd failed");
    return (EXIT_FAILURE);
}

int builtin_cd(char **args)
{
    char *target_dir;
    char *old_pwd;
    char *new_pwd;
    char *expanded_path;

    // Get target directory
    if (!args[1] || dl_strncmp(args[1], "~", 2) == 0)
    {
        target_dir = get_environment_variable("HOME");
        if (!target_dir)
        {
            print_error("cd", "HOME not set");
            return (EXIT_FAILURE);
        }
    }
    else if (dl_strncmp(args[1], "-", 2) == 0)
    {
        target_dir = get_environment_variable("OLDPWD");
        if (!target_dir)
        {
            print_error("cd", "OLDPWD not set");
            return (EXIT_FAILURE);
        }
    }
    else
    {
        // Handle tilde expansion
        if (args[1][0] == '~')
        {
            char *home = get_environment_variable("HOME");
            if (home)
            {
                if (args[1][1] == '/' || args[1][1] == '\0')
                {
                    // ~/path or just ~
                    expanded_path = dl_strjoin(home, args[1] + 1);
                    target_dir = expanded_path;
                }
                else
                {
                    // ~username (not implemented yet, treat as literal)
                    target_dir = dl_strdup(args[1]);
                }
                free(home);
            }
            else
            {
                print_error("cd", "HOME not set");
                return (EXIT_FAILURE);
            }
        }
        else
        {
            target_dir = dl_strdup(args[1]);
        }
    }
    
    // Get current directory before changing
    old_pwd = getcwd(NULL, 0);
    if (!old_pwd)
    {
        print_error("cd", "getcwd failed");
        free(target_dir);
        return (EXIT_FAILURE);
    }
    
    // Change directory
    if (chdir(target_dir) != 0)
    {
        print_error("cd", "No such file or directory");
        free(old_pwd);
        free(target_dir);
        return (EXIT_FAILURE);
    }
    
    // Update PWD and OLDPWD environment variables
    new_pwd = getcwd(NULL, 0);
    if (new_pwd)
    {
        // Update PWD
        if (set_environment_variable("PWD", new_pwd) != 0)
        {
            print_error("cd", "failed to update PWD");
        }
        
        // Update OLDPWD
        if (set_environment_variable("OLDPWD", old_pwd) != 0)
        {
            print_error("cd", "failed to update OLDPWD");
        }
        
        free(new_pwd);
    }
    
    // If cd - was used, print the new directory
    if (args[1] && dl_strncmp(args[1], "-", 2) == 0)
        dl_putendl_fd(target_dir, STDOUT_FILENO);
    
    // Cleanup
    free(old_pwd);
    if (target_dir != expanded_path) // Don't double-free if they're the same
        free(target_dir);
    if (expanded_path && target_dir == expanded_path)
        free(expanded_path);
    
    return (EXIT_SUCCESS);
}
