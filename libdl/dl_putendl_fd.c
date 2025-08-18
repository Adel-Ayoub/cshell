#include "libdl.h"

void dl_putendl_fd(char *s, int fd)
{
    dl_putstr_fd(s, fd);
    dl_putchar_fd('\n', fd);
}
