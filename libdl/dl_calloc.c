#include "libdl.h"

void *dl_calloc(size_t count, size_t size)
{
    void *ptr;
    size_t total_size;

    total_size = count * size;
    if (count != 0 && total_size / count != size)
        return (NULL);
    ptr = malloc(total_size);
    if (ptr)
        dl_bzero(ptr, total_size);
    return (ptr);
}
