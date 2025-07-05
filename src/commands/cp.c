#include "../shell.h"

static int copy_file(const char *source, const char *dest) {
    FILE *src = fopen(source, "r");
    if (src == NULL) {
        perror(source);
        return 0;
    }

    FILE *dst = fopen(dest, "w");
    if (dst == NULL) {
        perror(dest);
        fclose(src);
        return 0;
    }

    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            perror("write error");
            fclose(src);
            fclose(dst);
            return 0;
        }
    }

    fclose(src);
    fclose(dst);
    return 1;
}

int cmd_cp(int argc, char **args) {
    if (argc < 3) {
        fprintf(stderr, "cp: missing arguments\n");
        return 0;
    }

    int recursive = 0;
    int start_index = 1;

    if (strcmp(args[1], "-r") == 0) {
        recursive = 1;
        start_index = 2;
        if (argc < 4) {
            fprintf(stderr, "cp: missing arguments after -r\n");
            return 0;
        }
    }

    char *dest = args[argc - 1];

    if (is_directory(dest)) {
        // Copy files into directory
        for (int i = start_index; i < argc - 1; i++) {
            char dest_path[MAX_PATH_LENGTH];
            snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, args[i]);

            if (!copy_file(args[i], dest_path)) {
                return 0;
            }
        }
    } else {
        // Copy single file
        if (!copy_file(args[start_index], dest)) {
            return 0;
        }
    }

    return 1;
}
