#include "parser.h"

char *read_input(void) {
    char *line = NULL;
    size_t bufsize = 0;
    ssize_t line_size;

    line_size = getline(&line, &bufsize, stdin);
    if (line_size <= 0) {
        if (line) free(line);
        return NULL;
    }

    return line;
}

char **parse_input(char *input) {
    int bufsize = MAX_ARGS;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
    char delim[] = " \t\r\n\a";

    if (!tokens) {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(input, delim);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += MAX_ARGS;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, delim);
    }

    tokens[position] = NULL;
    return tokens;
}

int check_background(char **args, int *argc) {
    if (*argc > 0 && strcmp(args[*argc - 1], "&") == 0) {
        args[*argc - 1] = NULL;
        (*argc)--;
        return 1;
    }
    return 0;
}
