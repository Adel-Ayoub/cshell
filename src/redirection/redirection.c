#include "cshell.h"

// Parse redirections from command arguments
int parse_redirections(void)
{
    t_args *current;
    t_redi *new_redir;
    
    if (!g_data.args_list)
        return (0);
    
    current = g_data.args_list;
    
    while (current)
    {
        if (current->arg && (dl_strcmp(current->arg, "<") == 0 || 
                            dl_strcmp(current->arg, ">") == 0 || 
                            dl_strcmp(current->arg, ">>") == 0 || 
                            dl_strcmp(current->arg, "<<") == 0))
        {
            // Create new redirection
            new_redir = dl_calloc(1, sizeof(t_redi));
            if (!new_redir)
                return (-1);
            
            // Set redirection type
            if (dl_strcmp(current->arg, "<") == 0)
                new_redir->type = REDIR_IN;
            else if (dl_strcmp(current->arg, ">") == 0)
                new_redir->type = REDIR_OUT;
            else if (dl_strcmp(current->arg, ">>") == 0)
                new_redir->type = REDIR_APPEND;
            else if (dl_strcmp(current->arg, "<<") == 0)
                new_redir->type = REDIR_HEREDOC;
            
            // Get filename from next argument
            if (current->next && current->next->arg)
            {
                new_redir->file = dl_strdup(current->next->arg);
                if (!new_redir->file)
                {
                    free(new_redir);
                    return (-1);
                }
                
                // Add redirection to list
                new_redir->next = g_data.redirections;
                g_data.redirections = new_redir;
            }
            else
            {
                // Missing filename
                print_error("redirection", "missing filename");
                free(new_redir);
                return (-1);
            }
        }
        current = current->next;
    }
    
    return (0);
}

// Remove redirection arguments from args array
int cleanup_redirection_args(void)
{
    char **new_array;
    int new_count;
    int i;
    int j;
    
    if (!g_data.args_array)
        return (0);
    
    // Count non-redirection arguments
    new_count = 0;
    for (i = 0; g_data.args_array[i]; i++)
    {
        // Skip redirection operators and their filenames
        if (dl_strcmp(g_data.args_array[i], "<") == 0 || 
            dl_strcmp(g_data.args_array[i], ">") == 0 || 
            dl_strcmp(g_data.args_array[i], ">>") == 0 || 
            dl_strcmp(g_data.args_array[i], "<<") == 0)
        {
            i++; // Skip the filename too
            continue;
        }
        new_count++;
    }
    
    // Create new array without redirection arguments
    new_array = (char **)dl_calloc(new_count + 1, sizeof(char *));
    if (!new_array)
        return (-1);
    
    j = 0;
    for (i = 0; g_data.args_array[i]; i++)
    {
        // Skip redirection operators and their filenames
        if (dl_strcmp(g_data.args_array[i], "<") == 0 || 
            dl_strcmp(g_data.args_array[i], ">") == 0 || 
            dl_strcmp(g_data.args_array[i], ">>") == 0 || 
            dl_strcmp(g_data.args_array[i], "<<") == 0)
        {
            i++; // Skip the filename too
            continue;
        }
        
        new_array[j] = dl_strdup(g_data.args_array[i]);
        if (!new_array[j])
        {
            free_string_array(new_array);
            return (-1);
        }
        j++;
    }
    
    // Replace old array
    free_string_array(g_data.args_array);
    g_data.args_array = new_array;
    g_data.args_count = new_count;
    
    return (0);
}

