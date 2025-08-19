#include "cshell.h"

int setup_shell(char **env)
{
    // Copy environment variables
    if (copy_environment(env) != 0)
        return (-1);
    
    // Setup terminal
    if (tcgetattr(STDIN_FILENO, &g_data.original_termios) != 0)
        return (-1);
    
    // Check if running interactively
    g_data.interactive_mode = isatty(STDIN_FILENO);
    
    // Setup signals
    setup_signals();
    
    // Initialize shell level
    update_shell_level();
    
    // Initialize exit status
    g_data.exit_status = EXIT_SUCCESS;
    g_data.last_exit_status = EXIT_SUCCESS;
    
    return (0);
}

void cleanup_shell(void)
{
    // Restore terminal settings
    if (g_data.interactive_mode)
        tcsetattr(STDIN_FILENO, TCSANOW, &g_data.original_termios);
    
    // Cleanup all allocated memory
    cleanup_all();
}

void shell_loop(void)
{
    char *input;
    
    while (1)
    {
        // Display prompt
        if (g_data.interactive_mode)
        {
            char *prompt = "cshell$ ";
            input = readline(prompt);
        }
        else
        {
            input = read_input();
        }
        
        // Check for EOF (Ctrl+D)
        if (input == NULL)
        {
            if (g_data.interactive_mode)
                dl_putendl_fd("exit", STDOUT_FILENO);
            break;
        }
        
        // Add to history if interactive
        if (g_data.interactive_mode && *input)
            add_history(input);
        
        // Process input
        if (parse_input(input) == 0)
        {
            if (execute_commands() != 0)
                g_data.exit_status = EXIT_FAILURE;
        }
        else
        {
            g_data.exit_status = EXIT_SYNTAX_ERROR;
        }
        
        // Cleanup input
        free(input);
        
        // Reset error state
        reset_error_state();
    }
}

char *read_input(void)
{
    char *line = NULL;
    size_t bufsize = 0;
    ssize_t line_size;

    line_size = getline(&line, &bufsize, stdin);
    if (line_size <= 0)
    {
        if (line)
            free(line);
        return (NULL);
    }
    
    // Remove newline
    if (line[line_size - 1] == '\n')
        line[line_size - 1] = '\0';
    
    return (line);
}

void cleanup_all(void)
{
    // Free environment
    free_string_array(g_data.env);
    
    // Free command data
    if (g_data.args_array)
        free_string_array(g_data.args_array);
    if (g_data.args_list)
        free_args_list(g_data.args_list);
    
    // Free redirections
    if (g_data.redirections)
        free_redirections(g_data.redirections);
    
    // Free pipes
    if (g_data.pipes)
    {
        for (int i = 0; i < g_data.pipe_count; i++)
            free(g_data.pipes[i]);
        free(g_data.pipes);
    }
    
    // Free child PIDs
    if (g_data.child_pids)
        free(g_data.child_pids);
    
            // Free wildcards
        if (g_data.wildcards)
            free_wildcards(g_data.wildcards);
        if (g_data.wild_help)
            free_wild_help(g_data.wild_help);
    
    // Free command tree
    if (g_data.command_tree)
        free_command_tree(g_data.command_tree);
    
    // Free here-docs
    if (g_data.here_docs)
        free_string_array(g_data.here_docs);
    
    // Reset global data
    dl_memset(&g_data, 0, sizeof(t_data));
}

void free_string_array(char **array)
{
    if (!array)
        return;
    
    for (int i = 0; array[i]; i++)
        free(array[i]);
    free(array);
}

void set_error(int type, const char *message)
{
    g_data.error_state.error_message = (char *)message;
    
    switch (type)
    {
        case 1:
            g_data.error_state.syntax_error = 1;
            break;
        case 2:
            g_data.error_state.quote_error = 1;
            break;
        case 3:
            g_data.error_state.redirection_error = 1;
            break;
        case 4:
            g_data.error_state.command_error = 1;
            break;
        default:
            break;
    }
}

void print_error(const char *command, const char *message)
{
    if (command)
        dl_putstr_fd((char *)command, STDERR_FILENO);
    if (message)
    {
        dl_putstr_fd(": ", STDERR_FILENO);
        dl_putstr_fd((char *)message, STDERR_FILENO);
    }
    dl_putchar_fd('\n', STDERR_FILENO);
}

void reset_error_state(void)
{
    dl_memset(&g_data.error_state, 0, sizeof(t_error));
}

int parse_logical_operators(void)
{
    // TODO: Implement logical operator parsing
    return (0);
}

int validate_syntax(void)
{
    // TODO: Implement syntax validation
    return (0);
}

void free_redirections(t_redi *list)
{
    (void)list;
    // TODO: Implement redirection cleanup
}

void free_wildcards(t_wild *list)
{
    (void)list;
    // TODO: Implement wildcard cleanup
}

void free_wild_help(t_whelp *list)
{
    t_whelp *current;
    t_whelp *next;

    if (!list)
        return;
    
    current = list;
    while (current)
    {
        next = current->next;
        if (current->str)
            free(current->str);
        free(current);
        current = next;
    }
}

void free_command_tree(t_cond *tree)
{
    (void)tree;
    // TODO: Implement command tree cleanup
}
