
#include "../include/pipe.h"

extern PROC *running;
extern PIPE pipe[];
extern OFT oft[];

char *MODE[] = {"READ_PIPE ", "WRITE_PIPE"};

/**
 * Print details and contents of supplied PIPE.
 * @param Pointer to PIPE
 */
int show_pipe(PIPE *ptr) {
    int i, j = ptr->tail;
    
    printf("Pipe 0x%x\n", ptr);
    printf("Head: %u, Tail: %u, Data: %u, Room: %u\n",
           ptr->head, ptr->tail, ptr->data, ptr->room);
    printf("Readers: %u, Writers: %u\n", ptr->nreader, ptr->nwriter);
    printf("------------ PIPE CONTENETS ------------\n");
    for (i=ptr->data; i>0; i--) {
        if (i > PSIZE) {
            printf("Invalid pipe address!\n");
            break;
        }
        if (j == PSIZE) { j = 0; }
        printf("%c", ptr->buf[j++]);
    }
    printf("\n----------------------------------------\n");
    
    return(0);
}

/**
 * Print current PROC file descriptors.
 */
int pfd() {
    int i;
    OFT *ptr;
    
    // print running process' opened file descriptors
    printf("Proc %d file descriptors:\n", running->pid);
    printf("----------------------------\n");
    printf("- FD Mode pipe   refCount  -\n");
    printf("----------------------------\n");
    for(i=0; i<NFD; i++) {
        ptr = running->fd[i];
        printf("- %u  ", i);
        if (ptr != 0) {
            switch(ptr->mode) {
                case(READ_PIPE): printf("%c", 'R'); break;
                case(WRITE_PIPE): printf("%c", 'W'); break;
                default: printf("%c", ' ');
            }
            printf("    0x%x %d", ptr->pipe_ptr, ptr->refCount);
        }
        printf("\n");
    }
    printf("----------------------------\n");
}

/**
 * Reads data from the supplied file descriptor.
 * @param File descriptor index
 * @param Buffer to write into
 * @param Number of bytes to read
 * @return Bytes read
 */
int read_pipe(int ifd, char *buf, int n) {
    PIPE *ptr = running->fd[ifd]->pipe_ptr;
    int nRead = 0;
    
    if (running->fd[ifd] == 0) {
        printf("Invalid file handle index(%d)\n", ifd);
        return(-1);
    }
    if (running->fd[ifd]->mode != READ_PIPE) {
        printf("Invalid file handle mode: write-only pipe\n", ifd);
        return(-1);
    }
    if (n == 0) {
        n = ptr->data;
    }
    
    // read data
    while (n > 0) {
        if (ptr->data) {
            //printf("kmode: got %c\n", ptr->buf[ptr->tail]);
            put_byte(ptr->buf[ptr->tail++], running->uss, buf++);
            if (ptr->tail == PSIZE) { ptr->tail = 0; }
            ptr->room++;
            ptr->data--;
            nRead++;
            n--;
            kwakeup(&ptr->room);
        } else {
            if (ptr->nwriter > 0) {
                //printf("Waking PROCs on pipe 0x%x ...", ptr);
                //printf("and going to sleep.\n", ptr);
                kwakeup(&ptr->room);
                ksleep(&ptr->room);
            } else {
                printf("No more data or writers ... closing pipe.\n", ptr);
                close_pipe(ifd);
                break;
            }
        }
    }
    
    return(nRead);
}

/**
 * Writes data into the supplied file descriptor.
 * @param File descriptor index
 * @param Buffer to read from
 * @param Number of bytes to write
 * @return Bytes written
 */
int write_pipe(int ifd, char *buf, int n) {
    PIPE *ptr = running->fd[ifd]->pipe_ptr;
    char str[1024];
    int i = 0;
    
    if (running->fd[ifd] == 0) {
        printf("Invalid file handle index(%d)\n", ifd);
        return(-1);
    }
    if (running->fd[ifd]->mode != WRITE_PIPE) {
        printf("Invalid file handle mode: read-only pipe\n", ifd);
        return(-1);
    }
    if (ptr->nreader <= 0) {
        printf("Broken pipe: No readers, closing pipe\n");
        close_pipe(ifd);
        return(-1);
    }
    get_str(buf, str, n);
    
    // Code to write pipe
    while (n > 0) {
        if (ptr->room) {
            // write data
            ptr->buf[ptr->head++] = str[i++];
            if (ptr->head == PSIZE) { ptr->head = 0; }
            ptr->room--;
            ptr->data++;
            n--;
            kwakeup(&ptr->room);
        } else {
            // No room, wait
            //printf("sleep");
            ksleep(&ptr->room);
        }
    }
    
    return(i);
}

