#include "libdl.h"

void dl_putstr_fd(char *s, int fd)
{
    if (!s)
        return;
    write(fd, s, dl_strlen(s));
}
