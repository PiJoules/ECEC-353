#include "chat.h"

/**
 * djb2 by Dan Bernstein.
 */
key_t name_to_key(const char* str){
    int hash = 5381;

    for (int i = 0; *(str + i); i++){
        int c = (int)str[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return (key_t)hash;
}


Node* create_node(const char* name, size_t size, int permissions){
    if (!(permissions & WRITE)){
        fprintf(stderr, "The permissions given must include WRITE in order to initiualize default values.\n");
        exit(EXIT_FAILURE);
    }

    // Actual size to allocate will be the the size of the node and
    // the buffer size after it
    size_t total_size = sizeof(Node) + size;

    // Create segment
    key_t key = name_to_key(name);
    int shmid = shmget(key, total_size, permissions);
    if (shmid == -1){
        perror("shmget");
        fprintf(stderr, "Could not create node for %s.\n", name);
        exit(EXIT_FAILURE);
    }

    // Attach segment to local memory
    void* shm_addr = shmat(shmid, NULL, 0);
    if (!shm_addr){
        perror("shmat");
        fprintf(stderr, "Could not create node for %s.\n", name);
        exit(EXIT_FAILURE);
    }

    // Create and copy the node into shared mem
    //Node node;
    //node.name = name;
    //node.id = shmid;
    //node.buffer_size = size;
    //node.status = AVAILABLE;
    //memcpy(shm_addr, &node, sizeof(Node));
    Node* node = (Node*)shm_addr;
    node->name = name;
    node->id = shmid;
    node->buffer_size = size;
    
    return (Node*)shm_addr;
}


void detatch_node(Node* node){
    if (shmdt(node) == -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}


void free_node(Node* node){
    int id = node->id;
    detatch_node(node);
    if (shmctl(id, IPC_RMID, NULL) == -1){
       perror("shmctl");
       exit(EXIT_FAILURE);
    }
}

int node_is_up(char* node_name){
    key_t key = name_to_key(node_name);

    int shmid = shmget(key, sizeof(Node), READ);
    if (shmid == -1){
        return 0;
    }

    // Attach segment to local memory
    void* shm_addr = shmat(shmid, NULL, 0);
    if (!shm_addr){
        return 0;
    }

    // Detatch node
    if (shmdt(shm_addr) == -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    return 1;
}

int server_is_up(){
    return node_is_up(SERVER_NAME);
}


/**
 * Copy the bytes of a message onto the buffer of a node.
 */
void transfer_message(Message* message, Node* node){
    memcpy(&(node->buffer), message, sizeof(Message));
}


/**
 * Send a message and receive a response.
 * This will create a node representing the recipient for this process and free the node.
 */
Message* send_message(Message* message, const char* recipient, Node* sender){
    Node* recipient_node = create_node(recipient, sizeof(Message), READ | WRITE);

    // Copy bytes
    transfer_message(message, recipient_node);
    sender->status = WAITING_FOR_RESPONSE;
    
    // Wait for response
    recipient_node->status = RECIEVED_MESSAGE;
    detatch_node(recipient_node);
    while (sender->status != HANDLING_RESPONSE){
        sleep(1);
    }

    // Return the response
    sender->status = AVAILABLE;
    return (Message*)(&(sender->buffer));
}

/**
 * Respond to a sent message.
 * This will create a node representing the recipient for this process and free the node.
 */
void respond_to_message(Message* response, const char* recipient_name, Node* sender){
    assert(sender->status == RECIEVED_MESSAGE);

    Node* recipient = create_node(recipient_name, sizeof(Message), READ | WRITE);
    assert(recipient->status == WAITING_FOR_RESPONSE);

    if (response){
        transfer_message(response, recipient);
    }
    else{
        strcpy(recipient->buffer, "");
    }
    recipient->status = HANDLING_RESPONSE;
    detatch_node(recipient);
    sender->status = AVAILABLE;
}


