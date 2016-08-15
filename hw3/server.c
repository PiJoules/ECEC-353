#include "server.h"

/**
 * Server constants.
 */
static const int server_permissions = IPC_CREAT | READ | WRITE;
static int kill_server = 0;


void sigterm_handler(int signum){
    kill_server = 1;
    fprintf(stderr, "Exiting server. Attempting to free server server.\n");
}


int main(int argc, char* argv[]){
    // Only 1 server should be up
    if (server_is_up()){
        fprintf(stderr, "Another instance of this server has already been started. Multiple instances cannot be created.\n");
        exit(EXIT_FAILURE);
    }

    // Create the server server
    Node* server = create_node(SERVER_NAME, DEFAULT_SHM_SIZE, server_permissions);
    server->status = AVAILABLE;

    // Attempt to free server when catching unexpected exit
    signal(SIGINT, sigterm_handler);
    signal(SIGTERM, sigterm_handler);

    while (!kill_server){
        // Wait for messages
        if (server->status == RECIEVED_MESSAGE){
            // Handle the message
            Message* message = (Message*)(&(server->buffer));
            const char* sender_name = message->sender;
            printf("%s\n", message->content);

            Message response;
            strcpy(response.sender, SERVER_NAME);
            strcpy(response.content, "Hello client.");
            response.content_size = strlen(response.content);

            respond_to_message(&response, sender_name, server);
        }
        sleep(1);
    }

    free_node(server);
    printf("Successfully quit server.\n");

    return 0;
}
