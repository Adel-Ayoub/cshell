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

// Job control constants
#define MAX_JOBS 100

// Job status constants
#define JOB_NONE 0
#define JOB_RUNNING 1
#define JOB_STOPPED 2
#define JOB_COMPLETED 3

// Job structure
typedef struct s_job
{
    pid_t pid;
    char *command;
    int status;
    int exit_status;
    time_t start_time;
} t_job;

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

// Trinary tree node for command chaining
typedef struct s_trinary
{
    int             type;           // 0=command, 1=AND, 2=OR, 3=BACKGROUND
    int             ret;            // Return value
    int             hd_n;          // Here-doc number
    char            *content;       // Command string
    struct s_trinary   *up;           // Parent node
    struct s_trinary   *first_cond;   // First child (for AND/OR)
    struct s_trinary   *sec_cond;     // Second child (for AND/OR)
    struct s_trinary   *back;         // Previous node
    struct s_trinary   *next;         // Next node (for semicolons)
} t_trinary;

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
    int *quoted_args;    // Track which args were quoted (skip wildcard expansion)
    int args_count;
    
    // Redirections
    t_redi *redirections;
    int redir_count;
    int original_stdin;
    int original_stdout;
    
    // Pipes
    int *pipes;           // Array of pipe file descriptors
    int pipe_count;
    pid_t *child_pids;
    int child_count;
    int cmd_amount;
    int pipe_amount;
    int check;  // For pipe file descriptor setup status
    
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
    
    // Job control
    t_job *background_jobs;
    int job_count;
    
    // Trinary tree for command chaining
    t_trinary *trinary_tree;
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
void handle_sigtstp(int sig);
void handle_child_signals(void);
void block_signals(void);
void unblock_signals(void);

// Input handling
char *read_input(void);
int parse_input(char *input);
int tokenize_input(char *input);
int count_tokens_with_quotes(char *input);
int fill_tokens_with_quotes(char *input, char **tokens);
int expand_environment_variables(void);
int expand_environment_array(char **array);
char *expand_argument(char *arg);
char *expand_environment_string(char *str);
int expand_wildcards(void);
int wild_card_check(char *pattern);
int match_pattern(const char *filename, const char *pattern);
int match_glob_pattern(const char *filename, const char *pattern);
int match_question_pattern(const char *filename, const char *pattern);
int match_bracket_pattern(const char *filename, const char *pattern);
int match_mixed_pattern(const char *filename, const char *pattern);
int match_mixed_pattern_recursive(const char *filename, const char *pattern, int filename_pos, int pattern_pos);
char **expand_wildcard_pattern(const char *pattern);
char *expand_range_pattern(const char *pattern);
void sort_string_array(char **array, int count);

// Recursive directory pattern support
int is_recursive_pattern(const char *pattern);
char **expand_recursive_pattern(const char *pattern);
int search_recursive_matches(const char *current_dir, const char *pattern, char **matches, int *match_count, int max_matches);
int match_recursive_pattern(const char *file_path, const char *pattern);
char **split_pattern_path(const char *pattern);
char **split_file_path(const char *file_path);
int match_path_components(char **pattern_parts, char **path_parts);
int validate_syntax(void);

// Parsing
t_args *create_args_list(char **array);
char **convert_args_to_array(t_args *list);
void free_args_list(t_args *list);
int parse_redirections(void);
int parse_logical_operators(void);
int process_single_command(char *command);
char *trim_whitespace(char *str);

// Logical operator parsing
int has_logical_operators(const char *str);
int find_logical_operator(const char *str, int *operator_type);
int parse_logical_expression(const char *str, char **left, char **right, int *operator_type);
void cleanup_logical_expression(char **left, char **right);

// Complex expression parsing with operator precedence
int parse_complex_expression(const char *str, char **left, char **right, int *operator_type);

// Trinary tree functions
t_trinary *create_level(char *str, t_trinary *back, t_trinary *up, int var);
t_trinary *create_condition_node(char *str, t_trinary *up);
t_trinary *create_tokenode(t_error help, char *str, t_trinary *back, t_trinary *up);
int go_through_list(void);
void traveler(t_trinary *current);
void exec_node(t_trinary *current);
void empty_tree(t_trinary *head);

// Redirections
int parse_redirections(void);
int cleanup_redirection_args(void);
int setup_redirections(void);
int open_redirection_files(void);
int handle_here_documents(void);
int handle_single_heredoc(t_redi *current);
int apply_redirections(void);
void restore_redirections(void);
void cleanup_redirections(void);

// Commands
int execute_commands(void);
int execute_builtin(char **args);
int execute_external(char **args);
int find_executable(char *command);
int setup_pipes(void);
void cleanup_pipes(void);

// Pipes
int handle_pipe(char *cmd_str);
int handle_pipe_direct(char *cmd_str);
int init_pipes(void);
void go_through_pipeline(char **commands);
void the_piper(int cmd_index);
void the_kindergarden(int cmd_index, char *cmd_str);
int prepare_command(int cmd_index, char *cmd_str);
void close_pipeline(void);
void free_commands(void);
char **parse_pipe_commands(char *cmd_str);
char **parse_single_command(char *cmd_str);
t_redi *find_redirection_node(int type);

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
int builtin_history(char **args);
int builtin_type(char **args);
int builtin_help(char **args);
int builtin_help_specific(char *command);
int builtin_jobs(char **args);
int add_background_job(pid_t pid, char *command);
int remove_background_job(pid_t pid);
void update_job_status(pid_t pid, int status);
int builtin_fg(char **args);
int builtin_bg(char **args);
char *find_command_path(char *command);

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
int set_environment_variable(char *name, char *value);
int unset_environment_variable(char *name);

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
