#include "chat.h"

/**
 * Allocate specific shared memory writeable to only the server and
 * readable by clients.
 */
void* shmalloc_universal(size_t size, int shmflag){
    return NULL;
}


/**
 * Allocate shared memory space.
 */
void* shmalloc(size_t size, int shmflag){
    return NULL;
}
