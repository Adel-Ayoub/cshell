#include "../shell.h"

int cmd_grep(int argc, char **args) {
    if (argc < 2) {
        fprintf(stderr, "grep: missing pattern\n");
        return 0;
    }

    char *pattern = args[1];

    if (argc == 2) {
        // Read from stdin
        char line[1024];
        while (fgets(line, sizeof(line), stdin) != NULL) {
            if (strstr(line, pattern) != NULL) {
                printf("%s", line);
            }
        }
    } else {
        // Read from files
        for (int i = 2; i < argc; i++) {
            FILE *file = fopen(args[i], "r");
            if (file == NULL) {
                perror(args[i]);
                continue;
            }

            char line[1024];
            while (fgets(line, sizeof(line), file) != NULL) {
                if (strstr(line, pattern) != NULL) {
                    printf("%s:%s", args[i], line);
                }
            }

            fclose(file);
        }
    }

    return 1;
}

// Utility functions implementation
int is_directory(const char *path) {
    struct stat stats;
    if (stat(path, &stats) == 0) {
        return S_ISDIR(stats.st_mode);
    }
    return 0;
}

int is_file(const char *path) {
    struct stat stats;
    if (stat(path, &stats) == 0) {
        return S_ISREG(stats.st_mode);
    }
    return 0;
}
