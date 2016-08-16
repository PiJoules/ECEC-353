#include "hashtable.h"

/**
 * Private functions
 */
static size_t ht_hash(const char* str);
static char* str_copy(const char* str);
static HashtableEntry* entry_for_key(Hashtable* ht, const char* key);
static void free_ht_entry(Hashtable* ht, HashtableEntry* entry);

Hashtable* ht_create(size_t size, void (*free_func)(void*), void* (*copy_func)(void*)){
    if (size < 1){
        return NULL;
    }

    // Create the hashtable
    Hashtable* ht = (Hashtable*)malloc(sizeof(Hashtable));
    if (!ht){
        perror("malloc failed");
        return NULL;
    }

    // Create the table itself
    HashtableEntry** table = (HashtableEntry**)malloc(sizeof(HashtableEntry*) * size);
    if (!ht){
        perror("malloc failed");
        return NULL;
    }

    // Initialize entries
    for (int i = 0; i < size; i++){
        table[i] = NULL;
    }

    ht->size = size;
    ht->entry_count = 0;
    ht->table = table;
    ht->free_func = free_func;
    ht->copy_func = copy_func;

    return ht;
}


static void free_ht_entry(Hashtable* ht, HashtableEntry* entry){
    ht->free_func(entry->value);
    free(entry->key);
    free(entry);
}


/**
 * Need to free the hashtable, the table in the hashtable, each of the Hashtableentrys
 * pointed to in the table, and each of the values held by each entry.
 */
void ht_free(Hashtable* ht){
    HashtableEntry** table = ht->table;
    for (int i = 0; i < ht->size; i++){
        HashtableEntry* entry = table[i];
        while (entry){
            HashtableEntry* next = entry->next;

            // Free each value + entry + key in this list of entries
            free_ht_entry(ht, entry);
            entry = next;
        }
    }
    ht->entry_count = 0;
    ht->size = 0;
    free(table);
    free(ht);
}

/**
 * djb2 by Dan Bernstein.
 * Be sure to mod the result of this by the bin size.
 */
static size_t ht_hash(const char* str){
    size_t hash = 5381;

    for (int i = 0; *(str + i); i++){
        int c = (int)str[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

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

static HashtableEntry* ht_newpair(Hashtable* ht, const char* key, void* value){
    HashtableEntry* new_pair = (HashtableEntry*)malloc(sizeof(HashtableEntry));
    if (!new_pair){
        perror("malloc failed");
        return NULL;
    }

    // Copy over the key and value
    if (!(new_pair->key = str_copy(key))){
        return NULL;
    }
    if (!(new_pair->value = ht->copy_func(value))){
        return NULL;
    }

    new_pair->next = NULL;
    return new_pair;
}

void ht_set(Hashtable* ht, const char* key, void* value){
    size_t bin = ht_hash(key) % ht->size;
    HashtableEntry* next = ht->table[bin];
    HashtableEntry* last = NULL;
    while (next && next->key && strcmp(key, next->key) > 0){
        last = next;
        next = next->next;
    }
    
    if (next && next->key && !strcmp(key, next->key)){
        // There's already a pair. Free the current value and replace the old.
        ht->free_func(next->value);
        next->value = ht->copy_func(value);
    }
    else {
        // Nope, could't find it.  Time to grow a pair.
        HashtableEntry* new_pair = ht_newpair(ht, key, value);
        
        if (next == ht->table[bin]){
            // Start of linked list in bin
            new_pair->next = next;
            ht->table[bin] = new_pair;
        }
        else if (!next){
            // End of linked list in bin
            last->next = new_pair;
        }
        else {
            // Middle of list
            new_pair->next = next;
            last->next = new_pair;
        }

        ht->entry_count++;
    }
}


static HashtableEntry* entry_for_key(Hashtable* ht, const char* key){
    size_t bin = ht_hash(key) % ht->size;
    HashtableEntry* pair = ht->table[bin];
    while (pair && pair->key && strcmp(key, pair->key) > 0){
        pair = pair->next;
    }
    if (!pair || !pair->key || strcmp(key, pair->key)){
        // Found nothing
        return NULL;
    }
    return pair;
}


void* ht_get(Hashtable* ht, const char* key){
    HashtableEntry* entry = entry_for_key(ht, key);
    if (entry){
        return entry->value;
    }
    return NULL;
}


int ht_contains(Hashtable* ht, const char* key){
    return (entry_for_key(ht, key) != NULL);
}


void ht_remove(Hashtable* ht, const char* key){
    size_t bin = ht_hash(key) % ht->size;
    HashtableEntry* pair = ht->table[bin];
    HashtableEntry* last = NULL;
    while (pair && pair->key && strcmp(key, pair->key) > 0){
        last = pair;
        pair = pair->next;
    }
    if (!pair || !pair->key || strcmp(key, pair->key)){
        // Found nothing
        return;
    }

    // Found something. Free the entry and reconnect the pairs.
    HashtableEntry* next = pair->next;
    free_ht_entry(ht, pair);
    if (!last){
        // First element in linked list
        ht->table[bin] = next;
    }
    else {
        last->next = next;
    }
    ht->entry_count--;
}
