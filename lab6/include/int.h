#ifndef INT_H
#define INT_H

#include "type.h"

#define PA 13
#define PB 14
#define PC 15
#define PD 16
#define AX  8

int chname(char *name);
int do_exit(int value);
int do_ps();
int do_show_pipe(int fd_num);
int do_wait(int status);
int get_str(char *str, char *buf, int size);
int kcinth();
int kmode();

#endif
