#ifndef MY_ASSERT_H
#define MY_ASSERT_H

#define assert(expression) _assert(expression, __FILE__, __LINE__)

void _assert(int expression, char *file, int line);

#endif
