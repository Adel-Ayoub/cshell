#include "../shell.h"

int cmd_pwd(void) {
    char current_dir[MAX_PATH_LENGTH];

    if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
        printf("%s\n", current_dir);
        return 1;
    } else {
        perror("pwd");
        return 0;
    }
}
