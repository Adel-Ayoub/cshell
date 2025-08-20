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
    int i;
    
    (void)back;  // Suppress unused parameter warning
    (void)var;   // Suppress unused parameter warning
    
    if (!str || !*str)
        return (create_condition_node(str, up));
    
    // Find the first semicolon (command separator)
    i = 0;
    while (str[i])
    {
        if (str[i] == ';')
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
    
    // Parse the command content
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
    
    // Execute current node if it's a command
    if (current->type == 0)
    {
        exec_node(current);
    }
    
    // Traverse next node (semicolon chain)
    if (current->next)
    {
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
    
    if (head->content)
        free(head->content);
    free(head);
}
