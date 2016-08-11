#include "server.h"

// Shared mem creation, and read/write permissions
static const int server_flag = IPC_CREAT | 0666;


/**
 * Create shared memory 
 */
void* shmalloc_server(size_t size){
    int shmid = shmget(UNIVERSAL_KEY, SHM_SIZE, IPC_CREAT | READ | WRITE);
    if (shmid < 0){
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return NULL;
}


int main(int argc, char* argv[]){
    // Write to shared piece of memory known to clients for indicating
    // the server is on.

    return 0;
}
