#include "type.h"

PROC proc[NPROC], *running, *freeList, *readyList, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;

int body();
void debugStatement();
void printLists();

char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter", 
               "Saturn", "Uranus", "Neptune" };

/**************************************************
  bio.o, queue.o loader.o are in mtxlib
**************************************************/
/* #include "bio.c" */
/* #include "queue.c" */
/* #include "loader.c" */

#include "printlib.c"
#include "wait.c"             // YOUR wait.c   file
#include "kernel.c"           // YOUR kernel.c file
#include "int.c"              // YOUR int.c    file
int color;

int init() {
    PROC *p; int i;
    color = 0x0C;
    printf("init ....");
    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;  
        strcpy(proc[i].name, pname[i]);
        p->next = &proc[i+1];
    }
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyList = sleepList = 0;

    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
} 

int scheduler() {
    if (running->status == READY) {
        enqueue(&readyList, running);
    }
    running = dequeue(&readyList);
    //printf("New color: %d\n", running->pid);
    color = running->pid + 0x01;
}

int int80h();
int set_vector(u16 vector, u16 handler) {
    // put_word(word, segment, offset) in mtxlib
    put_word(handler, 0, vector<<2);
    put_word(0x1000,  0,(vector<<2) + 2);
}

main() {
    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    set_vector(80, int80h);

    kfork("/bin/u1");     // P0 kfork() P1

    while(1) {
        printf("P0 running\n");
        while(!readyList);
        printf("P0 switch process\n");
        tswitch();         // P0 switch to run P1
    }
}

int body() {
    char c;
    printf("proc %d resumes to body()\n", running->pid);
    
    while(1) {
	printLists();
	printf("proc %d[%d] running: parent=%d\n",
	       running->pid, running->priority, running->ppid);
	
	printf("enter a char [Debug|Fork|Umode|Ps|Quit|Sleep|Wait] : ");
	c = getc(); printf("%c\n", c);
	switch(c) {
	    case 's' : tswitch();   break;
	    case 'f' : kfork();     break;
	    case 'q' : kexit();     break; 
	    case 'p' : kps();       break; 
	    case 'w' : kwait();     break;
	    case 'd' : debugStatement();	break;
            case 'u' : goUmode();   break;
	}
    }
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
		kps();
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
