#include "../shell.h"

int cmd_mv(int argc, char **args) {
    if (argc < 3) {
        fprintf(stderr, "mv: missing arguments\n");
        return 0;
    }

    char *source = args[1];
    char *dest = args[argc - 1];

    if (is_directory(dest)) {
        // Moving files into directory
        for (int i = 1; i < argc - 1; i++) {
            char dest_path[MAX_PATH_LENGTH];
            snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, args[i]);

            if (rename(args[i], dest_path) == -1) {
                perror("mv");
                return 0;
            }
        }
    } else {
        // Renaming single file
        if (rename(source, dest) == -1) {
            perror("mv");
            return 0;
        }
    }

    return 1;
}
