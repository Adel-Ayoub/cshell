#include "libdl.h"

void dl_bzero(void *s, size_t n)
{
    dl_memset(s, 0, n);
}
