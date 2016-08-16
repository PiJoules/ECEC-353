#include <stdio.h>
#include "client.h"

/**
 * Client constants
 */
static const char client_name[] = "client_test";
static int kill_client = 0;

void sigterm_handler(int signum){
	kill_client = 1;
	fprintf(stderr, "Exiting client. Attempting to free client.\n");
}


int main(int argc, char* argv[]){
    // Check if server node is up
    if (!server_is_up()){
        fprintf(stderr, "The server has not yet been started.\n");
        exit(EXIT_FAILURE);
    }

    // Check command line args
    if (argc != 2){
        fprintf(stderr, "Expected 1 command line arg.\n");
        exit(EXIT_FAILURE);
    }

    // Get group id
    char* group_id = argv[1];

    // Create client and server node
    Node* client = create_node(client_name, DEFAULT_SHM_SIZE, IPC_CREAT | READ | WRITE);
    client->status = AVAILABLE;

	 // Submits initial group creation message
    Message message;
	 message.type = JOIN_CREATE_GROUP; 
    strcpy(message.sender, client_name);
    strcpy(message.content, group_id);
    message.content_size = strlen(message.content);

	 //initialize response message. need to change type
    Message* returnMessage = send_message(&message, SERVER_NAME, client);
	 char *flag = "capacity";
	 char *msgContent = returnMessage->content;

	 if(strstr(msgContent, flag) != NULL){
		kill_client = 1;
	 }

	char userInput[256];
	char *broadcastFlag = "Broadcast: ";
	char *privateFlag = "Private";
	char *displayFlag = "Display Users";
	char *exitFlag = "Exit";

	while(!kill_client){

		if(client->status = AVAILABLE){
			// ask for user input
			printf("Enter message: \n");
			scanf("%s", userInput);

			// broadcast message
			if(strstr(userInput, broadcastFlag) != NULL){
				message.type == BROADCAST_MESSAGE;
				strcpy(message.content, userInput);
				message.content_size = strlen(message.content);
				send_message(&message, SERVER_NAME, client);

			}
			// private message. handle client B ID
			else if(strstr(userInput, privateFlag) != NULL){
			// user must enter 'Private UserID: Message'
			// parse

			}
			// display users flag
			else if(strstr(userInput, displayFlag) != NULL){
				message.type = DISPLAY_USERS;
				strcpy(message.content, "");
				message.content_size = strlen(message.content);
				returnMessage = send_message(&message, SERVER_NAME, client);
				printf("Members in group: %s", returnMessage.content);
			}
			else if(strstr(userInput, exitFlag) != NULL){
				kill_client = 1;
				printf("Exiting server");
				message.type = LEAVE_GROUP;
				strcpy(message.content, "");
				message.content_size = strlen(message.content);
				send_message(&message, SERVER_NAME, client);

			}
		}

		// handle messages sent to current node
		else if(client->status == RECIEVED_MESSAGE){
			
			if(returnMessage->type == JOIN_CREATE_GROUP){
				printf(returnMessage->content);
				
			}
			else if
			printf("%s\n", message->content);
			response->type =  
			}
		// user gets a message from either broadcast or private
		// and goes into HANDLING_RESPONSE status
		else if(client->status == HANDLING_RESPONSE){ 

		}


    // Free the client
    free_node(client);
	 printf("Successfully quit client.\n");

    return 0;
}
