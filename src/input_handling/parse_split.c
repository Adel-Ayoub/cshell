#include "cshell.h"

int parse_input(char *input)
{
    if (!input)
        return (-1);
    
    // Store input line
    g_data.input_line = dl_strdup(input);
    if (!g_data.input_line)
        return (-1);
    
    // Tokenize input
    if (tokenize_input(input) != 0)
        return (-1);
    
    // Expand environment variables
    if (expand_environment_variables() != 0)
        return (-1);
    
    // Expand wildcards
    if (expand_wildcards() != 0)
        return (-1);
    
    // Parse redirections
    if (parse_redirections() != 0)
        return (-1);
    
    // Parse logical operators
    if (parse_logical_operators() != 0)
        return (-1);
    
    // Validate syntax
    if (validate_syntax() != 0)
        return (-1);
    
    return (0);
}

int tokenize_input(char *input)
{
    char **tokens;
    char *input_copy;
    char *token;
    int count;
    int i;

    if (!input)
        return (-1);
    
    // Count tokens
    count = 0;
    input_copy = dl_strdup(input);
    if (!input_copy)
        return (-1);
    
    token = dl_strtok(input_copy, " \t\n");
    while (token)
    {
        count++;
        token = dl_strtok(NULL, " \t\n");
    }
    
    // Allocate token array
    tokens = (char **)dl_calloc(count + 1, sizeof(char *));
    if (!tokens)
    {
        free(input_copy);
        return (-1);
    }
    
    // Fill token array
    free(input_copy);
    input_copy = dl_strdup(input);
    if (!input_copy)
    {
        free(tokens);
        return (-1);
    }
    
    i = 0;
    token = dl_strtok(input_copy, " \t\n");
    while (token && i < count)
    {
        tokens[i] = dl_strdup(token);
        if (!tokens[i])
        {
            free_string_array(tokens);
            free(input_copy);
            return (-1);
        }
        i++;
        token = dl_strtok(NULL, " \t\n");
    }
    
    // Convert to linked list
    g_data.args_list = create_args_list(tokens);
    if (!g_data.args_list)
    {
        free_string_array(tokens);
        free(input_copy);
        return (-1);
    }
    
    // Convert back to array for easier access
    g_data.args_array = convert_args_to_array(g_data.args_list);
    if (!g_data.args_array)
    {
        free_args_list(g_data.args_list);
        free_string_array(tokens);
        free(input_copy);
        return (-1);
    }
    
    g_data.args_count = count;
    
    free_string_array(tokens);
    free(input_copy);
    return (0);
}
