#include "server.h"

/**
 * Server constants.
 */
static const int server_permissions = IPC_CREAT | READ | WRITE;
static int kill_server = 0;

#define VOIDIFY_FUNC(var) (void* (*)(void*))var
#define VOIDIFY_FUNC2(var) (void (*)(void*))var
#define COMP_FUNC(var) (int (*)(void*, void*))var

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

static int str_equal(const char* str1, const char* str2){
    return !strcmp(str1, str2);
}

static Node* server;


void sigterm_handler(int signum){
    kill_server = 1;
    fprintf(stderr, "Exiting server. Attempting to free server server.\n");
    free_node(server);
}


int main(int argc, char* argv[]){
    // Only 1 server should be up
    if (server_is_up()){
        fprintf(stderr, "Another instance of this server has already been started. Multiple instances cannot be created.\n");
        exit(EXIT_FAILURE);
    }

    // Create the server server
    server = create_node(SERVER_NAME, DEFAULT_SHM_SIZE, server_permissions);
    server->status = AVAILABLE;

    // Attempt to free server when catching unexpected exit
    signal(SIGINT, sigterm_handler);
    signal(SIGTERM, sigterm_handler);

	// Hash table creation for storing groupID -> [client1, client2,...]
	int max_size = 10;
    // client: group
	Hashtable* clients = ht_create(max_size, free, VOIDIFY_FUNC(str_copy));
    // group: [clients]
	Hashtable* group_to_clients = ht_create(max_size, VOIDIFY_FUNC2(ll_free), VOIDIFY_FUNC(ll_copy));
	int client_count = 0;

    while (!kill_server){
        // Wait for messages
        if (server->status == RECIEVED_MESSAGE){
            // Handle the message
            Message* message = (Message*)(&(server->buffer));
			const char* sender_name = message->sender;

			Message response;
			strcpy(response.sender, SERVER_NAME);

			// client is new and wants to join a group
			if (message->type == JOIN_CREATE_GROUP){
				response.type = JOIN_CREATE_GROUP;
                char* group_id = message->content;
				
                // add new group/client to group in hash table
                LinkedList* list = (LinkedList*)ht_get(group_to_clients, group_id);

				// check if client count has hit capacity
				if (client_count < max_size){
					if ( !list ){
                        // Group does not exist. Create one.
						list = ll_create(free, VOIDIFY_FUNC(str_copy), COMP_FUNC(str_equal));
						ll_prepend(list, message->sender);
						ht_set(group_to_clients, group_id, list);
						
						strcpy(response.content, "\0");
					}
					else{
                        // else just update the key's list
						ll_prepend(list, message->sender);
						strcpy(response.content, "\0");
					}
                    ht_set(clients, sender_name, group_id);
                    client_count++;
				}
				// send response to user that max limit is reached
				else{
					response.type = LEAVE_GROUP;
                    strcpy(response.content, "Server is at maximum capacity. Please end session.");
				}
                response.content_size = strlen(response.content);
                respond_to_message(&response, sender_name, server);

                char* s = ll_str(list);
                printf("clients for group '%s': %s\n", group_id, s);
                free(s);
			}

			// client wants to message the group
			else if (message->type == BROADCAST_MESSAGE){
                printf("Recieved broadcast from '%s'.\n", sender_name);
                printf("%s\n", message->content);

				// iterate through hashtable groupID->values to write
				// broadcast message
				char* group_id = (char*)ht_get(clients, message->sender);
                printf("group id: %s\n", group_id);
				LinkedList* list = (LinkedList*)ht_get(group_to_clients, group_id);
                char* s = ll_str(list);
                printf("%s\n", s);
                free(s);
				LinkedListNode* current_client = list->head;
				
                // Acknowledgement
				strcpy(response.content, "");
				response.type = BROADCAST_MESSAGE;
				response.content_size = strlen(response.content);
                respond_to_message(&response, sender_name, server);
                // Wait for the sender to become available before sending messages.
                Node* node = create_node(SERVER_NAME, DEFAULT_SHM_SIZE, server_permissions);
                while (node->status != AVAILABLE){
                    printf("Waiting for '%s' to be available before broadcasting.\n", sender_name);
                    sleep(1);
                }
                detatch_node(node);
                
                printf("sent ack.\n");

                // Response to group
				strcpy(response.content, message->content);
				response.type = BROADCAST_MESSAGE;
				response.content_size = strlen(response.content);

				while (current_client){
					char* client_id = current_client->value;
                    printf("Broadcast to '%s'.\n", client_id);
					send_message(&response, client_id, server);
                    printf("Done Broadcast to '%s'.\n", client_id);
					current_client = current_client->next;
				}
                printf("Done responding to clients.\n");
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
                char* group_id = ht_get(clients, sender_name);
                LinkedList* ll = ht_get(group_to_clients, group_id);
                printf("removing '%s' from '%s'.\n", sender_name, group_id);
                char* s = ll_str(ll);
                printf("existing: %s\n", s);
                free(s);

                ht_remove(clients, sender_name);
                ll_remove_value(ll, (void*)sender_name);
                client_count--;
                s = ll_str(ll);
                printf("after: %s\n", s);
                free(s);

                strcpy(response.content, "Session has been disconnected. Bye!");
                response.content_size = strlen(response.content);
				respond_to_message(&response, sender_name, server);
			}
            else if (message->type == PRIVATE_MESSAGE){
                strcpy(response.content, "The server does not accept private messages.\n");
                response.content_size = strlen(response.content);
				respond_to_message(&response, sender_name, server);
            }
			// print error
			else {
                strcpy(response.content, "Unknown message type.");
                response.content_size = strlen(response.content);
				respond_to_message(&response, sender_name, server);
			}

            printf("%s\n", message->content);

        }
        sleep(1);
    }

    printf("Successfully quit server.\n");

    return 0;
}
