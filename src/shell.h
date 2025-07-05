#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64
#define MAX_PATH_LENGTH 1024

// Command function declarations
int cmd_pwd(void);
int cmd_cd(char **args);
int cmd_ls(int argc, char **args);
int cmd_cat(int argc, char **args);
int cmd_mkdir(char *dirname);
int cmd_mv(int argc, char **args);
int cmd_cp(int argc, char **args);
int cmd_rm(int argc, char **args);
int cmd_chmod(int argc, char **args);
int cmd_grep(int argc, char **args);

// Utility functions
int is_directory(const char *path);
int is_file(const char *path);
int count_args(char **args);

// Core shell functions (from parser.h and executor.h)
char *read_input(void);
char **parse_input(char *input);
int execute_command(int argc, char **args);
int execute_builtin(int argc, char **args);
int execute_external(int argc, char **args);
int is_builtin(char *command);
int check_background(char **args, int *argc);

// Shell loop
void shell_loop(void);

#endif
