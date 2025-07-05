#include "../shell.h"

int cmd_cat(int argc, char **args) {
    if (argc < 2) {
        fprintf(stderr, "cat: missing file argument\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        FILE *file = fopen(args[i], "r");
        if (file == NULL) {
            perror(args[i]);
            continue;
        }

        printf("%s:\n", args[i]);

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("%s", buffer);
        }

        fclose(file);
        printf("\n\n");
    }

    return 1;
}
