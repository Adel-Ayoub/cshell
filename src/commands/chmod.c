#include "../shell.h"

int cmd_chmod(int argc, char **args) {
    if (argc < 3) {
        fprintf(stderr, "chmod: missing arguments\n");
        return 0;
    }

    mode_t mode = strtol(args[1], NULL, 8);

    if (chmod(args[2], mode) == -1) {
        perror("chmod");
        return 0;
    }

    return 1;
}
