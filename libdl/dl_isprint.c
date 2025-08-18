#include "libdl.h"

int dl_isprint(int c)
{
    return (c >= 32 && c <= 126);
}
