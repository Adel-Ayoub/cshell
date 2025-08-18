#include "cshell.h"

t_data g_data;

int main(int argc, char **argv, char **env)
{
    (void)argc;
    (void)argv;
    
    // Initialize global data structure
    dl_memset(&g_data, 0, sizeof(t_data));
    
    // Setup shell environment
    if (setup_shell(env) != 0)
    {
        dl_putstr_fd("cshell: initialization failed\n", STDERR_FILENO);
        return (EXIT_FAILURE);
    }
    
    // Main shell loop
    shell_loop();
    
    // Cleanup and exit
    cleanup_shell();
    return (g_data.exit_status);
}
