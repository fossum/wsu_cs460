#include "ucode.h"

// Functions user can call (order matters)
char *cmd[]={"getpid", "ps", "chname", "kmode", "switch", "wait", "exit", 
             "fork", "exec", "pipe", "pfd", "read", "write", "close",
             "show_p", "debug", 0};
// File descriptors
int pd[2] = {1,2};

/**
 * Prints menu of commands for user to execute
 */
int show_menu() {
    printf("******************** Menu ***************************\n");
    printf("*  ps   chname kmode switch wait  exit   fork  exec *\n");
           //  1    2      3     4      5     6      7     8 
    printf("*  pipe pfd    read  write  close show_p debug      *\n");
           //  9    10     11    12     13    14     15
    printf("*****************************************************\n");
}

/**
 * Find user entered command in cmd array.
 * @param Command string
 * @return Command index
 */
int find_cmd(char *name) {
    int i = 0;   
    char *p = cmd[0];
    
    while (p){
        if (!strcmp(p, name)) {
            return i;
        }
        p = cmd[++i];
    }
    return(-1);
}

/**
 * Call kernel mode debug command.
 */
int debug() {
    syscall(98, 0, 0);
}

/**
 * Gets current PID
 * @return Current PROC PID
 */
int getpid() {
    return syscall(0,0,0);
}

/**
 * Print PROC list with details
 */
int ps() {
    return syscall(1, 0, 0);
}

/**
 * Call kernel mode to change PROC name.
 */
int chname() {
    char s[64];
    printf("input new name : ");
    gets(s);
    return syscall(2, s, 0);
}

/**
 * Switch to old kernel mode style.
 */
int kmode() {
    printf("kmode : enter Kmode via INT 80\n");
    printf("proc %d going K mode ....\n", getpid());
    syscall(3, 0, 0);
    printf("proc %d back from Kernel\n", getpid());
}    

/**
 * Give up processor and switch to
 * next PROC.
 */
int kswitch() {
    printf("proc %d enter Kernel to switch proc\n", getpid());
    syscall(4,0,0);
    printf("proc %d back from Kernel\n", getpid());
}

/**
 * Wait for a child to die, if none currently
 * sleep till one available.
 * @return Collected child PID
 */
int wait() {
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0) {
        printf("exitValue=%d", exitValue);
    }
    printf("\n");
    return child; 
} 

/**
 * Die/exit current PROC.
 */
int exit() {
    int exitValue;
    printf("enter an exitValue: ");
    exitValue = geti();
    printf("enter kernel to die with exitValue=%d\n", exitValue);
    syscall(6,exitValue,0);
}

/**
 * Fork current PROC into another PROC.
 * @return 0 if child PROC, else child PID
 */
int fork() {
    int child;
    child = syscall(7,0,0,0);
    if (child) {
        printf("parent %d return form fork, child=%d\n", getpid(), child);
    } else {
        printf("child %d return from fork, child=%d\n", getpid(), child);
    }
}

/**
 * Execute new program.
 */
int exec() {
    int r;
    char filename[32];
    printf("enter exec filename : ");
    gets(filename);
    r = syscall(8,filename,0,0);
    printf("exec failed\n");
}

/**
 * Get integer from STDIN.
 * @return integer
 */
int geti() {
    char s[16];
    gets(s, 16);
    return atoi(s);
}

/**
 * Get character from STDIN.
 * @return Character
 */
int getc() {
    return syscall(90,0,0) & 0x7F;
}

/**
 * Gets string from STDIN.
 * @param Buffer to store string
 * @param Max size (0 == unlimited)
 * @return Length of string
 */
int gets(char *buf, int size) {
    int i = 0, done = 0;
    char c;
    
    if (!size) { size = -1; }
    
    while (!done) {
        c = getc();
        if (i == size && c != '\b') {
            continue;
        }
        switch (c) {
            case('\r'):
                printf("\r\n");
                *(buf + i) = '\0';
                done = 1;
                break;
            
            case('\b'):
                if (i > 0) {
                    printf("\b \b");
                    *(buf + i) = '\0';
                    i--;
                }
                i--;
                break;
            
            default:
                putc(c);
                *(buf + i) = c;
                break;
        }
        i++;
    }
    
    return(i);
}

/**
 * Put character on screen.
 * @param Character to print
 */
int putc(char c) {
    return syscall(91,c,0,0);
}

/**
 * Create new PIPE and two file descriptors.
 * Sets file scope fd[] variable if successful.
 */
int pipe() {
    printf("pipe syscall\n");
    if (syscall(30, pd, 0) >= 0) {
        printf("proc %d created a pipe with fd = %d %d\n", 
                getpid(), pd[0], pd[1]);
    } else {
        printf("Failed to create pipe.\n");
    }
}

/**
 * Print PIPE details and it's contents.
 */
void show_pipe() {
    int i;
    
    pfd();
    printf("Which pipe: ");
    i = geti();
    syscall(35, i);
}

/**
 * Print list of current PROCs file descriptors.
 */
int pfd() {
    syscall(34,0,0,0);
}

/**
 * Read contents of PIPE.
 */
int read_pipe() {
    char fds[32], buf[1024]; 
    int fd, n, nbytes;
    pfd();
    
    printf("enter fd: ");
    fd = geti();
    printf("nbytes: ");
    nbytes = geti();
    //sscanf(fds, "%d %d",&fd, &nbytes);
    
    n = syscall(31, fd, buf, nbytes);
    
    if (n>=0) {
        printf("proc %d back to Umode, read %d bytes from pipe:\n",
               getpid(), n);
        buf[n]=0;
        printf("'%s'\n", buf);
    } else {
        printf("read pipe failed\n");
    }
}

/**
 * Write data to PIPE.
 */
int write_pipe() {
    char fds[16], buf[1024]; 
    int fd, n, nbytes;
    pfd();
    printf("enter fd: ");
    fd = geti();
    printf("String: ");
    gets(buf, 1024);
    //sscanf(fds, "%d %s", &fd, buf);
    nbytes = strlen(buf);
    //printf("fd=%d nbytes=%d : %s\n", fd, nbytes, buf);
    
    n = syscall(32, fd, buf, nbytes);
    
    if (n>=0) {
        printf("proc %d back to Umode, wrote %d bytes to pipe\n", getpid(),n);
    } else {
        printf("write pipe failed\n");
    }
}

/**
 * Close PIPE.
 */
int close_pipe() {
    char s[16]; 
    int fd;
    
    pfd();
    printf("enter fd to close : ");
    gets(s);
    fd = atoi(s);
    syscall(33, fd);
}

/**
 * Prints invalid command prompt. Used
 * when user does not type correct command.
 */
int invalid(char *name) {
    printf("Invalid command : %s\n", name);
}
