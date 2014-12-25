#include "types.h"

PROC proc[NPROC], *running, *freeList, *readyList, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;
int color;

void do_exit();
int do_ps();
void do_wait();
void printList(char *str, PROC *list);
void printLists();
void printProc(PROC *p);
int body();

void debugStatement() {
    int flag = 1;
    char c;
    
    while (flag) {
	printf("Debug Point (l, n, p, num): ");
	c = getc();
	printf("\n");
	switch(c) {
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
		printProc(&proc[c-'0']);
		break;
	    case 'n':
		printf("nproc: %d\n", nproc);
		break;
	    case 'p':
		do_ps();
		break;
	    case 'l':
		printLists();
		break;
	    default:
		flag = 0;
		break;
	}
    }
}

void printProc(PROC *p) {
    printf("*** Process Info ***\n");
    printf("Address: 0x%x Next: 0x%x Stack: 0x%x\n", p, p->next, p->ksp);
    printf("PID: %d Status: %d Priority: %d\n", p->pid, p->status, p->priority);
    printf("Parent: %d Address: 0x%x\n", p->ppid, p->parent);
    printf("********************\n");
}

int ksleep(int event) {
    running->event = event;      // Record event in PROC
    running->status = SLEEP;     // mark itself SLEEP
    
    //printf("SleepList Debug (before)\n");
    //debugStatement();
    running->next = 0;
    if (sleepList == 0) {
	sleepList = running;
    } else {
	PROC *ptr = sleepList;
	while (ptr->next != 0) { ptr->next; }
	ptr->next = running;
    }
    //printf("SleepList Debug (after)\n");
    //debugStatement();
    
    tswitch();
}

void kwakeup(int event) {
    int i = 0;
    PROC *ptr;
    
    //printf("kwakeup debug: \n");
    //debugStatement();
    if (sleepList == 0) { return; } 
    
    printf("Search for wake-up event 0x%x ... \n", event);
    while (i < NPROC) {
	ptr = &proc[i];
	//printf("S: %d, E: 0x%x\n", ptr->status, ptr->event);
	if (ptr->status==SLEEP && ptr->event==event) {
	    printf("Event found, waking PROC %d\n", ptr->pid);
	    //printLists();
	    dequeue(&sleepList, ptr);	// ERROR HERE !!! Does not dequeue ptr, only first in list!!!
	    ptr->status = READY;
	    enqueue(&readyList, ptr);
	}
	
	i++;
    }
    //printf("\n");
}

int hasChildren(PROC *ptr) {
    int i;
    
    //printf("Finding children ... ");
    for (i=1; i<NPROC; i++) {
	if (proc[i].ppid == ptr->pid &&
	    proc[i].status != FREE) {
	    //printf("found %d\n", i);
	    return(&proc[i]);
	}
    }
    
    //printf("\n");
    return(0);
}

int kexit(int exitvalue) {
    int i, wake_p1 = 0;
    PROC *ptr;
    
    running->exitCode = exitvalue;
    
    // Make sure P1 does not die if other procs still exist.
    if (running->pid == 1) {
	printf("waiting for running procs ... \n");
	while ((ptr = hasChildren(running)) != 0) { do_wait(ptr); }
	printf(" ... all P1 children done\n");
    }
    
    // Give away children (dead or alive) to P1.
    //printf("Finding children:\n");
    for (i=2; i<NPROC; i++) {
	ptr = &proc[i];
	
	if (running->pid == ptr->ppid) {
	    printf("Giving %d to P1\n", ptr->pid);
	    ptr->ppid = 1;
	    ptr->parent = &proc[1];
	    wake_p1 = 1;
	}
    }
    
    // Issue wakeup(parent) to wake up its parent
    printf("Waking parent %d at 0x%x\n", running->ppid, running->parent);
    kwakeup(running->parent);
    
    // Wake up P1 also if it has sent any children to P1
    if (wake_p1 &&
	running->ppid != 1) {
	printf("Also waking P1\n");
	kwakeup(&proc[1]);
    }
    
    running->status = ZOMBIE;
    //printf("I'm a zombie...\n");
    //debugStatement();
    tswitch();
}

