#include "cshell.h"

// Parse a single command string into arguments array
char **parse_single_command(char *cmd_str)
{
    char **args;
    int count;
    int i;
    int start;
    int len;
    
    if (!cmd_str)
        return (NULL);
    
    // Count arguments (simple space-based splitting for now)
    count = 1;
    for (i = 0; cmd_str[i]; i++)
    {
        if (cmd_str[i] == ' ' && cmd_str[i + 1] && cmd_str[i + 1] != ' ')
            count++;
    }
    
    // Allocate args array
    args = (char **)dl_calloc(count + 1, sizeof(char *));
    if (!args)
        return (NULL);
    
    // Split by spaces
    start = 0;
    count = 0;
    i = 0;
    while (cmd_str[i])
    {
        if (cmd_str[i] == ' ')
        {
            if (i > start)
            {
                len = i - start;
                args[count] = (char *)dl_calloc(len + 1, sizeof(char));
                if (!args[count])
                {
                    free_string_array(args);
                    return (NULL);
                }
                dl_strncpy(args[count], cmd_str + start, len);
                args[count][len] = '\0';
                count++;
            }
            start = i + 1;
        }
        i++;
    }
    
    // Add the last argument
    if (i > start)
    {
        len = i - start;
        args[count] = (char *)dl_calloc(len + 1, sizeof(char));
        if (!args[count])
        {
            free_string_array(args);
            return (NULL);
        }
        dl_strncpy(args[count], cmd_str + start, len);
        args[count][len] = '\0';
        count++;
    }
    
    args[count] = NULL;
    return (args);
}

// Set up file descriptors for a command in the pipeline
void the_piper(int cmd_index)
{
    dl_putstr_fd("DEBUG: the_piper called for command ", 2);
    char *cmd_num = dl_itoa(cmd_index);
    dl_putstr_fd(cmd_num, 2);
    dl_putstr_fd("\n", 2);
    free(cmd_num);
    
    // Set up input file descriptor
    if (cmd_index == 0)
    {
        // First command: read from stdin
        g_data.original_stdin = dup(STDIN_FILENO);
        dl_putstr_fd("DEBUG: Command 0: stdin set to original stdin\n", 2);
    }
    else
    {
        // Other commands: read from previous pipe (pipe[2*i-2])
        g_data.original_stdin = g_data.pipes[2 * cmd_index - 2];
        dl_putstr_fd("DEBUG: Command ", 2);
        char *cmd_num = dl_itoa(cmd_index);
        dl_putstr_fd(cmd_num, 2);
        dl_putstr_fd(": stdin set to pipe[", 2);
        char *pipe_num = dl_itoa(2 * cmd_index - 2);
        dl_putstr_fd(pipe_num, 2);
        dl_putstr_fd("]\n", 2);
        free(pipe_num);
        free(cmd_num);
    }
    
    // Set up output file descriptor
    if (cmd_index == g_data.cmd_amount - 1)
    {
        // Last command: write to stdout
        g_data.original_stdout = dup(STDOUT_FILENO);
        dl_putstr_fd("DEBUG: Last command: stdout set to original stdout\n", 2);
    }
    else
    {
        // Other commands: write to next pipe (pipe[2*i+1])
        g_data.original_stdout = g_data.pipes[2 * cmd_index + 1];
        dl_putstr_fd("DEBUG: Command ", 2);
        char *cmd_num = dl_itoa(cmd_index);
        dl_putstr_fd(cmd_num, 2);
        dl_putstr_fd(": stdout set to pipe[", 2);
        char *pipe_num = dl_itoa(2 * cmd_index + 1);
        dl_putstr_fd(pipe_num, 2);
        dl_putstr_fd("]\n", 2);
        free(pipe_num);
        free(cmd_num);
    }
    
    // Apply the redirections
    if (dup2(g_data.original_stdin, STDIN_FILENO) == -1 ||
        dup2(g_data.original_stdout, STDOUT_FILENO) == -1)
    {
        dl_putstr_fd("DEBUG: dup2 failed\n", 2);
        print_error("pipe", "failed to redirect file descriptors");
        exit(1);
    }
    
    dl_putstr_fd("DEBUG: File descriptors redirected successfully\n", 2);
}

