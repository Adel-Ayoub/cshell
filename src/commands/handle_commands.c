#include "cshell.h"

int execute_commands(void)
{
    int ret;
    
    if (!g_data.args_array || !g_data.args_array[0])
        return (EXIT_SUCCESS);
    
    // Process wildcards before execution
    if (expand_wildcards() != 0)
        return (EXIT_FAILURE);
    
    // Setup redirections before execution (only if there are redirections)
    if (g_data.redirections)
    {
        if (setup_redirections() != 0)
            return (EXIT_FAILURE);
        
        // Apply redirections to file descriptors
        if (apply_redirections() != 0)
        {
            cleanup_redirections();
            return (EXIT_FAILURE);
        }
    }
    
    // Check if it's a builtin
    if (is_builtin(g_data.args_array[0]))
    {
        ret = execute_builtin(g_data.args_array);
    }
    else
    {
        ret = execute_external(g_data.args_array);
    }
    
    // Restore file descriptors and cleanup redirections (only if there were redirections)
    if (g_data.redirections)
    {
        restore_redirections();
        cleanup_redirections();
    }
    
    return (ret);
}

int execute_external(char **args)
{
    pid_t pid;
    int status;

    if (!args || !args[0])
        return (EXIT_FAILURE);
    
    // Check if command exists (allow absolute/relative paths)
    if (args[0][0] != '/' && args[0][0] != '.' && !find_the_paths(args[0]))
    {
        print_error(args[0], "command not found");
        return (127); // Standard shell exit code for command not found
    }
    
    // Block signals during fork
    block_signals();
    
    // Fork process
    pid = fork();
    if (pid == 0)
    {
        // Child process
        unblock_signals();
        handle_child_signals();
        
        // Execute command
        if (execvp(args[0], args) == -1)
        {
            print_error(args[0], "execution failed");
            exit(127); // Standard exit code for exec failure
        }
    }
    else if (pid < 0)
    {
        // Fork failed
        unblock_signals();
        print_error("cshell", "fork failed");
        return (EXIT_FAILURE);
    }
    else
    {
        // Parent process
        unblock_signals();
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status))
            g_data.last_exit_status = WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
            g_data.last_exit_status = 128 + WTERMSIG(status);
    }
    

    
    return (g_data.last_exit_status);
}

int find_executable(char *command)
{
    return (find_the_paths(command));
}
