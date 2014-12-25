/************ t.c file **********************************/
#define NPROC     9        
#define SSIZE  1024                /* kstack int size */

#define DEAD	0                  /* proc status     */
#define READY	1
#define FREE	2
#define SLEEP   3
#define BLOCK   4
#define ZOMBIE  5

typedef struct proc{
    struct proc *next;
    int    ksp;

    int    status;       // FREE|READY|SLEEP|BLOCK|ZOMBIE
    int    priority;     // priority
    int    pid;          // process pid
    int    ppid;         // parent pid 
    struct proc *parent; // pointer to parent PROC

    int    kstack[SSIZE]; // SSIZE=1024
}PROC;

PROC proc[NPROC], *running, *freeList, *readyQueue;
int  procSize = sizeof(PROC);
int  color = 0x0C;

char *gasp[NPROC]={
    "Oh! You are killing me .......",
    "Oh! I am dying ...............", 
    "Oh! I am a goner .............", 
    "Bye! Bye! World...............",      
};

int body();
PROC *dequeue(PROC **queue);
PROC **enqueue(PROC **queue, PROC *p);
PROC *kfork();
int init();
void print_proc(PROC *p);
void print_queue(PROC *queue);

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

// init
//
// Initialize the processes
int init()
{
    int i, j;
    PROC *p;
    
    for (i=0; i < NPROC; i++) {
        p = &proc[i];
        p->next = 0;
        p->pid = i;
        p->status = FREE;
        p->priority = 0;
        p->ppid = 0;
        p->parent = 0;
        
        if (i) {     // initialize kstack[ ] of proc[1] to proc[N-1]
            for (j=1; j<10; j++) {
                p->kstack[SSIZE - j] = 0;          // all saved registers = 0
            }
            p->kstack[SSIZE-1]=(int)body;          // called tswitch() from body
            p->ksp = &(p->kstack[SSIZE-9]);        // ksp -> kstack top
            enqueue(&freeList, p);
        } else {
            running = p;
            p->status = READY;
            p->parent = p;
            p->ppid = p->pid;
        }
    }
    
    readyQueue = 0;
    printf("initialization complete\n");
}

// enqueue
//
// enter p into queue by priority; 
PROC **enqueue(PROC **queue, PROC *process)
{
    PROC *prev = *queue;
    PROC *ptr = *queue;
    
    // Empty queue?
    if (*queue == 0) {
        *queue = process;
        process->next = process;
    } else {
        // Find spot
        while (ptr->priority >= process->priority) {
            ptr = ptr->next;
            if (ptr == *queue) {
                break;
            }
        }
        while (prev->next != ptr) { prev = prev->next; }

        // Insert process
        process->next = prev->next;
        prev->next = process;
        
        // Move head?
        if ((*queue)->priority < process->priority) {
            *queue = process;
        }
    }
    
    return queue;
}

// dequeue
//
// remove a PROC with the highest priority
// (the first one in queue) return its pointer;
PROC *dequeue(PROC **queue)
{
    PROC *ptr = *queue;
    
    if (*queue) {
        // Find end of list
        while ((*queue)->next != ptr) {
            *queue = (*queue)->next;
        }
        
        // Patch list
        if ((*queue)->next == *queue) {
            *queue = 0;
        } else {
            (*queue)->next = ptr->next;
            *queue = ptr->next;
        }
        
        // Erase list ref.
        ptr->next = 0;
    }
    
    return(ptr);
}

void print_queue(PROC *queue) {
    int i = 0;
    PROC *beginning = queue;
    
    do {
        printf("[%d, %d]", queue->pid, queue->priority);
        queue = queue->next;
        if (queue != beginning) {
            printf(" -> ");
        }
    } while (queue != beginning && i++ < (NPROC * 2));
    if (i == (NPROC * 2)) {
        printf(">>>> ...Runaway print ASSERT... <<<<\n");
        getc();
    }
    printf("\n");
}

void print_proc(PROC *p) {
    printf("*** Process Info ***\n");
    printf("Address: 0x%x Next: 0x%x Stack: 0x%x\n", p, p->next, p->ksp);
    printf("PID: %d Status: %d Priority: %d\n", p->pid, p->status, p->priority);
    printf("Parent: %d Address: 0x%x\n", p->ppid, p->parent);
    printf("********************\n");
}

// grave
//
// Prints dying message and kills process, then switches.
int grave() {
    printf("\n*****************************************\n"); 
    printf("Task %d %s\n", running->pid,gasp[(running->pid) % 4]);
    printf("*****************************************\n");
    running->status = DEAD;

    tswitch();   /* journey of no return */        
}

// body
// 
// Process body, so far just prints info.
int body() {
    char c;
    
    //printf("Body\n");
    while(1) {
        color = 0x0A + (running->pid % 6);
        printf("Ready queue:\n");
        print_queue(readyQueue);
        printf("I am Proc %d in body()\n", running->pid);
        printf("Input a char : [(S)witch|(Q)uit|(F)ork]\n");
        c=getc();
        switch(c) {
            case 's': tswitch();    break;
            case 'q': grave();      break;
            case 'f': kfork();      break;
            default :               break;
        }
    }
}

// main
//
// start and end point
void main() {
    int i = 0;
    
    printf("\n");
    printf("Welcome to the 460 Multitasking System\n");
    init();

    printf("P0 switch to P1\n");
    kfork();
    while(1) {              // loop forever
        if (readyQueue) {
            tswitch();      // switch process
        }
    }
    
    printf("Will never get here...\n");
}

// scheduler
//
// Grabs the next ready process
int scheduler() {
    PROC *p;
    
    //printf("Scheduler\n");
    if (running->status == READY) {
        enqueue(&readyQueue, running);
    }
    //printf("Finding process ... ");
    running = dequeue(&readyQueue);
    //printf("found one\n");

    if (running == 0) {
       running = &proc[0];
    }

    printf("scheduler: next running proc = %d\n", running->pid);

}
