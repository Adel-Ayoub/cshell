#include "cshell.h"

int builtin_help(char **args)
{
    (void)args; // Suppress unused parameter warning
    
    dl_putendl_fd("cshell - Custom Shell", STDOUT_FILENO);
    dl_putendl_fd("", STDOUT_FILENO);
    dl_putendl_fd("Built-in commands:", STDOUT_FILENO);
    dl_putendl_fd("  echo [text]        - Display text", STDOUT_FILENO);
    dl_putendl_fd("  cd [directory]     - Change directory", STDOUT_FILENO);
    dl_putendl_fd("  pwd                - Print working directory", STDOUT_FILENO);
    dl_putendl_fd("  env                - Display environment variables", STDOUT_FILENO);
    dl_putendl_fd("  export [var=value] - Set environment variable", STDOUT_FILENO);
    dl_putendl_fd("  unset [variable]   - Remove environment variable", STDOUT_FILENO);
    dl_putendl_fd("  exit [status]      - Exit shell with optional status", STDOUT_FILENO);
    dl_putendl_fd("  history            - Show command history", STDOUT_FILENO);
    dl_putendl_fd("  type [command]     - Show command type and location", STDOUT_FILENO);
    dl_putendl_fd("  help               - Show this help message", STDOUT_FILENO);
    dl_putendl_fd("", STDOUT_FILENO);
    dl_putendl_fd("Special features:", STDOUT_FILENO);
    dl_putendl_fd("  $VAR               - Environment variable expansion", STDOUT_FILENO);
    dl_putendl_fd("  \"text\"             - Quoted strings", STDOUT_FILENO);
    dl_putendl_fd("  ~                  - Home directory expansion", STDOUT_FILENO);
    dl_putendl_fd("  cd -               - Change to previous directory", STDOUT_FILENO);
    dl_putendl_fd("  Ctrl+C             - Interrupt current command", STDOUT_FILENO);
    dl_putendl_fd("  Tab completion     - Command and filename completion", STDOUT_FILENO);
    dl_putendl_fd("", STDOUT_FILENO);
    dl_putendl_fd("For more information about a specific command, use: type [command]", STDOUT_FILENO);
    
    return (EXIT_SUCCESS);
}

int builtin_help_specific(char *command)
{
    if (!command)
        return (builtin_help(NULL));
    
    if (dl_strncmp(command, "echo", 5) == 0)
    {
        dl_putendl_fd("echo: Display text", STDOUT_FILENO);
        dl_putendl_fd("Usage: echo [-n] [text...]", STDOUT_FILENO);
        dl_putendl_fd("  -n   Do not output trailing newline", STDOUT_FILENO);
        dl_putendl_fd("Supports environment variable expansion with $VAR", STDOUT_FILENO);
    }
    else if (dl_strncmp(command, "cd", 3) == 0)
    {
        dl_putendl_fd("cd: Change directory", STDOUT_FILENO);
        dl_putendl_fd("Usage: cd [directory]", STDOUT_FILENO);
        dl_putendl_fd("  cd          - Change to home directory", STDOUT_FILENO);
        dl_putendl_fd("  cd ~        - Change to home directory", STDOUT_FILENO);
        dl_putendl_fd("  cd -        - Change to previous directory", STDOUT_FILENO);
        dl_putendl_fd("  cd ~/path   - Change to path relative to home", STDOUT_FILENO);
    }
    else if (dl_strncmp(command, "export", 7) == 0)
    {
        dl_putendl_fd("export: Set environment variables", STDOUT_FILENO);
        dl_putendl_fd("Usage: export [variable=value]", STDOUT_FILENO);
        dl_putendl_fd("  export          - Display all exported variables", STDOUT_FILENO);
        dl_putendl_fd("  export VAR=val  - Set variable to value", STDOUT_FILENO);
        dl_putendl_fd("  export VAR=\"quoted value\"  - Set with quoted value", STDOUT_FILENO);
    }
    else
    {
        dl_putstr_fd("help: no help available for '", STDOUT_FILENO);
        dl_putstr_fd(command, STDOUT_FILENO);
        dl_putendl_fd("'", STDOUT_FILENO);
        return (EXIT_FAILURE);
    }
    
    return (EXIT_SUCCESS);
}
