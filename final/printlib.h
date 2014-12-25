#ifndef PRINTLIB_H
#define PRINTLIB_H

#include <stdarg.h>
#include "type.h"

#define putchar(f, c) (writechar(f, c))

void writechar(int fd, char c);
void fprintf(int fd, char *str, ...);
void printf(char *string, ...);
void my_print(char *string, va_list args);

#endif
