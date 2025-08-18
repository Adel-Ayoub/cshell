#include "libdl.h"

int dl_isalnum(int c)
{
    return (dl_isalpha(c) || dl_isdigit(c));
}
