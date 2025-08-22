#include "cshell.h"

// Set up file descriptors for a command in the pipeline
void the_piper(int cmd_index)
{
    t_redi *in_node;
    t_redi *out_node;
    
    // Find input and output redirections for this command
    in_node = find_redirection_node(0);  // Input redirection
    out_node = find_redirection_node(1); // Output redirection
    
    // Set up input file descriptor
    if (!in_node)
    {
        if (cmd_index == 0)
            g_data.original_stdin = dup(STDIN_FILENO);
        else
            g_data.original_stdin = g_data.pipes[cmd_index - 1][0];
    }
    else
        g_data.original_stdin = in_node->fd;
    
    // Set up output file descriptor
    if (!out_node)
    {
        if (cmd_index == g_data.cmd_amount - 1)
            g_data.original_stdout = dup(STDOUT_FILENO);
        else
            g_data.original_stdout = g_data.pipes[cmd_index][1];
    }
    else
        g_data.original_stdout = out_node->fd;
    
    // Apply the redirections
    if (dup2(g_data.original_stdin, STDIN_FILENO) == -1 ||
        dup2(g_data.original_stdout, STDOUT_FILENO) == -1)
    {
        print_error("pipe", "failed to redirect file descriptors");
        exit(1);
    }
}

// Execute a single command in the pipeline
void the_kindergarden(int cmd_index)
{
    // Set up file descriptors for this command
    the_piper(cmd_index);
    
    // Close unused pipe ends
    close_pipeline();
    
    // Execute the command
    if (execute_commands() != 0)
        exit(1);
    
    exit(0);
}

// Execute all commands in the pipeline
void go_through_pipeline(void)
{
    int i;
    
    for (i = 0; i < g_data.cmd_amount; i++)
    {
        // Fork child process for this command
        g_data.child_pids[i] = fork();
        
        if (g_data.child_pids[i] < 0)
        {
            print_error("pipe", "fork failed");
            exit(1);
        }
        
        if (g_data.child_pids[i] == 0)
        {
            // Child process
            the_kindergarden(i);
        }
        
        // Parent process continues to next command
        free_commands();
    }
}

// Handle pipe command execution (direct version to avoid recursion)
int handle_pipe_direct(char *cmd_str)
{
    char **commands;
    int i;
    int status;
    
    // Parse commands by pipe separator
    commands = parse_pipe_commands(cmd_str);
    if (!commands)
        return (1);
    
    // Count commands
    g_data.cmd_amount = 0;
    while (commands[g_data.cmd_amount])
        g_data.cmd_amount++;
    
    if (g_data.cmd_amount <= 1)
    {
        // No pipes, execute single command
        free_string_array(commands);
        // Parse and execute the single command
        if (parse_input(commands[0]) != 0)
            return (1);
        return (execute_commands());
    }
    
    // Allocate child process IDs array
    g_data.child_pids = (pid_t *)dl_calloc(g_data.cmd_amount, sizeof(pid_t));
    if (!g_data.child_pids)
    {
        free_string_array(commands);
        return (1);
    }
    
    // Initialize pipes
    if (init_pipes() != 0)
    {
        free_string_array(commands);
        free(g_data.child_pids);
        return (1);
    }
    
    // Execute pipeline
    go_through_pipeline();
    
    // Wait for all child processes to complete
    for (i = 0; i < g_data.cmd_amount; i++)
    {
        waitpid(g_data.child_pids[i], &status, 0);
        if (i == g_data.cmd_amount - 1)
            g_data.last_exit_status = WEXITSTATUS(status);
    }
    
    // Cleanup
    close_pipeline();
    free(g_data.child_pids);
    free_string_array(commands);
    
    return (g_data.last_exit_status);
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
