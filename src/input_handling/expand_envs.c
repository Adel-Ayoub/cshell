#include "cshell.h"

// Execute a command and return its output (for command substitution)
char *execute_subshell(const char *command)
{
    int pipe_fds[2];
    pid_t pid;
    char *output;
    char buffer[1024];
    ssize_t bytes_read;
    size_t total_size;
    size_t output_capacity;
    
    if (!command || !*command)
        return (dl_strdup(""));
    
    // Create pipe for capturing output
    if (pipe(pipe_fds) == -1)
        return (dl_strdup(""));
    
    pid = fork();
    if (pid == -1)
    {
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return (dl_strdup(""));
    }
    
    if (pid == 0)
    {
        // Child process
        close(pipe_fds[0]);  // Close read end
        dup2(pipe_fds[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(pipe_fds[1]);
        
        // Execute the command using /bin/sh for proper parsing
        execl("/bin/sh", "sh", "-c", command, NULL);
        exit(127);  // If execl fails
    }
    
    // Parent process
    close(pipe_fds[1]);  // Close write end
    
    // Read output from pipe
    output_capacity = 1024;
    output = dl_calloc(output_capacity, sizeof(char));
    if (!output)
    {
        close(pipe_fds[0]);
        waitpid(pid, NULL, 0);
        return (dl_strdup(""));
    }
    
    total_size = 0;
    while ((bytes_read = read(pipe_fds[0], buffer, sizeof(buffer) - 1)) > 0)
    {
        // Resize if needed
        if (total_size + bytes_read >= output_capacity)
        {
            output_capacity *= 2;
            char *new_output = dl_calloc(output_capacity, sizeof(char));
            if (!new_output)
            {
                free(output);
                close(pipe_fds[0]);
                waitpid(pid, NULL, 0);
                return (dl_strdup(""));
            }
            dl_memcpy(new_output, output, total_size);
            free(output);
            output = new_output;
        }
        
        dl_memcpy(output + total_size, buffer, bytes_read);
        total_size += bytes_read;
    }
    output[total_size] = '\0';
    
    close(pipe_fds[0]);
    waitpid(pid, NULL, 0);
    
    // Remove trailing newline(s)
    while (total_size > 0 && (output[total_size - 1] == '\n' || output[total_size - 1] == '\r'))
    {
        output[total_size - 1] = '\0';
        total_size--;
    }
    
    return (output);
}

// Expand command substitution $(command) in a string
char *expand_command_substitution(char *str)
{
    char *result;
    char *cmd_start;
    char *cmd_end;
    char *command;
    char *cmd_output;
    int i;
    int paren_depth;
    
    if (!str)
        return (NULL);
    
    // Check if string contains command substitution
    if (strstr(str, "$(") == NULL)
        return (dl_strdup(str));
    
    result = dl_calloc(1, 1);  // Start with empty string
    if (!result)
        return (NULL);
    
    i = 0;
    while (str[i])
    {
        if (str[i] == '$' && str[i + 1] == '(')
        {
            // Find matching closing parenthesis (handle nesting)
            cmd_start = str + i + 2;
            paren_depth = 1;
            cmd_end = cmd_start;
            
            while (*cmd_end && paren_depth > 0)
            {
                if (*cmd_end == '(')
                    paren_depth++;
                else if (*cmd_end == ')')
                    paren_depth--;
                if (paren_depth > 0)
                    cmd_end++;
            }
            
            if (paren_depth != 0)
            {
                // Unmatched parenthesis, treat as literal
                char temp[3] = {'$', '(', '\0'};
                char *new_result = dl_strjoin(result, temp);
                free(result);
                result = new_result;
                i += 2;
                continue;
            }
            
            // Extract and execute command
            int cmd_len = cmd_end - cmd_start;
            command = dl_calloc(cmd_len + 1, sizeof(char));
            if (!command)
            {
                free(result);
                return (NULL);
            }
            dl_strncpy(command, cmd_start, cmd_len);
            command[cmd_len] = '\0';
            
            // Execute command and get output
            cmd_output = execute_subshell(command);
            free(command);
            
            if (cmd_output)
            {
                char *new_result = dl_strjoin(result, cmd_output);
                free(result);
                free(cmd_output);
                result = new_result;
            }
            
            // Skip past the closing parenthesis
            i = (cmd_end - str) + 1;
        }
        else
        {
            // Copy literal character
            char temp[2] = {str[i], '\0'};
            char *new_result = dl_strjoin(result, temp);
            free(result);
            result = new_result;
            i++;
        }
    }
    
    return (result);
}

int expand_environment_variables(void)
{
    // Expand command substitution and environment variables in command arguments
    if (!g_data.args_array)
        return (0);
    
    return (expand_environment_array(g_data.args_array));
}

// Expand environment variables in an array of strings
int expand_environment_array(char **array)
{
    if (!array)
        return (0);
    
    for (int i = 0; array[i]; i++)
    {
        if (array[i] && dl_strchr(array[i], '$') != NULL)
        {
            // First expand command substitution $(...)
            char *cmd_expanded = expand_command_substitution(array[i]);
            if (cmd_expanded)
            {
                free(array[i]);
                array[i] = cmd_expanded;
            }
            
            // Then expand environment variables $VAR
            if (dl_strchr(array[i], '$') != NULL)
            {
                char *env_expanded = expand_environment_string(array[i]);
                if (env_expanded)
                {
                    free(array[i]);
                    array[i] = env_expanded;
                }
            }
        }
    }
    
    return (0);
}

// Expand environment variables in a single argument
char *expand_argument(char *arg)
{
    if (!arg)
        return (NULL);
    
    // Check if argument contains environment variables
    if (dl_strchr(arg, '$') == NULL)
        return (dl_strdup(arg));
    
    return (expand_environment_string(arg));
}

char *expand_environment_string(char *str)
{
    char *result;
    char *var_start;
    char *var_end;
    char *var_name;
    char *var_value;
    int i;
    
    if (!str)
        return (NULL);
    
    // Check if string contains any environment variables
    if (dl_strchr(str, '$') == NULL)
        return (dl_strdup(str));
    
    result = dl_calloc(1, 1); // Start with empty string
    if (!result)
        return (NULL);
    
    i = 0;
    while (str[i])
    {
        if (str[i] == '$' && (i == 0 || str[i - 1] != '\\'))
        {
            // Find variable name
            var_start = str + i + 1;
            if (*var_start == '{')
            {
                var_start++;
                var_end = dl_strchr(var_start, '}');
                if (!var_end)
                {
                    // Invalid syntax, treat as literal
                    result = dl_strjoin(result, "$");
                    i++;
                    continue;
                }
                *var_end = '\0';
                var_name = dl_strdup(var_start);
                *var_end = '}';
            }
            else
            {
                var_end = var_start;
                while (*var_end && (dl_isalnum(*var_end) || *var_end == '_'))
                    var_end++;
                var_name = dl_strdup(var_start);
                if (var_end != var_start)
                    var_name[var_end - var_start] = '\0';
            }
            
            // Get variable value
            if (var_name && *var_name)
            {
                var_value = get_environment_variable(var_name);
                if (var_value)
                {
                    result = dl_strjoin(result, var_value);
                    free(var_value);
                }
                else
                {
                    // Variable not found - check for default value syntax
                    // ${VAR:-default} or ${VAR:=default}
                    char *colon_pos = dl_strchr(var_name, ':');
                    if (colon_pos && *(colon_pos + 1) == '-')
                    {
                        *colon_pos = '\0';
                        char *default_value = colon_pos + 2;
                        result = dl_strjoin(result, default_value);
                    }
                    else
                    {
                        // No default value, expand to empty string (like bash)
                        // result = dl_strjoin(result, ""); // No need to join empty string
                    }
                }
            }
            else
            {
                result = dl_strjoin(result, "$");
            }
            
            if (var_name)
                free(var_name);
            
            // Skip to end of variable
            if (*var_end == '}')
                i = (var_end - str) + 1;
            else
                i = (var_end - str);
        }
        else
        {
            // Copy literal character
            char temp[2] = {str[i], '\0'};
            result = dl_strjoin(result, temp);
            i++;
        }
    }
    
    return (result);
}
