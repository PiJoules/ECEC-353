#include "client.h"

/**
 * Client constants
 */
//static const char client_name[] = "client_test";
static char client_name[256];
static int kill_client = 0;
static Node* client;


void send_leave_message(){
    kill_client = 1;
    printf("Exiting server");
    Message message;
    strcpy(message.sender, client_name);
    message.type = LEAVE_GROUP;
    strcpy(message.content, "");
    message.content_size = strlen(message.content);
    send_message(&message, SERVER_NAME, client);
}


void sigterm_handler(int signum){
	kill_client = 1;
	fprintf(stderr, "Exiting client. Attempting to free client.\n");
    send_leave_message();
    free_node(client);
}


void* handle_user_input(void* args){
    char* userInput = NULL;
	char privateFlag[] = "[private";
	char displayFlag[] = "[display]\n";
	char exitFlag[] = "[exit]\n";
    while (!kill_client){
        printf("Enter message: ");
        size_t size;
        getline(&userInput, &size, stdin);

        // private message. handle client B ID
        if(strstr(userInput, privateFlag) == userInput){
            char user[256];
            char input[256];
            sscanf(userInput, "[private %[^]]] %s", user, input);
            printf("user: %s\n", user);
            printf("input: %s\n", input);

            if (!node_is_up(user)){
                printf("This user is not available at the moment.\n");
            }
            else if (!strcmp(user, client_name)){
                printf("Cannot send a private message to self.\n");
            }
            else {
                Message message;
                strcpy(message.sender, client_name);
                strcpy(message.content, input);
                message.content_size = strlen(message.content);
                send_message(&message, user, client);
            }
        }
        // display users flag
        else if(!strcmp(userInput, displayFlag)){
            Message message;
            strcpy(message.sender, client_name);
            message.type = DISPLAY_USERS;
            strcpy(message.content, "");
            message.content_size = strlen(message.content);
            Message* returnMessage = send_message(&message, SERVER_NAME, client);
            printf("users in group: %s\n", returnMessage->content);
        }
        else if(!strcmp(userInput, exitFlag)){
            send_leave_message();
        }
        else {
            // Broadcast
            Message response;
            strcpy(response.sender, client_name);
            response.type = BROADCAST_MESSAGE;
            strcpy(response.sender, client_name);
            strcpy(response.content, userInput);
            response.content_size = strlen(response.content);
            send_message(&response, SERVER_NAME, client);
        }
    }
    return NULL;
}


int main(int argc, char* argv[]){
    // Check if server node is up
    if (!server_is_up()){
        fprintf(stderr, "The server has not yet been started.\n");
        exit(EXIT_FAILURE);
    }

    // Check command line args
    if (argc != 3){
        fprintf(stderr, "Expected 2 command line args [group_id, client_name].\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigterm_handler);
    signal(SIGTERM, sigterm_handler);

    // Get group id
    char* group_id = argv[1];

    // Check for unique name
    if (node_is_up(argv[2])){
        printf("The name '%s' is already taken. Please use another one.\n", argv[2]);
        return 0;
    }

    strcpy(client_name, argv[2]);

    // Create client and server node
    client = create_node(client_name, DEFAULT_SHM_SIZE, IPC_CREAT | READ | WRITE);
    client->status = AVAILABLE;

	// Submits initial group creation message
    Message message;
	message.type = JOIN_CREATE_GROUP; 
    strcpy(message.sender, client_name);
    strcpy(message.content, group_id);
    message.content_size = strlen(message.content);

	//initialize response message. need to change type
    Message* returnMessage = send_message(&message, SERVER_NAME, client);
    if (returnMessage->type == LEAVE_GROUP){
        free_node(client);
        return 0;
    }

    pthread_t pth;
    pthread_create(&pth, NULL, handle_user_input, NULL);

	while(!kill_client){
		// handle messages sent to current node
		if(client->status == RECIEVED_MESSAGE){
            Message* message = (Message*)(&(client->buffer));
			const char* sender_name = message->sender;

            switch (message->type){
                case LEAVE_GROUP:
                    kill_client = 1;
                    free_node(client);
                    break;
                default:
                    printf("[%s] %s\n", sender_name, returnMessage->content);

                    Message response;
                    strcpy(response.sender, client_name);
                    strcpy(response.content, "");
                    response.content_size = strlen(response.content);

                    respond_to_message(&response, sender_name, client);
                    break;
            }
        }
        sleep(1);
    }
	printf("Successfully quit client.\n");

    return 0;
}
