/*************************************************************************
  -24 -22 -20 -18 -16 -14 -12 -10 -8  -6  -4   -2
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

#include "../include/int.h"
#include "../include/my_assert.h"

extern PROC *running, *freeList, *readyList, *sleepList;
extern PROC proc[];

/**
 * INT-80 vector address. Used to handle any
 * syscall from user-space. Params are poped
 * from user-space. Returns are placed in
 * user-space as well.
 */
int kcinth() {
    u16    segment, offset;
    int    a, b, c, d, r;
   
    segment = running->uss; 
    offset = running->usp;
    
    a = get_word(segment, offset + 2*PA);
    b = get_word(segment, offset + 2*PB);
    c = get_word(segment, offset + 2*PC);
    d = get_word(segment, offset + 2*PD);
    
    assert(a > 99 || a < 0 ||
	   (a > 8 && a < 30) ||
	   (a > 35 && a < 90));
 
    switch(a) {
        case 0 : r = running->pid;          break;
        case 1 : r = do_ps();               break;
        case 2 : r = chname(b);             break;
        case 3 : r = kmode();               break;
        case 4 : r = tswitch();             break;
        case 5 : r = do_wait(b);            break;
        case 6 : r = do_exit(b);            break;
        case 7 : r = fork();                break;
        case 8 : r = exec(b);               break;
        
        /****** these are YOUR pipe functions ************/
        case 30 : r = kpipe(b);             break;
        case 31 : r = read_pipe(b,c,d);     break;
        case 32 : r = write_pipe(b,c,d);    break;
        case 33 : r = close_pipe(b);        break;
        case 34 : r = pfd();                break;
        case 35 : r = do_show_pipe(b);      break;
        /**************** end of pipe functions ***********/
        
        case 90: r =  getc();               break;
        case 91: r =  putc(b);              break;
	case 98: debugStatement();          break;
        case 99: do_exit(b);                break;
        default: printf("invalid syscall # : %d\n", a); 
    }
    
    put_word(r, segment, offset + 2*AX);
    return(r);
}

/**
 * Show details of the pipe attached to the
 * requested file descriptor.
 * @param File descriptor index in current PROC
 */
int do_show_pipe(int fd_num) {
    assert(running->fd[fd_num] == 0);
    return(show_pipe(running->fd[fd_num]->pipe_ptr));
}

/**
 * Gets a string from user-space virtual address.
 * @param User-space virtual address
 * @param Buffer to copy string into
 * @param Max size of string (zero for no max)
 * @return Number of characters (wo/NULL)
 */
int get_str(char *str, char *buf, int size) {
    int i = 0;
    
    assert(str == 0 || buf == 0);
    assert(size < 0 || size > 1024);
    
    while (size == 0 || i < size) {
        *(buf + i) = get_byte(running->uss, str + i);
        if (*(buf + i) == '\0') {
            break;
        }
        i++;
    }
    
    return(i);
}

/**
 * Print PROC list with names, addresses, next
 * PROC, etc.
 */
int do_ps() {
    int i;
    
    printf("=========================================\n");
    printf("status  pid  ppid Address Next   name    \n");
    printf("-----------------------------------------\n");
    for (i=0; i<NPROC; i++) {
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
	if (proc[i].status != FREE) {
	    printf(" %d    %d    0x%x  0x%x %s",
		   proc[i].pid, proc[i].ppid, &proc[i], proc[i].next, proc[i].name);
	}
	printf("\n");
    }
    printf("-----------------------------------------\n");
    printf("Ready: 0x%x Free: 0x%x Sleep: 0x%x\n", readyList, freeList, sleepList);
    printf("-----------------------------------------\n");
}

/**
 * Wait for zombie child. If found places exit code
 * in status and returns child index. Sleeps on running
 * children, otherwise exits.
 * @param Virtual address to status variable
 * @return Freed zombie child index
 */
int do_wait(int status) {
    int ret, code;
    
    assert(status >= 0);
    ret = kwait(&code);
    
    put_word(code, running->uss, status);
    return(ret);
}

/**
 * Kill current PROC with some exit value.
 * @param Integer exit value
 */
int do_exit(int value) {
    return(kexit(value));
}

/**
 * Switch to kernel mode execution.
 */
int kmode() {
    char c = '\0';
    printf("Jump to kmode()\n");
    
    while(c != 'u') {
	printLists();
	printf("proc %d[%d] running: parent=%d\n",
	       running->pid, running->priority, running->ppid);
	
	printf("enter a char [Debug|Fork|Umode|Ps|Quit|Sleep|Wait] : ");
	c = getc(); printf("%c\n", c);
	switch(c) {
	    case 'd' : debugStatement();   break;
	    case 's' : tswitch();   break;
	    case 'f' : kfork(0);    break;
	    case 'q' : kexit();     break; 
	    case 'p' : do_ps();     break; 
	    case 'w' : kwait();     break;
            case 'u' :
	    default  : break;
	}
    }
    
    return(0);
}

/**
 * Change current PROC name
 * @param Virtual address pointer to name
 */
int chname(char *name) {
    char c;
    int i = 0;
    
    assert(name == 0);
    
    while (i < 32) {
        c = get_byte(running->uss, name + i);
        running->name[i++] = c;
	if (i == 32) {
	    running->name[31] = '\0';
	}
        if (c == '\0') {
            break;
        }
    }
    
    return(0);
}
