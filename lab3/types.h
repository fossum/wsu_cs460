#ifndef TYPES_H
#define TYPES_H

#define NPROC    9
#define SSIZE 1024

/******* PROC status ********/
#define FREE     0
#define READY    1
#define RUNNING  2
#define STOPPED  3
#define SLEEP    4
#define ZOMBIE   5

typedef struct proc {
    struct proc *next;
    int    *ksp;
    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
    int    ppid;               // parent pid
    struct proc *parent;
    int    priority;
    int    event;
    int    exitCode;
    int    kstack[SSIZE];      // per proc stack area
} PROC;

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef char            int8_t;
typedef short           int16_t;
typedef unsigned long   uint32_t;
typedef long            int32_t;

#endif