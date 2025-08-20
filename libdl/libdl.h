#ifndef LIBDL_H
#define LIBDL_H

#include <stdlib.h>
#include <unistd.h>

// Memory functions
void *dl_memset(void *b, int c, size_t len);
void dl_bzero(void *s, size_t n);
void *dl_memcpy(void *dst, const void *src, size_t n);
void *dl_memmove(void *dst, const void *src, size_t len);
void *dl_memchr(const void *s, int c, size_t n);
int dl_memcmp(const void *s1, const void *s2, size_t n);
void *dl_calloc(size_t count, size_t size);

// String functions
size_t dl_strlen(const char *s);
char *dl_strdup(const char *s1);
char *dl_strcpy(char *dst, const char *src);
char *dl_strncpy(char *dst, const char *src, size_t len);
size_t dl_strlcpy(char *dst, const char *src, size_t dstsize);
size_t dl_strlcat(char *dst, const char *src, size_t dstsize);
char *dl_strchr(const char *s, int c);
char *dl_strrchr(const char *s, int c);
int dl_strncmp(const char *s1, const char *s2, size_t n);
int dl_strcmp(const char *s1, const char *s2);
char *dl_strnstr(const char *haystack, const char *needle, size_t len);
char *dl_strjoin(char const *s1, char const *s2);
char *dl_strtrim(char const *s1, char const *set);
char **dl_split(char const *s, char c);
char *dl_substr(char const *s, unsigned int start, size_t len);
char *dl_strmapi(char const *s, char (*f)(unsigned int, char));
void dl_striteri(char *s, void (*f)(unsigned int, char*));

// Character functions
int dl_isalpha(int c);
int dl_isdigit(int c);
int dl_isalnum(int c);
int dl_isascii(int c);
int dl_isprint(int c);
int dl_toupper(int c);
int dl_tolower(int c);

// Conversion functions
int dl_atoi(const char *str);
char *dl_itoa(int n);

// Output functions
void dl_putchar_fd(char c, int fd);
void dl_putstr_fd(char *s, int fd);
void dl_putendl_fd(char *s, int fd);
void dl_putnbr_fd(int n, int fd);

// Linked list functions
typedef struct s_list
{
    void *content;
    struct s_list *next;
} t_list;

t_list *dl_lstnew(void *content);
void dl_lstadd_front(t_list **lst, t_list *new);
int dl_lstsize(t_list *lst);
t_list *dl_lstlast(t_list *lst);
void dl_lstadd_back(t_list **lst, t_list *new);
void dl_lstdelone(t_list *lst, void (*del)(void*));
void dl_lstclear(t_list **lst, void (*del)(void*));
void dl_lstiter(t_list *lst, void (*f)(void *));
t_list *dl_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *));

#endif
