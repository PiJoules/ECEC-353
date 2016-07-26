#ifndef _QUEUE_H
#define _QUEUE_H

#define MAX_LENGTH 1024
#define TRUE 1
#define FALSE 0

/* Data type for queue element. */
typedef struct queue_element_tag{
    char path_name[MAX_LENGTH];                 /* Stores the path corresponding to the file/directory. */
    struct queue_element_tag *next;
} queue_element_t;

typedef struct queue_tag{
          queue_element_t *head;
          queue_element_t *tail;
} queue_t;


/* Function definitions. */
queue_t *createQueue(void);
void insertElement(queue_t *, queue_element_t *);
queue_element_t *removeElement(queue_t *);
void printQueue(queue_t *);

#endif
