#include "cshell.h"

// Create a condition node (command node)
t_trinary *create_condition_node(char *str, t_trinary *up)
{
    t_trinary *new;
    
    new = dl_calloc(1, sizeof(t_trinary));
    if (!new)
        return (NULL);
    
    new->type = 0;  // Command type
    new->content = str;
    new->up = up;
    new->first_cond = NULL;
    new->sec_cond = NULL;
    new->back = NULL;
    new->next = NULL;
    
    return (new);
}

// Create a token node (for operators)
t_trinary *create_tokenode(t_error help, char *str, t_trinary *back, t_trinary *up)
{
    t_trinary *new;
    
    (void)help;  // Suppress unused parameter warning
    
    new = dl_calloc(1, sizeof(t_trinary));
    if (!new)
        return (NULL);
    
    // For now, just create a command node
    // TODO: Implement proper operator detection
    new->type = 0;  // Command type
    
    new->content = str;
    new->up = up;
    new->back = back;
    new->first_cond = NULL;
    new->sec_cond = NULL;
    new->next = NULL;
    
    return (new);
}

// Create level in the trinary tree
t_trinary *create_level(char *str, t_trinary *back, t_trinary *up, int var)
{
    t_trinary *new;
    int operator_type;
    char *left, *right;
    
    (void)back;  // Suppress unused parameter warning
    (void)var;   // Suppress unused parameter warning
    
    if (!str || !*str)
        return (create_condition_node(str, up));
    
    // Check for logical operators first (they have higher precedence than semicolons)
    if (has_logical_operators(str) && parse_complex_expression(str, &left, &right, &operator_type))
    {
        // Create operator node
        new = dl_calloc(1, sizeof(t_trinary));
        if (!new)
        {
            cleanup_logical_expression(&left, &right);
            return (NULL);
        }
        
        new->type = operator_type;  // 1 for AND, 2 for OR
        new->ret = -1;
        new->content = NULL;  // Operator nodes don't have content
        new->up = up;
        new->back = back;
        new->next = NULL;
        
        // Create left child (first condition)
        new->first_cond = create_level(left, NULL, new, 0);
        if (!new->first_cond)
        {
            // Free the strings since they weren't assigned
            if (left) free(left);
            if (right) free(right);
            free(new);
            return (NULL);
        }
        
        // Create right child (second condition) - only for && and ||
        if (right)
        {
            new->sec_cond = create_level(right, new, up, 0);
            if (!new->sec_cond)
            {
                // Note: left is now owned by first_cond, don't free it here
                // Only free right since it failed to be assigned
                free(right);
                free(new);
                return (NULL);
            }
        }
        else
        {
            new->sec_cond = NULL;  // Background jobs don't have a second condition
        }
        
        // Don't clean up the strings - they are now owned by the tree nodes
        // Set pointers to NULL to prevent double-free
        left = NULL;
        right = NULL;
        

        
        return (new);
    }
    
    // If no logical operators, check for semicolons and background operators
    int i = 0;
    while (str[i])
    {
        if (str[i] == ';' || (str[i] == '&' && (i == 0 || str[i - 1] != '&')))
            break;
        i++;
    }
    
    if (str[i] == ';')
    {
        // Create command node for first part (before semicolon)
        char *first_cmd = dl_calloc(i + 1, sizeof(char));
        if (!first_cmd)
            return (NULL);
        dl_strncpy(first_cmd, str, i);
        first_cmd[i] = '\0';
        
        new = create_condition_node(first_cmd, up);
        if (!new)
        {
            free(first_cmd);
            return (NULL);
        }
        
        // Create next node for remaining commands (after semicolon)
        char *remaining = dl_strdup(str + i + 1);
        if (remaining)
        {
            new->next = create_level(remaining, new, up, 0);
            // Note: remaining memory is now owned by the next node, don't free it here
        }
        
        return (new);
    }
    else if (str[i] == '&' && (i == 0 || str[i - 1] != '&'))
    {
        // Handle background operator as command separator
        // Create command node for first part (before &)
        char *first_cmd = dl_calloc(i + 1, sizeof(char));
        if (!first_cmd)
            return (NULL);
        dl_strncpy(first_cmd, str, i);
        first_cmd[i] = '\0';
        
        // Create background node
        new = create_condition_node(first_cmd, up);
        if (!new)
        {
            free(first_cmd);
            return (NULL);
        }
        new->type = 3; // BACKGROUND type
        
        // Create next node for remaining commands (after &)
        char *remaining = dl_strdup(str + i + 1);
        if (remaining)
        {
            new->next = create_level(remaining, new, up, 0);
            // Note: remaining memory is now owned by the next node, don't free it here
        }
        
        return (new);
    }
    else
    {
        // No semicolon, just a command
        return (create_condition_node(str, up));
    }
}

