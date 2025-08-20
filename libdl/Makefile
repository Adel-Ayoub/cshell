CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
NAME = libdl.a
SRCDIR = .
BUILDDIR = obj

# Source files
SRC = dl_atoi.c dl_bzero.c dl_calloc.c dl_isalnum.c dl_isalpha.c dl_isascii.c \
      dl_isdigit.c dl_isprint.c dl_itoa.c dl_memchr.c dl_memcmp.c dl_memcpy.c \
      dl_memmove.c dl_memset.c dl_putchar_fd.c dl_putendl_fd.c dl_putnbr_fd.c \
      dl_putstr_fd.c dl_split.c dl_strchr.c dl_strcmp.c dl_strdup.c dl_striteri.c \
      dl_strjoin.c dl_strlcat.c dl_strlcpy.c dl_strlen.c dl_strmapi.c \
      dl_strncmp.c dl_strncpy.c dl_strnstr.c dl_strrchr.c dl_strtok.c dl_strtrim.c dl_substr.c \
      dl_tolower.c dl_toupper.c dl_lstadd_back_bonus.c dl_lstadd_front_bonus.c \
      dl_lstclear_bonus.c dl_lstdelone_bonus.c dl_lstiter_bonus.c \
      dl_lstlast_bonus.c dl_lstmap_bonus.c dl_lstnew_bonus.c dl_lstsize_bonus.c \
      get_next_line.c get_next_line_utils.c

# Object files
OBJ = $(SRC:%.c=$(BUILDDIR)/%.o)

.PHONY: all clean fclean re bonus

all: $(NAME)

$(NAME): $(OBJ)
	ar rcs $@ $^

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILDDIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

bonus: all
