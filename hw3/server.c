#include "server.h"
<<<<<<< HEAD
#include "hash.h"

=======
>>>>>>> 05651a86c6b1e6d8b3065ded2a87e0c82131f2e6

/**
 * Server constants.
 */
static const int server_permissions = IPC_CREAT | READ | WRITE;
static int kill_server = 0;

#define VOIDIFY_FUNC(var) (void* (*)(void*))var
#define VOIDIFY_FUNC2(var) (void (*)(void*))var

/**
 * Simple strdup implementation.
 */
static char* str_copy(const char* str){
    size_t len = strlen(str);
    char* new_str = (char*)malloc(sizeof(char) * len + 1);
    if (!new_str){
            perror("malloc failed");
            return NULL;
        }
    strncpy(new_str, str, len);
    new_str[len] = '\0';
    return new_str;
}


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

	 // Hash table creation for storing groupID -> [client1, client2,...]
	 int max_size = 10;
	 Hashtable* group_to_clients = ht_create(max_size, free, VOIDIFY_FUNC(str_copy));
	 Hashtable* clients = ht_create(max_size, VOIDIFY_FUNC2(ll_free), VOIDIFY_FUNC(ll_copy));
	 int client_count = 0;

    while (!kill_server){
        // Wait for messages
        if (server->status == RECIEVED_MESSAGE){
            // Handle the message
            Message* message = (Message*)(&(server->buffer));
<<<<<<< HEAD
				const char* sender_name = message->sender;

				Message response;
				strcpy(response.sender, SERVER_NAME);
				strcpy(response.content, message->content);
				response.content_size = strlen(response.content);

				// client is new and wants to join a group
				if (message->type == JOIN_CREATE_GROUP){
					response.type = JOIN_CREATE_GROUP;
					
					// check if client count has hit capacity
					if (client_count < max_size){

						// add new group/client to group in hash table
						char* group_id = message->content;
						List* list = (List*)ht_get(group_to_clients, group_id);

						if ( !list ){
							List* list = create_list();
							append_to_list(list, message->sender);
							ht_set(group_to_clients, group_id, list);
							
							strcpy(response.content, "Added to new group!");
							response.content_size = strlen(response.content);
							respond_to_message(&response, sender_name, server);
							
							client_count++;
						}
						// else just update the key's list
						else{
							append_to_list(list, message->sender);
							ht_set(group_to_clients, group_id, list);

							strcpy(response.content, "Added to existing group!");
							response.content_size = strlen(response.content);
							respond_to_message(&response, sender_name, server);
							
							client_count++;
						}
					}
					// send response to user that max limit is reached
					else{
						response.type = LEAVE_GROUP;
            		strcpy(response.content, "Server is at maximum capacity. Please end session.");
            		response.content_size = strlen(response.content);
=======
			const char* sender_name = message->sender;

			Message response;
			strcpy(response.sender, SERVER_NAME);
			strcpy(response.content, message->content);
			response.content_size = strlen(response.content);

			// client is new and wants to join a group
			if (message->type == JOIN_CREATE_GROUP){
				response.type = JOIN_CREATE_GROUP;
				
				// check if client count has hit capacity
				if (client_count < max_size){

					// add new group/client to group in hash table
					char* group_id = message->content;
					LinkedList* list = (LinkedList*)ht_get(group_to_clients, group_id);

					if ( !list ){
						LinkedList* list = ll_create(free, VOIDIFY_FUNC(str_copy));
						ll_prepend(list, message->sender);
						ht_set(group_to_clients, group_id, list);
						
						strcpy(response.content, "Added to new group!");
						response.content_size = strlen(response.content);
>>>>>>> 05651a86c6b1e6d8b3065ded2a87e0c82131f2e6
						respond_to_message(&response, sender_name, server);
						
						client_count++;
					}
					// else just update the key's list
					else{
						ll_prepend(list, message->sender);
						ht_set(group_to_clients, group_id, list);

						strcpy(response.content, "Added to existing group!");
						response.content_size = strlen(response.content);
						respond_to_message(&response, sender_name, server);
						
						client_count++;
					}
				}
				// send response to user that max limit is reached
				else{
					response.type = LEAVE_GROUP;
            	strcpy(response.content, "Server is at maximum capacity. Please end session.");
            	response.content_size = strlen(response.content);
					respond_to_message(&response, sender_name, server);
					
					char* group_id = (char*)ht_get(clients, message->sender);
					ht_remove(clients, sender_name);
					ll_remove_value(ht_get(group_to_clients, group_id), sender_name);
					client_count--;
				}
			}

			// client wants to message the group
			else if (message->type == BROADCAST_MESSAGE){
				// iterate through hashtable groupID->values to write
				// broadcast message
				char* group_id = (char*)ht_get(clients, message->sender);
				LinkedList* list = (LinkedList*)ht_get(group_to_clients, group_id);
				LinkedListNode* current_client = list->head;
				
				strcpy(response.content, message->content);
				response.type = BROADCAST_MESSAGE;
				response.content_size = strlen(response.content);
				
				while (current_client){
					char* client_id = current_client->value;
					respond_to_message(&response, client_id, server);
					current_client = current_client->next;
				}

<<<<<<< HEAD
=======
			}
			// client wants list of available users to message from
			// respective group
			else if (message->type == DISPLAY_USERS){
				// use list_to_str
				char* group_id = (char*)ht_get(clients, message->sender);
				LinkedList* user_list = (LinkedList*)ht_get(group_to_clients, group_id);
				char* conversion = ll_str(user_list);
				size_t len = strlen(conversion);

				strncpy(response.content, conversion, len);
				response.content_size = len;
				free(conversion);

				respond_to_message(&response, sender_name, server);
			}
			// client is leaving the server
			else if (message->type == LEAVE_GROUP){
				strcpy(response.sender, SERVER_NAME);
                strcpy(response.content, "Session has been disconnected. Bye!");
                response.content_size = strlen(response.content);
				respond_to_message(&response, sender_name, server);
			}
			// print error
			else {
				fprintf(stderr, "Error with message type.\n");
				exit(EXIT_FAILURE);
			}

            printf("%s\n", message->content);

>>>>>>> 05651a86c6b1e6d8b3065ded2a87e0c82131f2e6
        }
        sleep(1);
    }

    free_node(server);
    printf("Successfully quit server.\n");

    return 0;
}
