#ifndef WAIT_H
#define WAIT_H

#include "type.h"

PROC *dequeue(PROC **queue);
PROC *dequeueProc(PROC **queue, PROC *p);
PROC **enqueue(PROC **queue, PROC *ptr);

#endif
