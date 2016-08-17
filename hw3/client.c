#include "client.h"

/**
 * Client constants
 */
//static const char client_name[] = "client_test";
static char client_name[256];
static int kill_client = 0;
static Node* client;

void sigterm_handler(int signum){
	kill_client = 1;
	fprintf(stderr, "Exiting client. Attempting to free client.\n");
    free_node(client);
}


void* handle_user_input(void* args){
    char* userInput = NULL;
	char *privateFlag = "Private";
	char *displayFlag = "Display Users";
	char *exitFlag = "exit.";
    while (!kill_client){
        printf("Enter message: ");
        size_t size;
        getline(&userInput, &size, stdin);

        // private message. handle client B ID
        if(strstr(userInput, privateFlag) == userInput){

        }
        // display users flag
        else if(strstr(userInput, displayFlag) == userInput){
            Message message;
            strcpy(message.sender, client_name);
            message.type = DISPLAY_USERS;
            strcpy(message.content, "");
            message.content_size = strlen(message.content);
            Message* returnMessage = send_message(&message, SERVER_NAME, client);
            printf("users in group: %s\n", returnMessage->content);
        }
        else if(strstr(userInput, exitFlag) == userInput){
            kill_client = 1;
            printf("Exiting server");
            Message message;
            strcpy(message.sender, client_name);
            message.type = LEAVE_GROUP;
            strcpy(message.content, "");
            message.content_size = strlen(message.content);
            send_message(&message, SERVER_NAME, client);
        }
        else {
            // Broadcast
            Message response;
            strcpy(response.sender, client_name);
            response.type = BROADCAST_MESSAGE;
            strcpy(response.sender, client_name);
            strcpy(response.content, userInput);
            response.content_size = strlen(response.content);
            printf("Sending message.\n");
            while (client->status != AVAILABLE){
                printf("Waiting to be available before sending message.\n");
                sleep(1);
            }
            printf("client status: %d\n", client->status);
            send_message(&response, SERVER_NAME, client);
            printf("Sent message.\n");
        }
        printf("Kill client: %d\n", kill_client);
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
	char flag[] = "capacity";
	char *msgContent = returnMessage->content;

	if(strstr(msgContent, flag) != NULL){
        kill_client = 1;
	}

    pthread_t pth;
    pthread_create(&pth, NULL, handle_user_input, NULL);

	while(!kill_client){
		// handle messages sent to current node
		if(client->status == RECIEVED_MESSAGE){
            Message* message = (Message*)(&(client->buffer));
			const char* sender_name = message->sender;
			printf("[%s] %s\n", sender_name, returnMessage->content);

            Message response;
            strcpy(response.sender, client_name);
            strcpy(response.content, "");
            response.content_size = strlen(response.content);

            respond_to_message(&response, sender_name, client);
            printf("Handled message.\n");
			//if(returnMessage->type == JOIN_CREATE_GROUP){
			//	printf("%s\n", returnMessage->content);
			//}
			//else if
			//printf("%s\n", message->content);
			//response->type =  
        }
		// user gets a message from either broadcast or private
		// and goes into HANDLING_RESPONSE status
		//else if(client->status == HANDLING_RESPONSE){ 
        sleep(1);
    }


    // Free the client
    //free_node(client);
	printf("Successfully quit client.\n");

    return 0;
}
