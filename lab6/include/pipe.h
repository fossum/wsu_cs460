#ifndef PIPE_H
#define PIPE_H

#include "type.h"

int show_pipe(PIPE *ptr);
int pfd();
int read_pipe(int ifd, char *buf, int n);
int write_pipe(int ifd, char *buf, int n);
int kpipe(int pd[2]);
int close_pipe(int fd);

#endif