// Execute a single node
void exec_node(t_trinary *current)
{
    if (!current || !current->content)
        return;
    
    // Check if this is a background job (parent node is background type)
    int is_background = (current->up && current->up->type == 3);
    
    if (is_background)
    {
        // Execute command in background using fork
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process - execute the command
            if (dl_strchr(current->content, '|'))
            {
                // Handle pipe command directly
                exit(handle_pipe_direct(current->content));
            }
            else
            {
                // Parse and execute non-pipe command
                if (parse_input(current->content) == 0)
                    exit(execute_commands());
                else
                    exit(EXIT_FAILURE);
            }
        }
        else if (pid > 0)
        {
            // Parent process - add to background jobs and continue
            current->ret = 0;  // Background job started successfully
            
            // Add to background jobs list
            int job_id = add_background_job(pid, current->content);
            if (job_id >= 0)
            {
                dl_putstr_fd("[", 1);
                char *job_str = dl_itoa(job_id + 1);
                dl_putstr_fd(job_str, 1);
                free(job_str);
                dl_putstr_fd("] ", 1);
                char *pid_str = dl_itoa(pid);
                dl_putstr_fd(pid_str, 1);
                free(pid_str);
                dl_putstr_fd("\n", 1);
            }
        }
        else
        {
            // Fork failed
            current->ret = EXIT_FAILURE;
        }
        return;
    }
    
    // Regular foreground execution
    if (dl_strchr(current->content, '|'))
    {
        // Handle pipe command directly
        current->ret = handle_pipe_direct(current->content);
        return;
    }
    
    // Parse the command content (non-pipe command)
    if (parse_input(current->content) == 0)
    {
        // Execute the parsed command
        current->ret = execute_commands();
    }
    else
    {
        current->ret = EXIT_FAILURE;
    }
}

// Traverse and execute the trinary tree
void traveler(t_trinary *current)
{
    if (!current)
        return;
    
    // Execute first condition if this is a logical operator node
    if (current->first_cond)
    {
        traveler(current->first_cond);
        // Propagate the return value from first condition
        current->ret = current->first_cond->ret;
    }
    
    // Execute current node if it's a command
    if (current->type == 0)
    {
        exec_node(current);
    }
    
    // Execute second condition based on logical operator with short-circuit evaluation
    if (current->type == 1)  // AND operator (&&)
    {
        if (current->ret == 0)  // First condition succeeded, execute second
        {
            if (current->sec_cond)
            {
                traveler(current->sec_cond);
                // Update current node's return value with second condition's result
                current->ret = current->sec_cond->ret;
            }
        }
        // If first condition failed, don't execute second (short-circuit)
    }
    else if (current->type == 2)  // OR operator (||)
    {
        if (current->ret != 0)  // First condition failed, execute second
        {
            if (current->sec_cond)
            {
                traveler(current->sec_cond);
                // Update current node's return value with second condition's result
                current->ret = current->sec_cond->ret;
            }
        }
        // If first condition succeeded, don't execute second (short-circuit)
    }
    else if (current->type == 3)  // Background operator (&)
    {
        // For background jobs, we execute the command in the background
        // The return value should be 0 (success) for the shell
        current->ret = 0;
        
        // Add to background jobs list (if job control is implemented)
        // For now, we'll just note that this was a background job
        // TODO: Implement proper background job management
        
        // Ensure the return value is propagated to parent nodes
        if (current->up)
            current->up->ret = current->ret;
    }
    
    // Traverse next node (semicolon chain)
    if (current->next)
    {
        // Pass the return value to the next node
        if (current->next)
            current->next->ret = current->ret;
        traveler(current->next);
    }
}

// Go through the input and create the trinary tree
int go_through_list(void)
{
    if (!g_data.input_line)
        return (0);
    
    // Create the trinary tree
    g_data.trinary_tree = create_level(g_data.input_line, NULL, NULL, 1);
    
    return (g_data.trinary_tree != NULL);
}

// Clean up the trinary tree
void empty_tree(t_trinary *head)
{
    if (!head)
        return;
    
    empty_tree(head->first_cond);
    empty_tree(head->next);
    
    // Only free content if it's not the original input line
    if (head->content && head->content != g_data.input_line)
        free(head->content);
    free(head);
}
