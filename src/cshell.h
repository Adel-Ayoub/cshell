#ifndef CSHELL_H
#define CSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "../libdl/libdl.h"

// Constants
#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64
#define MAX_PATH_LENGTH 1024
#define MAX_ENV_SIZE 1000
#define MAX_REDIRECTIONS 10
#define MAX_PIPES 10

// Exit statuses
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define EXIT_SYNTAX_ERROR 2
#define EXIT_COMMAND_NOT_FOUND 127

// Redirection types
#define REDIR_IN 1      // <
#define REDIR_OUT 2     // >
#define REDIR_APPEND 3  // >>
#define REDIR_HEREDOC 4 // <<

// Data structures
typedef struct s_redi {
    int type;
    char *file;
    int fd;
    int usage;
    int start;
    int end;
    struct s_redi *next;
} t_redi;

typedef struct s_args {
    char *arg;
    struct s_args *next;
} t_args;

typedef struct s_wild {
    char *pattern;
    struct s_wild *next;
} t_wild;

typedef struct s_whelp {
    char *str;
    struct s_whelp *next;
} t_whelp;

typedef struct s_split {
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    int o;
    int p;
    int q;
    int r;
    int s;
    int t;
    int u;
    int v;
    int w;
    int x;
    int y;
    int z;
} t_split;

typedef struct s_error {
    int syntax_error;
    int quote_error;
    int redirection_error;
    int command_error;
    char *error_message;
} t_error;

typedef struct s_cond {
    char **args;
    t_redi *redirections;
    struct s_cond *left;
    struct s_cond *right;
    int operator; // 0 = none, 1 = &&, 2 = ||
} t_cond;

typedef struct s_data {
    // Terminal and signals
    struct termios original_termios;
    int interactive_mode;
    
    // Environment
    char **env;
    int env_size;
    int shell_level;
    
    // Command processing
    char *input_line;
    t_args *args_list;
    char **args_array;
    int args_count;
    
    // Redirections
    t_redi *redirections;
    int redir_count;
    
    // Pipes
    int **pipes;
    int pipe_count;
    pid_t *child_pids;
    int child_count;
    
    // Parsing state
    t_split split_state;
    t_error error_state;
    
    // Logical operators
    t_cond *command_tree;
    
    // Exit status
    int exit_status;
    int last_exit_status;
    
    // Wildcards
    t_wild *wildcards;
    t_whelp *wild_help;
    
    // Here-doc
    char **here_docs;
    int here_doc_count;
} t_data;

// Global data structure
extern t_data g_data;

// Core functions
int main(int argc, char **argv, char **env);
void shell_loop(void);
int setup_shell(char **env);
void cleanup_shell(void);
char *get_prompt(void);

// Signal handlers
void setup_signals(void);
void handle_sigint(int sig);
void handle_sigquit(int sig);
void handle_child_signals(void);

// Input handling
char *read_input(void);
int parse_input(char *input);
int tokenize_input(char *input);
int expand_environment_variables(void);
int expand_wildcards(void);
int validate_syntax(void);

// Parsing
t_args *create_args_list(char **array);
char **convert_args_to_array(t_args *list);
void free_args_list(t_args *list);
int parse_redirections(void);
int parse_logical_operators(void);

// Redirections
int setup_redirections(void);
int open_redirection_files(void);
int handle_here_documents(void);
void cleanup_redirections(void);

// Commands
int execute_commands(void);
int execute_builtin(char **args);
int execute_external(char **args);
int find_executable(char *command);
int setup_pipes(void);
void cleanup_pipes(void);

// Cross-module helpers
int work_built(char **args);
int find_the_paths(char *command);

// Builtins
int builtin_echo(char **args);
int builtin_cd(char **args);
int builtin_pwd(void);
int builtin_env(void);
int builtin_export(char **args);
int builtin_unset(char **args);
int builtin_exit(char **args);

// Builtin helpers
int is_builtin(char *command);
int is_built_in(char *command);

// Additional builtin helpers
void display_exported_variables(void);
int is_valid_variable_name(char *name);

// Environment
int copy_environment(char **env);
int add_environment_variable(char *name, char *value);
int remove_environment_variable(char *name);
char *get_environment_variable(char *name);
int update_shell_level(void);

// Memory management
void cleanup_all(void);
void free_string_array(char **array);
void free_redirections(t_redi *list);
void free_wildcards(t_wild *list);
void free_wild_help(t_whelp *list);
void free_command_tree(t_cond *tree);

// Error handling
void set_error(int type, const char *message);
void print_error(const char *command, const char *message);
void reset_error_state(void);

// Extra libdl helpers used in cshell
char *dl_strtok(char *str, const char *delim);

#endif
