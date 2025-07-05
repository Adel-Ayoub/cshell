#include "shell.h"

void shell_loop(void) {
    char *input;
    char **args;
    int status = 1;
    int argc;

    while (status) {
        // Display prompt
        char current_dir[MAX_PATH_LENGTH];
        if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
            printf("\n%s>> ", current_dir);
        } else {
            printf("cshell>> ");
        }

        // Read input
        input = read_input();
        if (input == NULL) {
            break;
        }

        // Parse input
        args = parse_input(input);
        if (args[0] == NULL) {
            free(input);
            free(args);
            continue;
        }

        // Count arguments
        argc = count_args(args);

        // Check for exit command
        if (strcmp(args[0], "exit") == 0) {
            status = 0;
        } else {
            // Execute command
            execute_command(argc, args);
        }

        // Cleanup
        free(input);
        free(args);
    }
}

int count_args(char **args) {
    int count = 0;
    while (args[count] != NULL) {
        count++;
    }
    return count;
}
