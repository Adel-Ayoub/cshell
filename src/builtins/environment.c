#include "cshell.h"

int copy_environment(char **env)
{
    int count;
    int i;

    // Count environment variables
    count = 0;
    while (env[count])
        count++;
    
    // Allocate space for environment array
    g_data.env = (char **)dl_calloc(count + 1, sizeof(char *));
    if (!g_data.env)
        return (-1);
    
    // Copy environment variables
    i = 0;
    while (i < count)
    {
        g_data.env[i] = dl_strdup(env[i]);
        if (!g_data.env[i])
        {
            free_string_array(g_data.env);
            return (-1);
        }
        i++;
    }
    
    g_data.env_size = count;
    return (0);
}

void display_env(void)
{
    int i;

    i = 0;
    while (g_data.env[i])
    {
        dl_putendl_fd(g_data.env[i], STDOUT_FILENO);
        i++;
    }
}

int builtin_env(void)
{
    display_env();
    return (EXIT_SUCCESS);
}

char *get_environment_variable(char *name)
{
    int i;
    int name_len;
    char *env_var;

    if (!name || !g_data.env)
        return (NULL);
    
    name_len = dl_strlen(name);
    i = 0;
    while (g_data.env[i])
    {
        if (dl_strncmp(g_data.env[i], name, name_len) == 0 && 
            g_data.env[i][name_len] == '=')
        {
            env_var = dl_strdup(g_data.env[i] + name_len + 1);
            return (env_var);
        }
        i++;
    }
    return (NULL);
}

int update_shell_level(void)
{
    char *shlvl_str;
    char *new_shlvl;
    int current_level;

    shlvl_str = get_environment_variable("SHLVL");
    if (shlvl_str)
    {
        current_level = dl_atoi(shlvl_str);
        free(shlvl_str);
        current_level++;
    }
    else
    {
        current_level = 1;
    }
    
    new_shlvl = dl_strjoin("SHLVL=", dl_itoa(current_level));
    if (new_shlvl)
    {
        add_environment_variable("SHLVL", dl_itoa(current_level));
        free(new_shlvl);
    }
    
    g_data.shell_level = current_level;
    return (0);
}
