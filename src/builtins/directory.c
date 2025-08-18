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
    char *pwd_var;
    char *oldpwd_var;

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
        target_dir = dl_strdup(args[1]);
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
        pwd_var = dl_strjoin("PWD=", new_pwd);
        oldpwd_var = dl_strjoin("OLDPWD=", old_pwd);
        
        if (pwd_var && oldpwd_var)
        {
            add_environment_variable("PWD", new_pwd);
            add_environment_variable("OLDPWD", old_pwd);
        }
        
        if (pwd_var)
            free(pwd_var);
        if (oldpwd_var)
            free(oldpwd_var);
        free(new_pwd);
    }
    
    // If cd - was used, print the new directory
    if (args[1] && dl_strncmp(args[1], "-", 2) == 0)
        dl_putendl_fd(target_dir, STDOUT_FILENO);
    
    free(old_pwd);
    free(target_dir);
    return (EXIT_SUCCESS);
}
