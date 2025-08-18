#include "cshell.h"

void setup_signals(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;
    
    // Setup SIGINT handler
    dl_memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);
    
    // Setup SIGQUIT handler
    dl_memset(&sa_quit, 0, sizeof(sa_quit));
    sa_quit.sa_handler = handle_sigquit;
    sa_quit.sa_flags = SA_RESTART;
    sigaction(SIGQUIT, &sa_quit, NULL);
}

void handle_sigint(int sig)
{
    (void)sig;
    
    if (g_data.interactive_mode)
    {
        dl_putchar_fd('\n', STDOUT_FILENO);
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

void handle_sigquit(int sig)
{
    (void)sig;
    
    if (g_data.interactive_mode)
    {
        // Ignore SIGQUIT in interactive mode (like bash)
        return;
    }
}

void handle_child_signals(void)
{
    // Restore default signal handlers for child processes
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}
