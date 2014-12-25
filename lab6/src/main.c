
#include "../include/forkexec.h"
#include "../include/int.h"
#include "../include/kernel.h"
#include "../include/pipe.h"
#include "../include/printlib.h"
#include "../include/type.h"
#include "../include/wait.h"

extern int body();
extern int int80h();

// PROC info
PROC proc[NPROC], *running, *freeList, *readyList, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;
char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter", 
               "Saturn", "Uranus", "Neptune" };

OFT  oft[NOFT];		// File descriptors
PIPE pipe[NPIPE];	// Pipes

/**
 * Initializes the system and sets first PROC
 * to run.
 */
int init() {
    PROC *p;
    int i, j;
    printf("init ....");
    for (i=0; i<NPROC; i++) {   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;  
        strcpy(proc[i].name, pname[i]);
        p->next = &proc[i+1];
	
        for (j=0; j<NFD; j++) {
	    p->fd[j] = 0;
        }
    }
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyList = sleepList = 0;
    
    for (i=0; i<NOFT; i++) {
        oft[i].refCount = 0;
    }
    for (i=0; i<NPIPE; i++) {
        pipe[i].busy = 0;
    }
    
    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
} 

/**
 * Queues running PROC and runs next PROC.
 */
int scheduler() {
    if (running->status == RUNNING) {
       running->status = READY;
       enqueue(&readyList, running);
    }
    running = dequeue(&readyList);
    running->status = RUNNING;
}

/**
 * Sets vector address for syscalls.
 * @param Vector number
 * @param Address of function to run
 */
int set_vec(u16 vector, u16 addr) {
    u16 location,cs;
    location = vector << 2;
    put_word(addr, 0, location);
    put_word(0x1000,0,location+2);
}

/**
 * Main function (not entry point)
 */
main() {
    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    set_vec(80,int80h);
    
    kfork("/bin/u1");     // P0 kfork() P1
    while(1) {
	printf("P0 running\n");
	if (nproc==2 && proc[1].status != READY) {
	    printf("no runable process, system halts\n");
	}
	while(!readyList);
	printf("P0 switch process\n");
	tswitch();   // P0 switch to run P1
    }
}
