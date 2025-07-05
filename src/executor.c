#include "executor.h"

static char *builtin_commands[] = {
    "ls", "cd", "cat", "pwd", "mv", "grep",
    "mkdir", "chmod", "rm", "cp"
};

static int num_builtins = sizeof(builtin_commands) / sizeof(char*);

int is_builtin(char *command) {
    for (int i = 0; i < num_builtins; i++) {
        if (strcmp(command, builtin_commands[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int execute_command(int argc, char **args) {
    int background = check_background(args, &argc);

    if (is_builtin(args[0])) {
        if (background) {
            pid_t pid = fork();
            if (pid == 0) {
                setpgid(0, 0);
                execute_builtin(argc, args);
                exit(0);
            } else if (pid > 0) {
                printf("Background process started: %d\n", pid);
                waitpid(pid, NULL, WNOHANG);
            } else {
                perror("fork failed");
            }
        } else {
            return execute_builtin(argc, args);
        }
    } else {
        return execute_external(argc, args);
    }
    return 1;
}

int execute_builtin(int argc, char **args) {
    if (strcmp(args[0], "ls") == 0) {
        return cmd_ls(argc, args);
    } else if (strcmp(args[0], "cd") == 0) {
        return cmd_cd(args);
    } else if (strcmp(args[0], "cat") == 0) {
        return cmd_cat(argc, args);
    } else if (strcmp(args[0], "pwd") == 0) {
        return cmd_pwd();
    } else if (strcmp(args[0], "mv") == 0) {
        return cmd_mv(argc, args);
    } else if (strcmp(args[0], "grep") == 0) {
        return cmd_grep(argc, args);
    } else if (strcmp(args[0], "mkdir") == 0) {
        return cmd_mkdir(args[1]);
    } else if (strcmp(args[0], "chmod") == 0) {
        return cmd_chmod(argc, args);
    } else if (strcmp(args[0], "rm") == 0) {
        return cmd_rm(argc, args);
    } else if (strcmp(args[0], "cp") == 0) {
        return cmd_cp(argc, args);
    }
    return 0;
}

int execute_external(int argc __attribute__((unused)), char **args) {
    pid_t pid = fork();
    int status;

    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("command not found");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Fork failed
        perror("fork failed");
        return 0;
    } else {
        // Parent process
        wait(&status);
    }
    return 1;
}
