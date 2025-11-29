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
    
    // Allocate quoted_args tracking array
    g_data.quoted_args = (int *)dl_calloc(count + 1, sizeof(int));
    if (!g_data.quoted_args)
    {
        free_string_array(tokens);
        return (-1);
    }
    
    // Fill token array with proper quote handling
    if (fill_tokens_with_quotes(input, tokens) != 0)
    {
        free_string_array(tokens);
        free(g_data.quoted_args);
        g_data.quoted_args = NULL;
        return (-1);
    }
    
    // Convert to linked list
    g_data.args_list = create_args_list(tokens);
    if (!g_data.args_list)
    {
        free_string_array(tokens);
        free(g_data.quoted_args);
        g_data.quoted_args = NULL;
        return (-1);
    }
    
    // Convert back to array for easier access
    g_data.args_array = convert_args_to_array(g_data.args_list);
    if (!g_data.args_array)
    {
        free_args_list(g_data.args_list);
        free_string_array(tokens);
        free(g_data.quoted_args);
        g_data.quoted_args = NULL;
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
    int was_quoted;
    
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
        was_quoted = 0;
        
        // Process token - first pass to find end and check if quoted
        while (input[i] && (in_quotes || (input[i] != ' ' && input[i] != '\t' && input[i] != '\n')))
        {
            if (!in_quotes && (input[i] == '"' || input[i] == '\''))
            {
                in_quotes = 1;
                quote_char = input[i];
                was_quoted = 1;  // Mark that this token contains quotes
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
        
        // Extract token - strip quotes from the content
        tokens[j] = dl_calloc(i - token_start + 1, sizeof(char));
        if (!tokens[j])
            return (-1);
        
        k = 0;
        in_quotes = 0;
        quote_char = 0;
        for (int m = token_start; m < i; m++)
        {
            // Skip quote characters but include their content
            if (!in_quotes && (input[m] == '"' || input[m] == '\''))
            {
                in_quotes = 1;
                quote_char = input[m];
                // Don't add the opening quote to the token
            }
            else if (in_quotes && input[m] == quote_char)
            {
                in_quotes = 0;
                quote_char = 0;
                // Don't add the closing quote to the token
            }
            else
            {
                tokens[j][k++] = input[m];
            }
        }
        tokens[j][k] = '\0';
        
        // Track if this argument was quoted (to skip wildcard expansion)
        if (g_data.quoted_args)
            g_data.quoted_args[j] = was_quoted;
        
        j++;
    }
    
    tokens[j] = NULL;
    return (0);
}
