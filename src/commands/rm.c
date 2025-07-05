#include "../shell.h"

static int remove_recursive(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return remove(path);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (is_directory(full_path)) {
            remove_recursive(full_path);
        } else {
            remove(full_path);
        }
    }

    closedir(dir);
    return remove(path);
}

int cmd_rm(int argc, char **args) {
    if (argc < 2) {
        fprintf(stderr, "rm: missing arguments\n");
        return 0;
    }

    int recursive = 0;
    int start_index = 1;

    if (strcmp(args[1], "-r") == 0) {
        recursive = 1;
        start_index = 2;
    }

    for (int i = start_index; i < argc; i++) {
        if (recursive && is_directory(args[i])) {
            if (remove_recursive(args[i]) == -1) {
                perror(args[i]);
            } else {
                printf("%s removed\n", args[i]);
            }
        } else {
            if (remove(args[i]) == -1) {
                perror(args[i]);
            } else {
                printf("%s removed\n", args[i]);
            }
        }
    }

    return 1;
}
