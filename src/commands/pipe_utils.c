#include "cshell.h"

// Initialize pipes for command pipeline
int init_pipes(void)
{
    int i;
    
    // Calculate number of pipes needed
    g_data.pipe_amount = (g_data.cmd_amount - 1) * 2;
    
    if (g_data.pipe_amount <= 0)
        return (0);
    
    // Allocate pipe array
    g_data.pipes = (int **)dl_calloc(g_data.cmd_amount - 1, sizeof(int *));
    if (!g_data.pipes)
        return (-1);
    
    // Create pipes
    for (i = 0; i < g_data.cmd_amount - 1; i++)
    {
        g_data.pipes[i] = (int *)dl_calloc(2, sizeof(int));
        if (!g_data.pipes[i])
        {
            close_pipeline();
            return (-1);
        }
        
        if (pipe(g_data.pipes[i]) < 0)
        {
            close_pipeline();
            return (-1);
        }
    }
    
    return (0);
}

// Close all pipes in the pipeline
void close_pipeline(void)
{
    int i;
    
    if (!g_data.pipes)
        return;
    
    for (i = 0; i < g_data.cmd_amount - 1; i++)
    {
        if (g_data.pipes[i])
        {
            close(g_data.pipes[i][0]);
            close(g_data.pipes[i][1]);
            free(g_data.pipes[i]);
        }
    }
    
    free(g_data.pipes);
    g_data.pipes = NULL;
}

// Free command-related memory
void free_commands(void)
{
    if (g_data.args_array)
        free_string_array(g_data.args_array);
    g_data.args_array = NULL;
    g_data.args_count = 0;
}