// Execute a single command in the pipeline
void the_kindergarden(int cmd_index, char *cmd_str)
{
    char **args;
    int ret;
    
    dl_putstr_fd("DEBUG: the_kindergarden called for command ", 2);
    char *cmd_num = dl_itoa(cmd_index);
    dl_putstr_fd(cmd_num, 2);
    dl_putstr_fd(": '", 2);
    dl_putstr_fd(cmd_str, 2);
    dl_putstr_fd("'\n", 2);
    free(cmd_num);
    
    // Set up file descriptors for this command
    the_piper(cmd_index);
    
    dl_putstr_fd("DEBUG: File descriptors set up\n", 2);
    
    // Close the original file descriptors after dup2
    close(g_data.original_stdin);
    close(g_data.original_stdout);
    
    // Close ALL pipes in child process
    // This ensures proper pipe communication
    if (g_data.pipe_amount > 0)
    {
        dl_putstr_fd("DEBUG: Closing all pipes in child process\n", 2);
        close_pipeline();
    }
    
    // Check for invalid file descriptors
    if (g_data.original_stdin == -1 || g_data.original_stdout == -1)
    {
        dl_putstr_fd("DEBUG: Invalid file descriptors\n", 2);
        exit(1);
    }
    
    // Parse the individual command
    args = parse_single_command(cmd_str);
    if (!args || !args[0])
    {
        dl_putstr_fd("DEBUG: Command parsing failed\n", 2);
        free_string_array(args);
        exit(1);
    }
    
    dl_putstr_fd("DEBUG: Command parsed successfully, args[0] = '", 2);
    dl_putstr_fd(args[0], 2);
    dl_putstr_fd("'\n", 2);
    
    // Check if it's a builtin
    if (is_builtin(args[0]))
    {
        dl_putstr_fd("DEBUG: Executing builtin command\n", 2);
        ret = execute_builtin(args);
        free_string_array(args);
        exit(ret);
    }
    else
    {
        dl_putstr_fd("DEBUG: Executing external command\n", 2);
        // Execute external command
        if (execvp(args[0], args) == -1)
        {
            dl_putstr_fd("DEBUG: execvp failed\n", 2);
            print_error(args[0], "execution failed");
            free_string_array(args);
            exit(127);
        }
    }
}

// Parse and prepare a single command for execution
int prepare_command(int cmd_index, char *cmd_str)
{
    char **args;
    
    (void)cmd_index; // Suppress unused parameter warning
    
    // Parse the command into arguments
    args = parse_single_command(cmd_str);
    if (!args || !args[0])
        return (1);
    
    // Check if it's a builtin
    if (is_builtin(args[0]))
    {
        // For builtins, we'll execute them in the_kindergarden
        free_string_array(args);
        return (0);
    }
    else
    {
        // For external commands, check if they exist
        if (!find_the_paths(args[0]))
        {
            print_error(args[0], "command not found");
            free_string_array(args);
            return (1);
        }
        free_string_array(args);
        return (0);
    }
}

// Execute all commands in the pipeline
void go_through_pipeline(char **commands)
{
    int i;
    
    dl_putstr_fd("DEBUG: Starting pipeline execution\n", 2);
    
    for (i = 0; i < g_data.cmd_amount; i++)
    {
        dl_putstr_fd("DEBUG: Processing command ", 2);
        char *cmd_num1 = dl_itoa(i);
        dl_putstr_fd(cmd_num1, 2);
        dl_putstr_fd(": '", 2);
        dl_putstr_fd(commands[i], 2);
        dl_putstr_fd("'\n", 2);
        free(cmd_num1);
        
        // Prepare the command (parse arguments, check if it exists)
        if (prepare_command(i, commands[i]) != 0)
        {
            dl_putstr_fd("DEBUG: Command preparation failed for command ", 2);
            char *cmd_num2 = dl_itoa(i);
            dl_putstr_fd(cmd_num2, 2);
            dl_putstr_fd("\n", 2);
            free(cmd_num2);
            // Command preparation failed, skip this command
            continue;
        }
        
        dl_putstr_fd("DEBUG: Forking process for command ", 2);
        char *cmd_num3 = dl_itoa(i);
        dl_putstr_fd(cmd_num3, 2);
        dl_putstr_fd("\n", 2);
        free(cmd_num3);
        
        // Fork child process for this command
        g_data.child_pids[i] = fork();
        
        if (g_data.child_pids[i] < 0)
        {
            dl_putstr_fd("DEBUG: Fork failed for command ", 2);
            char *cmd_num4 = dl_itoa(i);
            dl_putstr_fd(cmd_num4, 2);
            dl_putstr_fd("\n", 2);
            free(cmd_num4);
            print_error("pipe", "fork failed");
            exit(1);
        }
        
        if (g_data.child_pids[i] == 0)
        {
            dl_putstr_fd("DEBUG: Child process ", 2);
            char *cmd_num5 = dl_itoa(i);
            dl_putstr_fd(cmd_num5, 2);
            dl_putstr_fd(" starting\n", 2);
            free(cmd_num5);
            // Child process
            the_kindergarden(i, commands[i]);
        }
        
        dl_putstr_fd("DEBUG: Parent process forked command ", 2);
        char *cmd_num6 = dl_itoa(i);
        dl_putstr_fd(cmd_num6, 2);
        dl_putstr_fd("\n", 2);
        free(cmd_num6);
    }
    
    dl_putstr_fd("DEBUG: All commands forked, parent waiting for completion\n", 2);
}

