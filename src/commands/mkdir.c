#include "../shell.h"

int cmd_mkdir(char *dirname) {
    if (dirname == NULL) {
        fprintf(stderr, "mkdir: missing directory name\n");
        return 0;
    }

    if (mkdir(dirname, 0755) == -1) {
        perror("mkdir");
        return 0;
    }

    return 1;
}
