#include "cshell.h"

// Initialize pipes for command pipeline
int init_pipes(void)
{
    int i;
    
    // Calculate number of pipes needed
    g_data.pipe_amount = (g_data.cmd_amount - 1) * 2;
    
    if (g_data.pipe_amount <= 0)
        return (0);
    
    // Allocate child PIDs array
    g_data.child_pids = (pid_t *)dl_calloc(g_data.cmd_amount, sizeof(pid_t));
    if (!g_data.child_pids)
        return (-1);
    
    // Allocate flat pipe array
    g_data.pipes = (int *)dl_calloc(g_data.pipe_amount, sizeof(int));
    if (!g_data.pipes)
    {
        free(g_data.child_pids);
        g_data.child_pids = NULL;
        return (-1);
    }
    
    // Create pipes (pipe[2*i] and pipe[2*i+1])
    for (i = 0; i < g_data.cmd_amount - 1; i++)
    {
        if (pipe(&g_data.pipes[2 * i]) < 0)
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
    
    // Close all pipe file descriptors
    for (i = 0; i < g_data.pipe_amount; i++)
    {
        close(g_data.pipes[i]);
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
