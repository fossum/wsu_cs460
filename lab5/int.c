
/*************************************************************************
  -24 -22 -20 -18 -16 -14 -12 -10 -8  -6  -4   -2
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

char tmp_str[100];

/****************** syscall handler in C ***************************/
int kcinth() {
   int a, b, c, d, r;
   int segment = running->uss;
   
   // CODE TO GET get syscall parameters a,b,c,d from ustack
   a = get_word(segment, running->usp + 26);
   b = get_word(segment, running->usp + 28);
   c = get_word(segment, running->usp + 30);
   d = get_word(segment, running->usp + 32);
   
   //if (a > 3 && a < 10) {
   //   printf("Before (%u, %x, %u, %u) -> %d\n", a, b, c, d, r);
   //   printProc(running);
   //}
   
   switch(a) {
      case 0 : r = kgetpid();        break;
      case 1 : r = kps();            break;
      case 2 : r = kchname(b);       break;
      case 3 : r = kkfork();         break;
      case 4 : r = ktswitch();       break;
      case 5 : r = kkwait(b);        break;
      case 6 : r = kkexit(b);        break;
      case 7 : r = kdebug();         break;
      case 8 : r = kexec(b);         break;
      
      case 90: r = getc();           break;
      case 91: r = putc(b);          break;
      case 99: kkexit(b);            break;
      default: printf("invalid syscall # : %d\n", a); 
   }

   // CODE to let r be the return value to Umode
   put_word(r, segment, running->usp + 16);
   
   //if (a > 3 && a < 10) {
   //   printf("After (%d, %d, %d, %d) -> %d\n", a, b, c, d, r);
   //   printProc(running);
   //}
   
   return(r);
}

char *get_str(char *str) {
    int i = 0;
    
    while (i < sizeof(tmp_str)) {
        tmp_str[i] = get_byte(running->uss, str + i);
        if (tmp_str[i] == '\0') {
            break;
        }
        i++;
    }
    
    return(tmp_str);
}

int kexec(char *filename) {
    int i, reg;
    char *str = get_str(filename);
    
    // Load new uMode image
    load(str, running->uss);
    
    // Reset uStack
    for (i=-2; i>=-24; i-=2) {
        switch(i) {
            case -2:    reg = 0x0200; break;
            case -4:
            case -22:
            case -24:   reg = running->uss; break;
            default:    reg = 0; break;
        }
        put_word(reg, running->uss, i);
    }
    
    running->usp = -24;
    return(0);
}

int kgetpid() {
   // WRITE YOUR C code
   return(running->pid);
}

int kps() {
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
	//if (proc[i].status != FREE) {
	    printf(" %d    %d    0x%x  0x%x %s",
		   proc[i].pid, proc[i].ppid, &proc[i], proc[i].next, proc[i].name);
	//}
	printf("\n");
    }
    printf("-----------------------------------------\n");
    printf("Ready: 0x%x Free: 0x%x Sleep: 0x%x\n", readyList, freeList, sleepList);
    printf("-----------------------------------------\n");
}

int kdebug() {
    return(0);
}

int kchname(char *name) {
    char c;
    int i = 0;
    
    while (i < 32) {
        c = get_byte(running->uss, name + i);
        running->name[i] = c;
        if (c == '\0') {
            break;
        }
        i++;
    }
   
    return(0);
}

int kkfork() {
    PROC *ptr;
    
    // Get free proc, return if none
    ptr = kfork();
    if (ptr == 0) {
        printf("Failed to kkfork()\n");
        getc();
        return(0);
    }
    
    printf("Return PID: %u\n", ptr->pid);
    return(ptr->pid);
}

int ktswitch() {
    return tswitch();
}

int kkwait(int *status) {
    int ret, code;
    ret = kwait(&code);
    
    put_word(code, running->uss, status);
    return(ret);
}

int kkexit(int value) {
    return(kexit(value));
}
