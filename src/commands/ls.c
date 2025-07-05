#include "../shell.h"

int cmd_ls(int argc, char **args) {
    DIR *dir;
    struct dirent *entry;

    if (argc == 1) {
        // List current directory
        dir = opendir(".");
        if (dir) {
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_name[0] != '.') {  // Skip hidden files
                    printf("%s  ", entry->d_name);
                }
            }
            printf("\n");
            closedir(dir);
            return 1;
        }
    } else {
        // List specified directories/files
        for (int i = 1; i < argc; i++) {
            if (is_directory(args[i])) {
                printf("%s:\n", args[i]);
                dir = opendir(args[i]);
                if (dir) {
                    while ((entry = readdir(dir)) != NULL) {
                        if (entry->d_name[0] != '.') {
                            printf("%s  ", entry->d_name);
                        }
                    }
                    printf("\n\n");
                    closedir(dir);
                }
            } else if (is_file(args[i])) {
                printf("%s  ", args[i]);
            }
        }
        printf("\n");
    }
    return 1;
}
