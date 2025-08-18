#include "libdl.h"

void dl_putnbr_fd(int n, int fd)
{
    if (n == -2147483648)
    {
        dl_putstr_fd("-2147483648", fd);
        return;
    }
    
    if (n < 0)
    {
        dl_putchar_fd('-', fd);
        n = -n;
    }
    
    if (n >= 10)
        dl_putnbr_fd(n / 10, fd);
    
    dl_putchar_fd(n % 10 + '0', fd);
}
