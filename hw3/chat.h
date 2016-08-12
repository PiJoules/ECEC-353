#ifndef CHAT_H
#define CHAT_H

/**
 * Header to contain shared stuff between files.
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

// Place of shared memory where the server will initiually write to and
// clients can listen to
#define UNIVERSAL_KEY 1234

// Default size of each shared memory block
#define SHM_SIZE 1024

// Permissions
#define READ 0444
#define WRITE 0222

/**
 * Wrapper functions for treating shared memory more like heap memory.
 */
void* shmalloc(size_t size, int shmflag);
void shmfree();


#endif