/**
 * Gets a free file descriptor if available.
 * @return Free descriptor pointer (otherwise NULL)
 */
OFT *get_free_fd() {
    OFT *ptr = 0;
    int i = 0;
    
    while (i < NFD) {
        ptr = &oft[i++];
        if (ptr->refCount == 0) {
            return(ptr);
        }
    }
    
    printf("Failed to return free file descriptor\n");
    return(0);
}

/**
 * Gets free PIPE pointer if available.
 * @return Free PIPE pointer (otherwise NULL)
 */
PIPE *get_free_pipe() {
    PIPE *ptr;
    int i = 0;
    
    while (i < NPIPE) {
        ptr = &pipe[i++];
        if (ptr->busy == NULL) {
            return(ptr);
        }
    }
    
    printf("Failed to return free pipe\n");
    return(0);
}

/**
 * Creates a new PIPE with two file descriptors.
 * @param Virtual address to store file descriptors
 * @return negative for failure
 */
int kpipe(int pd[2]) {
    OFT *read_ptr = 0, *write_ptr = 0;
    PIPE *pipe_ptr;
    int i = 0, j = 0;
    
    // Find empty pipe
    read_ptr = get_free_fd();
    read_ptr->refCount = 1;
    write_ptr = get_free_fd();
    pipe_ptr = get_free_pipe();
    while (i < NFD && running->fd[i] != 0) { i++; }
    while (j < NFD && (j == i || running->fd[j] != 0)) { j++; }
    if (read_ptr == NULL || write_ptr == NULL ||
        pipe_ptr == NULL || i == NFD || j == NFD) {
        read_ptr->refCount = 0;
        return(-1);
    }
    
    // Assign to running
    running->fd[i] = read_ptr;
    put_word(i, running->uss, pd++);
    running->fd[j] = write_ptr;
    put_word(j, running->uss, pd);
    
    // Init pipe
    pipe_ptr->head = 0;
    pipe_ptr->tail = 0;
    pipe_ptr->data = 0;
    pipe_ptr->room = PSIZE;
    pipe_ptr->nwriter = 1;
    pipe_ptr->nreader = 1;
    pipe_ptr->busy = 1;
    
    // Init read descriptor
    read_ptr->mode = READ_PIPE;
    // already incremented refCount (read_ptr->refCount++;)
    read_ptr->pipe_ptr = pipe_ptr;
    
    // Init write descriptor
    write_ptr->mode = WRITE_PIPE;
    write_ptr->refCount++;
    write_ptr->pipe_ptr = pipe_ptr;
    
    return(0);
}

/**
 * Close file descriptor.
 * @param File descriptor index
 */
int close_pipe(int fd) {
    OFT *op; PIPE *pp;
  
    //printf("proc %d close_pipe: fd=%d\n", running->pid, fd);
  
    op = running->fd[fd];
    running->fd[fd] = 0;                // clear fd[fd] entry
    pp = op->pipe_ptr;
  
    if (op->mode == READ_PIPE) {
        pp->nreader--;                  // dec n reader by 1
        
        if (--op->refCount == 0) {      // last reader
            if (pp->nwriter <= 0) {     // no more writers
                pp->busy = 0;           // free the pipe   
                return;
            }
        }
        // Still others
        kwakeup(&pp->room); 
        return;
    }
    
    if (op->mode == WRITE_PIPE) {
        pp->nwriter--;
        
        if (--op->refCount == 0) {
            if (pp->nreader <= 0) {
                pp->busy = 0;           // free the pipe   
                return;
            }
        }
        // Still others
        kwakeup(&pp->room); 
        return;
    }
}