int kwait(int *status) {
    int cid;
    PROC *ptr;
    
    if (hasChildren(running) == 0) {
	printf("Warning: No children.\n");
	return(-1);
    }
    
    //printf("Finding zombies...\n");
    while (1) {
	for (cid=1; cid<NPROC; cid++) {
	    ptr = &proc[cid];
	    
	    // find ZOMBIE child
	    if (ptr->status == ZOMBIE &&
		ptr->ppid == running->pid) {
		//printf("Found zombie %d\n", ptr->pid);
		
		//copy child's exitValue to *status
		*status = ptr->exitCode;
		
		//free the ZOMBIE child PROC
		ptr->status = FREE;
		ptr->ppid = 0;
		ptr->parent = 0;
		ptr->priority = 0;
		ptr->exitCode = 0;
		enqueue(&freeList, ptr);
		
		//return dead child's pid
		return(cid);
	    }
	}
	
	//printf("sleeping...\n");
	ksleep(running);
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
    
    enqueue(&readyList, ptr);
    /*printf("Ready queue:\n");
    print_queue(readyList);*/
    
    return(ptr);
}

int init() {
    PROC *p;
    int i;
    
    printf("init ... ");
    
    for (i=0; i<NPROC; i++){   // initialize all procs
	p = &proc[i];
	p->pid = i;
	p->ppid = 0;
	p->status = FREE;
	p->priority = 0;     
	p->next = &proc[i+1];
    }
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyList = sleepList = 0;
    
    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = READY;
    running = p;
    nproc++;
    printf("done\n");
} 

int scheduler() {
    printf("Scheduler\n");
    debugStatement();
    if (running->status == READY) {
	enqueue(&readyList, running);
    }
    
    running = dequeue(&readyList);
    color = 0x000A + (running->pid % 6);
}

main() {
    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    kfork();     // P0 kfork() P1
    
    while(1) {
	printf("P0 running\n");
	//printf("Main loop (nproc == %d)\n", nproc);
	//debugStatement();
	if (nproc == 1 && proc[1].status != READY) {
	    printf("no runable process, system halts\n");
	}
	while(!readyList);
	//{
	//    printf("!readyList\n");
	//    debugStatement();
	//}
	
	// Debung statements
	printf("P0 switch process\n");
	//debugStatement();
	//tswitch();   // P0 switch to run P1
	do_wait();
    }
}

void do_exit() {
    char exitStr[4];
    int exitCode;
    /**
     * When a proc sees 'q' command, ask for an exit value:
     *   enter an exitValue:
     *   write YOUR gets(char s[ ]) function to return s;
     *   int exitValue = atoi(gets(s[8]);
     * Then, call kexit(exitValue) to DIE:
     */
    printf("Exit code? ");
    gets(exitStr);
    exitCode = atoi(exitStr);
    kexit(exitCode);
}

void do_wait() {
    int status;
    int cid;
    
    while ((cid = kwait(&status)) != -1) {
        printf("Done waiting for %d, returned status %d\n", cid, status);
	//debugStatement();
    }
}

int do_ps() {
    int i;
    
    printf("=========================================\n");
    printf("  name   status  pid  ppid Address Next  \n");
    printf("-----------------------------------------\n");
    for (i=0; i<NPROC; i++) {
	printf("         ");
	switch(proc[i].status) {
	    case FREE:
		printf("FREE   ");
		break;
	    case READY:
		printf("READY  ");
		break;
	    case RUNNING:
		printf("RUNNING");
		break;
	    case STOPPED:
		printf("STOPPED");
		break;
	    case SLEEP:
		printf("SLEEP  ");
		break;
	    case ZOMBIE:
		printf("ZOMBIE ");
		break;
	    default:
		printf("Error  ");
		break;
	}
	//if (proc[i].status != FREE) {
	    printf(" %d   %d   0x%x 0x%x",
		   proc[i].pid, proc[i].ppid, &proc[i], proc[i].next);
	//}
	printf("\n");
    }
    printf("-----------------------------------------\n");
    printf("Ready: 0x%x Free: 0x%x Sleep: 0x%x\n", readyList, freeList, sleepList);
    printf("-----------------------------------------\n");
}

void printList(char *str, PROC *list) {
    PROC *head, *ptr;
    head = list;
    ptr = list;
    
    printf("%s = ", str);
    while (1) {
	if (ptr == 0) {
	    printf("NULL\n");
	    return;
	} else {
	    printf("%d[", ptr->pid);
	    if (ptr->event) {
		printf("event=0x%x]", ptr->event);
	    } else {
		printf("%d]", ptr->ppid);
	    }
	    if (ptr->next != head) {
		printf("->");
	    }
	    ptr = ptr->next;
	}
    }
}

void printLists() {
    printf("-----------------------------------------\n");
    printList("freelist  ", freeList);
    printList("readyList", readyList);
    printList("sleepList ", sleepList);
    printf("-----------------------------------------\n");
}

int body() {
    char c;
    printf("proc %d resumes to body()\n", running->pid);
    
    while(1) {
	printLists();
	printf("proc %d[%d] running: parent=%d\n",
	       running->pid, running->priority, running->ppid);
	
	printf("enter a char [Sleep|Fork|Quit|Ps|Wait|Debug] : ");
	c = getc(); printf("%c\n", c);
	switch(c) {
	    case 's' : tswitch();   break;
	    case 'f' : kfork();     break;
	    case 'q' : do_exit();   break; 
	    case 'p' : do_ps();     break; 
	    case 'w' : do_wait();   break;
	    case 'd' : debugStatement();	break;
	}
    }
}
