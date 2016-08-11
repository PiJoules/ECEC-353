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

// Place of shared memory where the server will initiually write to and
// clients can listen to
#define UNIVERSAL_KEY 1337

// Default size of each shared memory block
#define SHM_SIZE 1024

// Permissions
#define READ 0x4
#define WRITE 0x2

/**
 * Wrapper functions for treating shared memory more like heap memory.
 */
void* shmalloc(size_t size, int shmflag);
void shmfree();


#endif
