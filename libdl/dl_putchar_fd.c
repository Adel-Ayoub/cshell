#include "libdl.h"

void dl_putchar_fd(char c, int fd)
{
    write(fd, &c, 1);
}
