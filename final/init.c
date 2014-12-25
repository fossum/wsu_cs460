/* 
 * File:   init.c
 * Author: ericfoss
 *
 * Created on December 1, 2014, 2:17 PM
 * 
 * NOTE: this init.c creates only ONE login process on console=/dev/tty0
 * YOUR init.c must also create login processes on serial ports /dev/ttyS0
 * and /dev/ttyS1.. 
 */

#include "my_assert.h"
#include "printlib.h"
#include "ucode.h"

#define TERMS 3

int pid, i, status;
int stdin, stdout, stderr;

// Terminal PIDs
int tty = -1, ttyS0 = -1, ttyS1 = -1, lp0 = -1;

int main()
{
    int tmp_child;
    
    // open output
    stdin = open("/dev/tty0", O_RDONLY);
    stdout = open("/dev/tty0", O_WRONLY);
    stderr = open("/dev/tty0", O_WRONLY);
    
    // Now we can use printf, which calls putc(), which writes to stdout
    printf("FOSIX : pid %d forking a login task on console\n", getpid());
    
    // Create all terminal processes !!! Might need to login after creating everything !!!
    for (i=(TERMS-1); i>=0; i--) {
        tmp_child = fork();
        if (tmp_child) {
            pause(1);
            switch(i) {
                case 0:
                    fprintf(STDERR, "Created tty0\n");
                    tty = tmp_child;
                    break;
                case 1:
                    fprintf(STDERR, "Created ttyS0\n");
                    ttyS0 = tmp_child;
                    break;
                case 2:
                    fprintf(STDERR, "Created ttyS1\n");
                    ttyS1 = tmp_child;
                    break;
                case 3:
                    fprintf(STDERR, "Created lp0\n");
                    lp0 = tmp_child;
                    break;
            }
        } else {
            break;
        }
    }
    
    if (tmp_child) {
        parent();
    } else {
        login(i);
    }
    
    assert(1);
    return(0);
}

int login(int terminal) {
    pause(0.5);
    switch(terminal) {
        case 0:
//            pause(0.43);
            exec("login /dev/tty0");
            break;
        case 1:
//            pause(0.33);
            exec("login /dev/ttyS0");
            break;
        case 2:
//            pause(0.14);
            exec("login /dev/ttyS1");
            break;
        case 3:
//            pause(0.89);
            exec("login /dev/lp0");
            break;
        default:
            assert(1);
            break;
    }
}

int parent() {
    while(1) {
        fprintf(STDERR, "FOSIX : PID:%d waiting for a terminal to end ...\n", getpid());
        pid = wait(&status);
        
        if (pid == tty) {
            tty = fork();
            if (!tty) {
                login(0);
            }
        } else if (pid == ttyS0) {
            ttyS0 = fork();
            if (!ttyS0) {
                login(1);
            }
        } else if (pid == ttyS1) {
            ttyS1 = fork();
            if (!ttyS1) {
                login(2);
            }
        } else if (pid == lp0) {
            lp0 = fork();
            if (!lp0) {
                login(3);
            }
        } else {
            fprintf(STDERR, "INIT: buried an orphan child pid=%d\n", pid);
        }
    }
}
