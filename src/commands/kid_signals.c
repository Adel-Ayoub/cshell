#include "cshell.h"

void kid_signals(void)
{
    handle_child_signals();
}

void parent_signals(void)
{
    // Restore parent signal handlers
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);
}
