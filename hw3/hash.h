#ifndef HASH_H
#define HASH_H

typedef struct entry_s entry_t;
typedef struct hashtable_s hashtable_t;

struct entry_s {
	char *key;
	char *value;
	struct entry_s *next;
};

struct hashtable_s {
	size_t size;
	struct entry_s **table;	
};


typedef struct List List;
typedef struct ListNode ListNode;

struct ListNode {
    char* value;
    ListNode* next;
};

struct List {
    size_t size;
    ListNode* head;
};


List* create_list();
void free_list(List* list);
void append_to_list(List* list, char* value);
int value_in_list(List* list, char* value);
void remove_from_list(List* list, int i);
char* item_at(List* list, int i);
char* list_to_str(List* list);  // TODO: Return malloc'd string


// Function declarations
hashtable_t *ht_create(size_t size);
int ht_hash( hashtable_t *hashtable, const char *key);
entry_t *ht_newpair(const char *key, List* value);

#endif
