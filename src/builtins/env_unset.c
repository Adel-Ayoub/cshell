#include "cshell.h"

int builtin_unset(char **args)
{
    int i;
    int result;

    if (!args[1])
    {
        // No arguments provided
        return (EXIT_SUCCESS);
    }
    
    result = EXIT_SUCCESS;
    i = 1;
    while (args[i])
    {
        if (!is_valid_variable_name(args[i]))
        {
            print_error("unset", "not a valid identifier");
            result = EXIT_FAILURE;
        }
        else
        {
            if (unset_environment_variable(args[i]) != 0)
            {
                print_error("unset", "failed to unset variable");
                result = EXIT_FAILURE;
            }
        }
        i++;
    }
    
    return (result);
}


