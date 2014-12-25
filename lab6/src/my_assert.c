
#include "../include/my_assert.h"

void _assert(int expression, char *file, int line)
{
    if (expression) {
        printf(" *** Assert in %s, Line: %d ***\n", file, line);
    }
}
