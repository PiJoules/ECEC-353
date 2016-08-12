#include "server.h"

// Shared mem creation, and read/write permissions
static const int server_flag = IPC_CREAT | 0666;


/**
 * Create shared memory 
 */
void* shmalloc_server(size_t size){
    // Create segment
    int shmid = shmget(UNIVERSAL_KEY, size, IPC_CREAT | READ | WRITE);
    if (shmid == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach segment to local memory
    void* shm_addr = shmat(shmid, NULL, 0);
    if (!shm_addr){
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    return shm_addr;
}


/**
 * Shared mem layout.
 * Bytes - Description
 * 0 - NodeStatus
 * 1+ - Bytes
 */


typedef struct Node Node;
typedef struct Message Message;
typedef enum {
    AVAILABLE,
    SENDING_MESSAGE,
    WAITING_FOR_RESPONSE
} NodeStatus;

struct Node {
    char* name;
    void* shared_mem;
    size_t buffer_size;
    int id;
    key_t key;
};

struct Message {
    Node* sender;
    char* content;
};


static Node self = {
    "Server",
    NULL,
    SHM_SIZE,
    0,
    UNIVERSAL_KEY
};


/**
 * Wait for the node to become open to talk first.
 */
void wait_until_available(Node* node){
    char* shared_mem = (char*)node->shared_mem;
    while (shared_mem[0] != AVAILABLE){
        sleep(1);
    }
}


/**
 * Send a message to someone.
 */
char* send_message(Node* recipient, Message* message){
    wait_until_available(recipient);

    // Write to recipient's shared mem and end with null terminator.
    char* shared_mem = (char*)recipient->shared_mem;
    size_t len = strlen(message->content);
    strncpy(shared_mem, message->content, len);
    shared_mem[len] = 0;

    // Scan message into buffer

    return NULL;
}

//Node* create_node(char* name, size_t size){
//    // Actual size to allocate will be the the size of the node and
//    // the buffer size after it
//    size_t total_size = sizeof(Node) + size;
//
//    // Create segment
//    int shmid = shmget(node->key, total_size, IPC_CREAT | READ | WRITE);
//    if (shmid == -1){
//        perror("shmget");
//        exit(EXIT_FAILURE);
//    }
//
//    // Attach segment to local memory
//    void* shm_addr = shmat(shmid, NULL, 0);
//    if (!shm_addr){
//        perror("shmat");
//        exit(EXIT_FAILURE);
//    }
//    
//    return NULL;
//}

void initialize_node(Node* node, size_t size){
    // Actual size to allocate will be the the size of the node and
    // the buffer size after it
    size_t total_size = sizeof(Node) + size;

    // Create segment
    int shmid = shmget(node->key, total_size, IPC_CREAT | READ | WRITE);
    if (shmid == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach segment to local memory
    void* shm_addr = shmat(shmid, NULL, 0);
    if (!shm_addr){
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Set initialized values
    node->shared_mem = shm_addr;
    node->buffer_size = size;
    node->id = shmid;

    // Set memory address space to all 0s
    memset(shm_addr, '0', total_size);

    // Copy the node contents onto the 
}

void free_node(Node* node){
    if (shmdt(node->shared_mem) == -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    if (shmctl(node->id, IPC_RMID, NULL) == -1){
       perror("shmctl");
       exit(EXIT_FAILURE);
    }
}


int main(int argc, char* argv[]){
    // Write to shared piece of memory known to clients for indicating
    // the server is on.

    initialize_node(&self, SHM_SIZE);
    free_node(&self);

    return 0;
}
