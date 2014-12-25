#ifndef KERNEL_H
#define KERNEL_H

#include "type.h"

PROC *kfork(char *filename);
int ksleep(int event);
void printProc(PROC *p);
void kwakeup(int event);
int kexit(int exitvalue);
int kwait(int *status);
void printLists();
void debugStatement();

#endif
