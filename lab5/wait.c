
/**
 * Dequeue most important PROC from a 
 * linked list.
 * @param Pointer to head of queue
 * @return Pointer to dequeued PROC
 */
PROC *dequeue(PROC **queue)
{
    PROC *ptr = *queue;
    
    if (ptr) {
        // Remove first PROC
        *queue = (*queue)->next;
        ptr->next = 0;
    }
    
    return(ptr);
}

/**
 * Dequeue specific PROC from a 
 * linked list.
 * @param Pointer to head of queue
 * @param Pointer proc for removal
 * @return Pointer to dequeued PROC
 */
PROC *dequeueProc(PROC **queue, PROC *p)
{
    PROC *prev = 0, *ptr = *queue;

    // Find PROC
    while (ptr != p && ptr != 0) {
        prev = ptr;
        ptr = ptr->next;
    }
    
    if (ptr) {
        // Remove PROC
        if (prev) {
            prev->next = ptr->next;
            ptr->next = 0;
        } else {
            *queue = (*queue)->next;
            ptr->next = 0;
        }
    }
    
    return(ptr);
}

/**
 * Enqueue PROC in order of priority 
 * in a linked list.
 * @param Pointer to head of queue
 * @param Pointer to PROC
 * @return Pointer to queued
 */
PROC **enqueue(PROC **queue, PROC *ptr)
{
    PROC *prev = 0, *queue_ptr = *queue;

    // Empty queue?
    if (*queue == 0) {
        *queue = ptr;
        ptr->next = 0;
    } else {
        // Find spot
        while (queue_ptr &&
               queue_ptr->priority >= ptr->priority) {
            prev = queue_ptr;
            queue_ptr = queue_ptr->next;
        }

        // Insert process
        if (prev) {
            ptr->next = prev->next;
            prev->next = ptr;
        } else {
            ptr->next = queue_ptr;
            *queue = ptr;
        }
    }
    
    return queue;
}