// Handle pipe command execution
int handle_pipe_direct(char *cmd_str)
{
    char **commands;
    int i;
    int status;
    
    // Debug: Print the command string
    dl_putstr_fd("DEBUG: handle_pipe_direct called with: '", 2);
    dl_putstr_fd(cmd_str, 2);
    dl_putstr_fd("'\n", 2);
    
    // Parse commands by pipe separator
    commands = parse_pipe_commands(cmd_str);
    if (!commands)
    {
        dl_putstr_fd("DEBUG: parse_pipe_commands failed\n", 2);
        return (1);
    }
    
    // Count commands
    g_data.cmd_amount = 0;
    while (commands[g_data.cmd_amount])
        g_data.cmd_amount++;
    
    dl_putstr_fd("DEBUG: Found ", 2);
    char *cmd_count_str = dl_itoa(g_data.cmd_amount);
    dl_putstr_fd(cmd_count_str, 2);
    dl_putstr_fd(" commands\n", 2);
    free(cmd_count_str);
    
    if (g_data.cmd_amount <= 1)
    {
        dl_putstr_fd("DEBUG: Single command, executing normally\n", 2);
        // No pipes, execute single command
        char *single_cmd = dl_strdup(commands[0]);
        free_string_array(commands);
        if (!single_cmd)
            return (1);
        // Parse and execute the single command
        if (parse_input(single_cmd) != 0)
        {
            free(single_cmd);
            return (1);
        }
        free(single_cmd);
        return (execute_commands());
    }
    
    dl_putstr_fd("DEBUG: Multiple commands, setting up pipeline\n", 2);
    
    // Initialize pipes
    if (init_pipes() < 0)
    {
        dl_putstr_fd("DEBUG: init_pipes failed\n", 2);
        return (1);
    }
    
    // Execute pipeline
    go_through_pipeline(commands);
    
    dl_putstr_fd("DEBUG: Pipeline execution completed, closing pipeline\n", 2);
    
    // Close all pipes in parent process AFTER forking all children
    // This is the key: parent must close all pipes so children can communicate properly
    close_pipeline();
    
    // Wait for all child processes to complete
    i = 0;
    while (i < g_data.cmd_amount && g_data.child_pids[i] > 0)
    {
        dl_putstr_fd("DEBUG: Waiting for child process ", 2);
        char *cmd_num = dl_itoa(i);
        dl_putstr_fd(cmd_num, 2);
        dl_putstr_fd(" (PID: ", 2);
        char *pid_str = dl_itoa(g_data.child_pids[i]);
        dl_putstr_fd(pid_str, 2);
        dl_putstr_fd(")\n", 2);
        free(pid_str);
        free(cmd_num);
        
        if (waitpid(g_data.child_pids[i], &status, 0) > 0)
        {
            dl_putstr_fd("DEBUG: Child process ", 2);
            char *cmd_num = dl_itoa(i);
            dl_putstr_fd(cmd_num, 2);
            dl_putstr_fd(" completed\n", 2);
            free(cmd_num);
        }
        i++;
    }
    
    dl_putstr_fd("DEBUG: All child processes completed\n", 2);
    
    // Cleanup
    free(g_data.child_pids);
    g_data.child_pids = NULL;
    free_string_array(commands);
    
    return (status);
}

// Parse commands separated by pipes
char **parse_pipe_commands(char *cmd_str)
{
    char **commands;
    int i;
    int count;
    int start;
    int len;
    
    if (!cmd_str)
        return (NULL);
    
    // Count pipe separators
    count = 1;
    for (i = 0; cmd_str[i]; i++)
    {
        if (cmd_str[i] == '|')
            count++;
    }
    
    // Allocate commands array
    commands = (char **)dl_calloc(count + 1, sizeof(char *));
    if (!commands)
        return (NULL);
    
    // Split by pipes
    start = 0;
    count = 0;
    for (i = 0; cmd_str[i]; i++)
    {
        if (cmd_str[i] == '|')
        {
            len = i - start;
            commands[count] = (char *)dl_calloc(len + 1, sizeof(char));
            if (!commands[count])
            {
                free_string_array(commands);
                return (NULL);
            }
            dl_strncpy(commands[count], cmd_str + start, len);
            commands[count][len] = '\0';
            // Trim whitespace from the command
            char *trimmed = trim_whitespace(commands[count]);
            free(commands[count]);
            commands[count] = trimmed;
            count++;
            start = i + 1;
        }
    }
    
    // Add the last command
    len = dl_strlen(cmd_str) - start;
    commands[count] = (char *)dl_calloc(len + 1, sizeof(char));
    if (!commands[count])
    {
        free_string_array(commands);
        return (NULL);
    }
    dl_strncpy(commands[count], cmd_str + start, len);
    commands[count][len] = '\0';
    // Trim whitespace from the last command
    char *trimmed = trim_whitespace(commands[count]);
    free(commands[count]);
    commands[count] = trimmed;
    commands[count + 1] = NULL;
    
    return (commands);
}



// Find redirection node for a specific command
t_redi *find_redirection_node(int type)
{
    t_redi *current;
    
    current = g_data.redirections;
    while (current)
    {
        if (current->type == type)
            return (current);
        current = current->next;
    }
    
    return (NULL);
}
