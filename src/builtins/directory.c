#include "cshell.h"

// Expand tilde in path (~, ~/path, ~username, ~username/path)
char *expand_tilde(const char *path)
{
    struct passwd *pw;
    char *home_dir;
    char *expanded;
    char *username;
    int username_len;
    
    if (!path || path[0] != '~')
        return (dl_strdup(path));
    
    // Case 1: Just ~ or ~/path (current user's home)
    if (path[1] == '\0' || path[1] == '/')
    {
        home_dir = get_environment_variable("HOME");
        if (!home_dir)
        {
            // Fallback to getpwuid for current user
            pw = getpwuid(getuid());
            if (pw && pw->pw_dir)
                home_dir = dl_strdup(pw->pw_dir);
            else
                return (dl_strdup(path));  // Can't expand, return as-is
        }
        
        if (path[1] == '\0')
        {
            // Just ~
            return (home_dir);
        }
        else
        {
            // ~/path
            expanded = dl_strjoin(home_dir, path + 1);
            free(home_dir);
            return (expanded);
        }
    }
    
    // Case 2: ~username or ~username/path
    // Find the end of username (until / or end of string)
    username_len = 1;
    while (path[username_len] && path[username_len] != '/')
        username_len++;
    
    // Extract username (skip the ~)
    username = dl_calloc(username_len, sizeof(char));
    if (!username)
        return (dl_strdup(path));
    
    dl_strncpy(username, path + 1, username_len - 1);
    username[username_len - 1] = '\0';
    
    // Look up user by name
    pw = getpwnam(username);
    free(username);
    
    if (!pw || !pw->pw_dir)
        return (dl_strdup(path));  // User not found, return as-is
    
    // Build expanded path
    if (path[username_len] == '\0')
    {
        // Just ~username
        return (dl_strdup(pw->pw_dir));
    }
    else
    {
        // ~username/path
        expanded = dl_strjoin(pw->pw_dir, path + username_len);
        return (expanded);
    }
}

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

    // Get target directory
    if (!args[1] || dl_strncmp(args[1], "~", 2) == 0)
    {
        // No argument or just "~" - go to HOME
        target_dir = get_environment_variable("HOME");
        if (!target_dir)
        {
            print_error("cd", "HOME not set");
            return (EXIT_FAILURE);
        }
    }
    else if (dl_strncmp(args[1], "-", 2) == 0)
    {
        // cd - : go to previous directory
        target_dir = get_environment_variable("OLDPWD");
        if (!target_dir)
        {
            print_error("cd", "OLDPWD not set");
            return (EXIT_FAILURE);
        }
    }
    else if (args[1][0] == '~')
    {
        // Tilde expansion: ~, ~/path, ~username, ~username/path
        target_dir = expand_tilde(args[1]);
        if (!target_dir)
        {
            print_error("cd", "tilde expansion failed");
            return (EXIT_FAILURE);
        }
    }
    else
    {
        // Regular path
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
    free(target_dir);
    
    return (EXIT_SUCCESS);
}
