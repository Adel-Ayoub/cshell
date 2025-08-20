CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
NAME = cshell
SRCDIR = src
BUILDDIR = obj
LIBDLDIR = libdl

# Source files organized by module
CORE_SRC = $(SRCDIR)/core/main.c \
           $(SRCDIR)/core/signal_handlers.c \
           $(SRCDIR)/core/utils.c

BUILTINS_SRC = $(SRCDIR)/builtins/built.c \
               $(SRCDIR)/builtins/directory.c \
               $(SRCDIR)/builtins/env_export.c \
               $(SRCDIR)/builtins/env_unset.c \
               $(SRCDIR)/builtins/environment.c \
               $(SRCDIR)/builtins/env_utils.c \
               $(SRCDIR)/builtins/exit_val_calc.c \
               $(SRCDIR)/builtins/handle_built.c \
               $(SRCDIR)/builtins/help.c \
               $(SRCDIR)/builtins/history.c \
               $(SRCDIR)/builtins/type.c \
               $(SRCDIR)/builtins/jobs.c

COMMANDS_SRC = $(SRCDIR)/commands/finding_execs.c \
               $(SRCDIR)/commands/handle_commands.c \
               $(SRCDIR)/commands/kid_signals.c \
               $(SRCDIR)/commands/pipe_utils.c \
               $(SRCDIR)/commands/piping.c

INPUT_SRC = $(SRCDIR)/input_handling/error_checks.c \
            $(SRCDIR)/input_handling/error_parser.c \
            $(SRCDIR)/input_handling/error_utils.c \
            $(SRCDIR)/input_handling/expand_envs.c \
            $(SRCDIR)/input_handling/dl_ls.c \
            $(SRCDIR)/input_handling/lex_checks.c \
            $(SRCDIR)/input_handling/parse_split.c \
            $(SRCDIR)/input_handling/split_utils.c \
            $(SRCDIR)/input_handling/wild_cards.c \
            $(SRCDIR)/input_handling/wild_utils.c

REDIRECTION_SRC = $(SRCDIR)/redirection/here_doc.c \
                  $(SRCDIR)/redirection/open_files.c \
                  $(SRCDIR)/redirection/redirection.c

# All source files (excluding libdl, which is built as a library)
ALL_SRC = $(CORE_SRC) $(BUILTINS_SRC) $(COMMANDS_SRC) $(INPUT_SRC) $(REDIRECTION_SRC)

# Object files
OBJ = $(ALL_SRC:%.c=$(BUILDDIR)/%.o)

# Libraries
LIBDL = build/libdl.a

# libdl source files (for building the library)
LIBDL_SRC = $(LIBDLDIR)/dl_atoi.c \
            $(LIBDLDIR)/dl_bzero.c \
            $(LIBDLDIR)/dl_calloc.c \
            $(LIBDLDIR)/dl_isalnum.c \
            $(LIBDLDIR)/dl_isalpha.c \
            $(LIBDLDIR)/dl_isascii.c \
            $(LIBDLDIR)/dl_isdigit.c \
            $(LIBDLDIR)/dl_isprint.c \
            $(LIBDLDIR)/dl_itoa.c \
            $(LIBDLDIR)/dl_memchr.c \
            $(LIBDLDIR)/dl_memcmp.c \
            $(LIBDLDIR)/dl_memcpy.c \
            $(LIBDLDIR)/dl_memmove.c \
            $(LIBDLDIR)/dl_memset.c \
            $(LIBDLDIR)/dl_putchar_fd.c \
            $(LIBDLDIR)/dl_putendl_fd.c \
            $(LIBDLDIR)/dl_putnbr_fd.c \
            $(LIBDLDIR)/dl_putstr_fd.c \
            $(LIBDLDIR)/dl_split.c \
            $(LIBDLDIR)/dl_strchr.c \
            $(LIBDLDIR)/dl_strcmp.c \
            $(LIBDLDIR)/dl_strdup.c \
            $(LIBDLDIR)/dl_striteri.c \
            $(LIBDLDIR)/dl_strjoin.c \
            $(LIBDLDIR)/dl_strlcat.c \
            $(LIBDLDIR)/dl_strlcpy.c \
            $(LIBDLDIR)/dl_strlen.c \
            $(LIBDLDIR)/dl_strmapi.c \
            $(LIBDLDIR)/dl_strncmp.c \
            $(LIBDLDIR)/dl_strncpy.c \
            $(LIBDLDIR)/dl_strnstr.c \
            $(LIBDLDIR)/dl_strrchr.c \
            $(LIBDLDIR)/dl_strtrim.c \
            $(LIBDLDIR)/dl_substr.c \
            $(LIBDLDIR)/dl_tolower.c \
            $(LIBDLDIR)/dl_toupper.c \
            $(LIBDLDIR)/dl_lstadd_back_bonus.c \
            $(LIBDLDIR)/dl_lstadd_front_bonus.c \
            $(LIBDLDIR)/dl_lstclear_bonus.c \
            $(LIBDLDIR)/dl_lstdelone_bonus.c \
            $(LIBDLDIR)/dl_lstiter_bonus.c \
            $(LIBDLDIR)/dl_lstlast_bonus.c \
            $(LIBDLDIR)/dl_lstmap_bonus.c \
            $(LIBDLDIR)/dl_lstnew_bonus.c \
            $(LIBDLDIR)/dl_lstsize_bonus.c \
            $(LIBDLDIR)/get_next_line.c \
            $(LIBDLDIR)/get_next_line_utils.c

# Cross-platform readline paths
# macOS (Homebrew)
ifeq ($(shell uname -s),Darwin)
    READLINE_DIR = /opt/homebrew/opt/readline
    READLINE_INC = -I$(READLINE_DIR)/include -I$(SRCDIR)
    READLINE_LIB = -L$(READLINE_DIR)/lib -lreadline
else
    # Linux (Ubuntu/Debian)
    READLINE_INC = -I/usr/include -I$(SRCDIR)
    READLINE_LIB = -lreadline
endif

.PHONY: all clean fclean re bonus

all: $(NAME)

$(NAME): $(LIBDL) $(OBJ)
	$(CC) $(OBJ) $(LIBDL) $(READLINE_LIB) -o $@

$(LIBDL): $(LIBDL_SRC)
	@mkdir -p build
	@make -C $(LIBDLDIR)
	@mv $(LIBDLDIR)/libdl.a build/

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(READLINE_INC) -c $< -o $@

clean:
	@make -C $(LIBDLDIR) clean
	@rm -rf $(BUILDDIR)

fclean: clean
	@make -C $(LIBDLDIR) fclean
	@rm -f $(NAME)

re: fclean all

bonus: all
