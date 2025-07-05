#include "../shell.h"

int cmd_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "cd: missing argument\n");
        return 0;
    }

    if (!is_directory(args[1])) {
        fprintf(stderr, "cd: %s: Not a directory\n", args[1]);
        return 0;
    }

    if (chdir(args[1]) != 0) {
        perror("cd");
        return 0;
    }

    return 1;
}
