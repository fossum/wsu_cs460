
#include "my_assert.h"

void _assert(int expression, char *file, int line)
{
    if (expression) {
        fprintf(STDERR, " *** Assert in %s, Line: %d ***\n", file, line);
        while(1) { getc(); }
    }
}
