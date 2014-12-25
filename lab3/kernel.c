#include "types.h"

extern struct PROC *running;

int ksleep(int event) {
    running->event = event;      // Record event in PROC
    running->status = SLEEP;     // mark itself SLEEP
    
    // For fairness, put running into a FIFO
    // sleepList so that they will wakeup in order
    enqueue(&sleepList, running);
    
    tswitch();                   // not in readyQueue anymore
}

int kwakeup(int event) {
    int i = 0;
    PROC ptr = proc;
    
    while (i < NPROC) {
	ptr = &proc[i];
	
	if (ptr.status==SLEEP && ptr.event==event) {
	    // remove p from sleepList if you implement a sleepList
	    dequeue(&sleepList, p);
	    // make it READY
	    p->status = READY;
	    // enter p into readyQueue
	    enqueue(&readyQueue, p);
	}
	
	i++;
    }
}

int kexit(int exitvalue) {
    /**
     * A process is born in kerenl. It must DIE in kernel.
     *
     * ADD TWO more fields to the PROC structure:
     *   int event;        // for proc to sleep on a value
     *   int exitValue;    // proc's exit value in LOW byte = 0-255
     *
     * When a proc sees 'q' command, ask for an exit value:
     *   enter an exitValue:
     *   write YOUR gets(char s[ ]) function to return s;
     *   int exitValue = atoi(gets(s[8]);
     * Then, call kexit(exitValue) to DIE:
     *
     *         Algorithm of kexit(value):
     * Record value in its PROC.exitValue;
     * Give away children (dead or alive) to P1.
     * Make sure P1 does not die if other procs still exist.
     * Issue wakeup(parent) to wake up its parent;
     *   Wake up P1 also if it has sent any children to P1;
     * Mark itself a ZOMBIE;
     * Call tswitch(); to give up CPU;
     *
     * When a proc dies, its PROC is marked as ZOMBIE, contains an exit value, but
     * the PROC is NOT yet freed. It will be freed by the parent via the wait()
     * operation.
     **/
}

int kwait(int *status) {
    if (no child) {
	return -1 for ERROR;
    }
    
    while(1) {
	if (found a ZOMBIE child) {
	    //copy child's exitValue to *status;
	    //save its pid;
	    //free the ZOMBIE child PROC (enter it into freeList);
	    //return dead child's pid;
	}
	sleep(running);    // sleep at its own &PROC
    }
}

PROC *kfork() {
    int j;
    PROC *ptr = dequeue(&freeList);
    
    if (ptr == 0) {
        printf("Failed to kfork()\n");
        getc();
        return(0);
    }

    ptr->status = READY;
    ptr->priority = 1;
    ptr->ppid = running->pid;
    ptr->parent = running;

    /*
     * INITIALIZE p's kstack to make it start from body() when it runs.
     * To do this, PRETNED that the process called tswitch() from the 
     * the entry address of body() and executed the SAVE part of tswitch()
     * to give up CPU before. 
     * Initialize its kstack[ ] and ksp to comform to these.
     */
    for (j=1; j<10; j++) {
        ptr->kstack[SSIZE - j] = 0;          // all saved registers = 0
    }
    ptr->kstack[SSIZE-1]=(int)body;          // called tswitch() from body
    ptr->ksp = &(ptr->kstack[SSIZE-9]);      // ksp -> kstack top
    
    //print_proc(ptr);
    
    enqueue(&readyQueue, ptr);
    /*printf("Ready queue:\n");
    print_queue(readyQueue);*/
    
    return(ptr);
}
