#include "cshell.h"

int builtin_history(char **args)
{
    int i;
    int max_lines;
    
    (void)args; // Suppress unused parameter warning
    
    // Get maximum number of history lines to display
    max_lines = 100; // Default limit
    
    // Display history (readline handles this automatically)
    if (g_data.interactive_mode)
    {
        // Use readline's history functions
        HIST_ENTRY **hist_list = history_list();
        if (hist_list)
        {
            for (i = 0; hist_list[i] && i < max_lines; i++)
            {
                // Format: line_number command
                char *line_num = dl_itoa(i + 1);
                if (line_num)
                {
                    dl_putstr_fd(line_num, STDOUT_FILENO);
                    free(line_num);
                }
                dl_putstr_fd("  ", STDOUT_FILENO);
                dl_putendl_fd(hist_list[i]->line, STDOUT_FILENO);
            }
        }
        else
        {
            dl_putendl_fd("No command history available", STDOUT_FILENO);
        }
    }
    else
    {
        dl_putendl_fd("History not available in non-interactive mode", STDOUT_FILENO);
    }
    
    return (EXIT_SUCCESS);
}

int clear_shell_history(void)
{
    if (g_data.interactive_mode)
    {
        clear_history();
        dl_putendl_fd("Command history cleared", STDOUT_FILENO);
    }
    
    return (EXIT_SUCCESS);
}
