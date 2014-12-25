struct PROC;

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