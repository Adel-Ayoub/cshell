#include "cshell.h"

int parse_input(char *input)
{
    if (!input)
        return (-1);
    
    // Use the input parameter directly for tokenization, don't modify g_data.input_line
    if (tokenize_input(input) != 0)
        return (-1);
    
    // Expand environment variables
    if (expand_environment_variables() != 0)
        return (-1);
    
    // Expand wildcards
    if (expand_wildcards() != 0)
        return (-1);
    
    // Note: Pipe handling is deferred to execution phase through trinary tree
    
    // Check if there are redirections in the input before parsing
    if (dl_strchr(input, '<') || dl_strchr(input, '>'))
    {
        // Parse redirections only if they exist
        if (parse_redirections() != 0)
            return (-1);
        
        // Clean up redirection arguments from args array
        if (cleanup_redirection_args() != 0)
            return (-1);
    }
    
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
    int count;

    if (!input)
        return (-1);
    
    // Count tokens with proper quote handling
    count = count_tokens_with_quotes(input);
    
    // Allocate token array
    tokens = (char **)dl_calloc(count + 1, sizeof(char *));
    if (!tokens)
        return (-1);
    
    // Fill token array with proper quote handling
    if (fill_tokens_with_quotes(input, tokens) != 0)
    {
        free_string_array(tokens);
        return (-1);
    }
    
    // Convert to linked list
    g_data.args_list = create_args_list(tokens);
    if (!g_data.args_list)
    {
        free_string_array(tokens);
        return (-1);
    }
    
    // Convert back to array for easier access
    g_data.args_array = convert_args_to_array(g_data.args_list);
    if (!g_data.args_array)
    {
        free_args_list(g_data.args_list);
        free_string_array(tokens);
        return (-1);
    }
    
    g_data.args_count = count;
    
    free_string_array(tokens);
    return (0);
}

int count_tokens_with_quotes(char *input)
{
    int count;
    int i;
    int in_quotes;
    char quote_char;
    
    if (!input)
        return (0);
    
    count = 0;
    i = 0;
    in_quotes = 0;
    quote_char = 0;
    
    while (input[i])
    {
        // Skip whitespace
        while (input[i] && (input[i] == ' ' || input[i] == '\t' || input[i] == '\n'))
            i++;
        
        if (!input[i])
            break;
        
        // Start of token
        count++;
        
        // Process token
        while (input[i] && (in_quotes || (input[i] != ' ' && input[i] != '\t' && input[i] != '\n')))
        {
            if (!in_quotes && (input[i] == '"' || input[i] == '\''))
            {
                in_quotes = 1;
                quote_char = input[i];
                i++;
            }
            else if (in_quotes && input[i] == quote_char)
            {
                in_quotes = 0;
                quote_char = 0;
                i++;
            }
            else
            {
                i++;
            }
        }
    }
    
    return (count);
}

int fill_tokens_with_quotes(char *input, char **tokens)
{
    int i, j, k;
    int in_quotes;
    char quote_char;
    int token_start;
    
    if (!input || !tokens)
        return (-1);
    
    i = 0;
    j = 0;
    in_quotes = 0;
    quote_char = 0;
    
    while (input[i])
    {
        // Skip whitespace
        while (input[i] && (input[i] == ' ' || input[i] == '\t' || input[i] == '\n'))
            i++;
        
        if (!input[i])
            break;
        
        // Start of token
        token_start = i;
        
        // Process token
        while (input[i] && (in_quotes || (input[i] != ' ' && input[i] != '\t' && input[i] != '\n')))
        {
            if (!in_quotes && (input[i] == '"' || input[i] == '\''))
            {
                in_quotes = 1;
                quote_char = input[i];
                i++;
            }
            else if (in_quotes && input[i] == quote_char)
            {
                in_quotes = 0;
                quote_char = 0;
                i++;
            }
            else
            {
                i++;
            }
        }
        
        // Extract token (without quotes)
        tokens[j] = dl_calloc(i - token_start + 1, sizeof(char));
        if (!tokens[j])
            return (-1);
        
        k = 0;
        for (int m = token_start; m < i; m++)
        {
            tokens[j][k++] = input[m];
        }
        tokens[j][k] = '\0';
        
        j++;
    }
    
    tokens[j] = NULL;
    return (0);
}
