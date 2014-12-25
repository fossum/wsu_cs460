
#include "../include/forkexec.h"
#include "../include/my_assert.h"

extern PROC *running;

/**
 * Copies byte for byte from one segment to another.
 * @param Source segment address
 * @param Destination segment address
 * @param Number of bytes to copy
 */
int copyImage(u16 seg1, u16 seg2, u16 size) {
    int i, word;
    char b;
    
    assert(seg1 == 0 || seg2 == 0);
    assert(seg1 % 0x1000 || seg2 % 0x1000);
    
    // Copy word for word... (word==2 bytes)
    printf("Copying %u from 0x%x to 0x%x\n", size, seg1, seg2);
    for (i=size; i; i--) {
	//printf("Use put byte in case odd size\n");
	b = get_byte(seg1, i);
	put_byte(b, seg2, i);
    }
    
    return(0);
}

/**
 * Fork user-mode PROC as mirror image of itself.
 * @return Child PID (failure: -1)
 */
int fork() {
    PROC *ptr = kfork(0);   	// kfork() but do NOT load any Umode image for child 
    
    if (ptr == 0) {           	// kfork failed
        printf("Failed to kfork()\n");
	return(-1);
    }
    
    return(ptr->pid);
}

/**
 * Execute user-defined new program from filename.
 * @param Virtual address to new executable path
 */
int exec(char *filename) {
    int i, reg;
    char buf[100];
    
    assert(filename == 0 || running->uss == 0);
    
    // Load new uMode image
    if (!get_str(filename, buf, 100) ||
	!load(buf, running->uss)) {
	return(-1);
    }
    
    // Reset uStack
    for (i=-2; i>=-24; i-=2) {
        switch(i) {
            case -2:    reg = 0x0200; 		break;
            case -4:
            case -22:
            case -24:   reg = running->uss;	break;
            default:    reg = 0; 		break;
        }
        put_word(reg, running->uss, i);
    }
    
    running->usp = -24;
    return(0);
}
