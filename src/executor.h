#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "shell.h"

int execute_command(int argc, char **args);
int execute_builtin(int argc, char **args);
int execute_external(int argc, char **args);
int is_builtin(char *command);

#endif
