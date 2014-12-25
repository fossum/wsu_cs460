#ifndef PRINTLIB_H
#define PRINTLIB_H

#include <stdarg.h>
#include "type.h"

#define putchar(c) (putc(c))

char *gets(char buf[]);
void printf(char *string, ...);
char *tokenize(char separator, char *str);
int strcpy(char *dest, char *src);
int strncpy(char *dest, char *src, int length);

#endif
