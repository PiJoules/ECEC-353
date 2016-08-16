#ifndef CHAT_H
#define CHAT_H

/**
 * Header to contain shared stuff between files.
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <limits.h>

#define SAFE_MALLOC(var_name, type, size) \
    type var_name = (type)malloc(size); \
    if (!var_name){ \
        fprintf(stderr, "Could not malloc enough space for type.\n");\
        exit(EXIT_FAILURE); \
    }

#include "hash.h"

// Place of shared memory where the server will initiually write to and
// clients can listen to
#define SERVER_NAME "server"

// Default size of each shared memory block
#define DEFAULT_SHM_SIZE 1024
// When sending a message, the size of the message is requested when creating
// the recipient node, so this message size must be smaller than that
// allocated to the original shm.
#define MESSAGE_SIZE 512
#define NAME_SIZE 64

// Permissions
#define READ 0444
#define WRITE 0222

typedef struct Node Node;
typedef struct Message Message;
typedef enum {
    AVAILABLE,
    RECIEVED_MESSAGE,
    WAITING_FOR_RESPONSE,
    HANDLING_RESPONSE,
} NodeStatus;

struct Node {
    const char* name;
    int id;
    size_t buffer_size;
    NodeStatus status;
    char* buffer;  // The address of this pointer is the start of the buffer (&node->buffer)
};

typedef enum {
    JOIN_CREATE_GROUP,
    BROADCAST_MESSAGE,
    DISPLAY_USERS,
	 LEAVE_GROUP,
} MessageType;

struct Message {
    char sender[NAME_SIZE];
	 MessageType type;
	 size_t content_size;
    char content[MESSAGE_SIZE];  // Treat the same as the node buffer member.
};


key_t name_to_key(const char* str);
// Get and attach memory
Node* create_node(const char* name, size_t size, int permissions);
void detatch_node(Node* node);
void free_node(Node* node);
int node_is_up(char* node_name);
int server_is_up();
Message* send_message(Message* message, const char* recipient, Node* sender);
void respond_to_message(Message* response, const char* recipient, Node* sender);


#endif
