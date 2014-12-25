
void goUmode();
void printProc(PROC *p);

int copyImage(int child_segment) {
    int i, word;
    
    // Copy word for word... (word==2 bytes)
    printf("Copying 0x%x to 0x%x\n", running->uss, child_segment);
    for (i=0; i<0xFFFE; i+=2) {
	word = get_word(running->uss, i);
	put_word(word, child_segment, i);
    }
    
    return(0);
}

PROC *kfork(char *filename)
{
    int j;
    
    // Get free proc, return if none
    PROC *ptr = dequeue(&freeList);
    if (ptr == 0) {
        printf("Failed to kfork()\n");
        getc();
        return(0);
    }
    
    // Set basics
    ptr->status = READY;
    ptr->priority = 1;
    ptr->ppid = running->pid;
    ptr->parent = running;
    ptr->uss = (ptr->pid + 1) * 0x1000;
    
    // Configure kStack
    for (j=1; j<10; j++) {
        ptr->kstack[SSIZE - j] = 0;     // all saved registers = 0
    }
    ptr->kstack[SSIZE-1]=(int)goUmode;  // called tswitch() from body
    ptr->ksp = &(ptr->kstack[SSIZE-9]); // ksp -> kstack top
  
    // Configure uStack
    if (filename) {
	load(filename, ptr->uss);           // Load executable
	for (j=1; j<13; j++) {
	    put_word(0, ptr->uss, -j*2);       // Registers to 0
	}
	put_word(0x0200, ptr->uss, -2);      // Flag
	ptr->usp = -24;
    } else {
	copyImage(ptr->uss);
	ptr->usp = running->usp;
	put_word(0, ptr->uss, ptr->usp + 16);
	//printProc(running);
	//getc();
	//printProc(ptr);
	//getc();
    }
    put_word(ptr->uss, ptr->uss, ptr->usp + 20);	// CS
    put_word(ptr->uss, ptr->uss, ptr->usp + 2);		// ES
    put_word(ptr->uss, ptr->uss, ptr->usp);    		// DS
    
    // Put in readyList and return
    enqueue(&readyList, ptr);
    return(ptr);
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

void printProc(PROC *p) {
    int i, value;
    printf("*** Process Info ************************************\n");
    printf("Address: 0x%x Next: 0x%x Stack: 0x%x\n", p, p->next, p->ksp);
    printf("PID: %d Status: %d Priority: %d\n", p->pid, p->status, p->priority);
    printf("Parent: %d Address: 0x%x\n", p->ppid, p->parent);
    printf("*****************************************************\n");
    printf("usp: %d, uss: 0x%x\n", p->usp, p->uss);
    printf("Stack:\n");
    for(i=-30; i<0; i+=2) {
        value = get_word(p->uss, i);
        printf("Offset %d: 0x%x(%d)\n", i, value, value);
    }
    printf("*****************************************************\n");
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
	    dequeueProc(&sleepList, ptr);
	    ptr->status = READY;
	    enqueue(&readyList, ptr);
	}
	
	i++;
    }
    //printf("\n");
}

int kexit(int exitvalue) {
    int i, wake_p1 = 0;
    PROC *ptr;
    
    running->exitCode = exitvalue;
    
    // Make sure P1 does not die if other procs still exist.
    if (running->pid == 1) {
	printf("waiting for running procs ... \n");
	while ((ptr = hasChildren(running)) != 0) { kwait(ptr); }
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
    
    return(-1);
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