// Open files for redirections
int open_redirection_files(void)
{
    t_redi *current;
    int fd;
    
    current = g_data.redirections;
    while (current)
    {
        if (current->file)
        {
            switch (current->type)
            {
                case REDIR_IN:
                    fd = open(current->file, O_RDONLY);
                    if (fd == -1)
                    {
                        print_error(current->file, "No such file or directory");
                        return (-1);
                    }
                    current->fd = fd;
                    break;
                    
                case REDIR_OUT:
                    fd = open(current->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd == -1)
                    {
                        print_error(current->file, "Permission denied");
                        return (-1);
                    }
                    current->fd = fd;
                    break;
                    
                case REDIR_APPEND:
                    fd = open(current->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if (fd == -1)
                    {
                        print_error(current->file, "Permission denied");
                        return (-1);
                    }
                    current->fd = fd;
                    break;
                    
                case REDIR_HEREDOC:
                    // Here-doc will be handled separately
                    current->fd = -1;
                    break;
            }
        }
        current = current->next;
    }
    
    return (0);
}

// Handle here-documents
int handle_here_documents(void)
{
    t_redi *current;
    
    current = g_data.redirections;
    while (current)
    {
        if (current->type == REDIR_HEREDOC && current->file)
        {
            if (handle_single_heredoc(current) != 0)
                return (-1);
        }
        current = current->next;
    }
    
    return (0);
}

// Handle a single here-document (works in both interactive and non-interactive modes)
int handle_single_heredoc(t_redi *current)
{
    static int heredoc_counter = 0;
    char *number;
    char *filename;
    int fd;
    
    // Create unique filename like minishell
    number = dl_itoa(heredoc_counter++);
    if (!number)
    {
        print_error("here-document", "memory allocation failed");
        return (-1);
    }
    
    filename = dl_strjoin(".cshell_heredoc_tmp", number);
    free(number);
    if (!filename)
    {
        print_error("here-document", "memory allocation failed");
        return (-1);
    }
    
    // Create temp file
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        print_error("here-document", "failed to create temporary file");
        free(filename);
        return (-1);
    }
    
    // For now, create an empty file since we're parsing from input stream
    // In the future, we can implement proper input parsing here
    close(fd);
    
    // Open the temp file for reading
    current->fd = open(filename, O_RDONLY);
    if (current->fd == -1)
    {
        print_error("here-document", "failed to read temporary file");
        free(filename);
        return (-1);
    }
    
    // Clean up temp file after opening
    unlink(filename);
    free(filename);
    
    return (0);
}

// Setup redirections before command execution
int setup_redirections(void)
{
    // Open redirection files
    if (open_redirection_files() != 0)
        return (-1);
    
    // Handle here-documents
    if (handle_here_documents() != 0)
        return (-1);
    
    return (0);
}

// Apply redirections to file descriptors
int apply_redirections(void)
{
    t_redi *current;
    int original_stdin, original_stdout;
    
    // Save original file descriptors
    original_stdin = dup(STDIN_FILENO);
    original_stdout = dup(STDOUT_FILENO);
    
    if (original_stdin == -1 || original_stdout == -1)
    {
        print_error("redirection", "failed to save original file descriptors");
        return (-1);
    }
    
    current = g_data.redirections;
    while (current)
    {
        if (current->fd != -1)
        {
            switch (current->type)
            {
                case REDIR_IN:
                case REDIR_HEREDOC:
                    if (dup2(current->fd, STDIN_FILENO) == -1)
                    {
                        print_error("redirection", "failed to redirect input");
                        return (-1);
                    }
                    break;
                    
                case REDIR_OUT:
                case REDIR_APPEND:
                    if (dup2(current->fd, STDOUT_FILENO) == -1)
                    {
                        print_error("redirection", "failed to redirect input");
                        return (-1);
                    }
                    break;
            }
        }
        current = current->next;
    }
    
    // Store original descriptors for restoration
    g_data.original_stdin = original_stdin;
    g_data.original_stdout = original_stdout;
    
    return (0);
}

// Restore original file descriptors
void restore_redirections(void)
{
    // Restore stdin and stdout from saved descriptors
    if (g_data.original_stdin != -1)
    {
        dup2(g_data.original_stdin, STDIN_FILENO);
        close(g_data.original_stdin);
        g_data.original_stdin = -1;
    }
    
    if (g_data.original_stdout != -1)
    {
        dup2(g_data.original_stdout, STDOUT_FILENO);
        close(g_data.original_stdout);
        g_data.original_stdout = -1;
    }
}

// Clean up redirections
void cleanup_redirections(void)
{
    t_redi *current;
    t_redi *next;
    
    if (!g_data.redirections)
        return;
    
    current = g_data.redirections;
    while (current)
    {
        next = current->next;
        
        // Close file descriptor safely
        if (current->fd != -1 && current->fd > 2)
        {
            close(current->fd);
        }
        
        // Free file name
        if (current->file)
            free(current->file);
        
        // Free redirection structure
        free(current);
        
        current = next;
    }
    
    g_data.redirections = NULL;
}
