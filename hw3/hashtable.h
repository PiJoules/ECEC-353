#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

typedef struct HashtableEntry HashtableEntry;
typedef struct Hashtable Hashtable;

struct HashtableEntry {
    char* key;
    void* value;
    HashtableEntry* next;
};

struct Hashtable {
    size_t size;
    size_t entry_count;
    void (*free_func)(void*);
    void* (*copy_func)(void*);
    HashtableEntry** table;
};

/**
 * The type of item stored must have
 * - A void function that takes a single void pointer argument:
 *   void (*free_func)(void*)
 * - A copy function that returns a duplicate representation of the value.
 *   This copy_func must return NULL on any kind of failure.
 *   void* (*copy_func)(void*)
 */

Hashtable* ht_create(size_t size, void (*free_func)(void*), void* (*copy_func)(void*));
void ht_free(Hashtable* ht);
void ht_set(Hashtable* ht, const char* key, void* value);
void* ht_get(Hashtable* ht, const char* key);
int ht_contains(Hashtable* ht, const char* key);
void ht_remove(Hashtable* ht, const char* key);

#endif
