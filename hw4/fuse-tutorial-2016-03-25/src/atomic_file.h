#ifndef ATOMIC_FILE_H
#define ATOMIC_FILE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>


/**
 * Requirements:
 * - 1 write at a time
 * - Process that creates the shared mem must also close it
 *
 * The file will be a shared memory segment that one process opening the file
 * will create and other processes will just have read/write access to.
 * The segment will have a status in it saying if it is available to write to.
 * If so, immediately set it to WRITING, print to file, then set back.
 * All processes must call atomic_file_close, but only the one that created the
 * shm segment will be clearing it.
 */


// Permissions
#define READ 0444
#define WRITE 0222

typedef struct AtomicFileNode AtomicFileNode;
typedef struct AtomicFile AtomicFile;

/**
 * AtomicFile is the struct exposed to the user to hide the implementation
 * of AtomicFileNode.
 */
struct AtomicFile {
    char* filename;
    AtomicFileNode* node;
};

// Public API functions
AtomicFile* atomic_file(const char* filename, const char* mode);
void atomic_file_write(AtomicFile*, const char*, va_list);
void atomic_file_close(AtomicFile* file);


#endif
