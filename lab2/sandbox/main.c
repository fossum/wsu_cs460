
#define NPROC     9
#define SSIZE  1024

typedef struct proc{
    struct proc *next;   
           int  ksp;               /* saved sp; offset = 2 */
           int  pid;
           int  status;            /* READY|DEAD, etc */
           int  kstack[SSIZE];     // kmode stack of task
}PROC;

int scheduler() { return(0); }

PROC proc[NPROC], *running;
int  procSize = sizeof(PROC);
int  color = 0x0C;

void main() {
    printf("Hello World%c _d: %s", '!', " string ");

    getc();

    return;
}
