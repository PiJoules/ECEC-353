#include "client.h"

/**
 * Client constants
 */
static const char client_name[] = "client_test";

int main(int argc, char* argv[]){
    // Check if server node is up
    if (!server_is_up()){
        fprintf(stderr, "The server has not yet been started.\n");
        exit(EXIT_FAILURE);
    }

    // Create client and server node
    Node* client = create_node(client_name, DEFAULT_SHM_SIZE, IPC_CREAT | READ | WRITE);
    client->status = AVAILABLE;

    // Send a test message to server
    Message message;
    strcpy(message.sender, client_name);
    strcpy(message.content, "Hello server");
    message.content_size = strlen(message.content);

    Message* response = send_message(&message, SERVER_NAME, client);
    printf("%s\n", response->content);

    // Free the client
    free_node(client);

    return 0;
}
