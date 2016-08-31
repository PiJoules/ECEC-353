#include "atomic_file.h"


static void* safe_malloc(size_t n, unsigned long line)
{
    void* p = malloc(n);
    if (!p){
        fprintf(stderr, "[%s:%lu]Out of memory(%lu bytes)\n", __FILE__, line, (unsigned long)n);
        exit(EXIT_FAILURE);
    }
    return p;
}
#define SAFEMALLOC(n) safe_malloc(n, __LINE__)


static const size_t default_buff_size = 1024;

typedef struct ShmSegment ShmSegment;

typedef enum {
    AVAILABLE,  // Nothing happening to shm
    WRITING,  // Something currently being written to shm
} ShmSegmentStatus;

struct AtomicFileNode {
    // Process specific
    int did_create_shm_seg;
    FILE* file_pointer;

    // Shared mem
    ShmSegment* segment;
};

struct ShmSegment {
    key_t key;
    int id;
    ShmSegmentStatus status;
    size_t buffer_size;
    int permissions;
    void* buffer_start;  // Address of this pointer is start of shared mem. This must be at the end.
};


// Private API functions
ShmSegment* shmalloc(key_t, size_t, int);
ShmSegment* shmrealloc(ShmSegment*, size_t);
void detatch_shm_seg(ShmSegment*);
void free_shm_seg(ShmSegment*);
int shm_seg_is_up(key_t);
void write_to_shm_seg(ShmSegment*, const void*, size_t);
void* buffer_start(const ShmSegment*);
key_t str_to_key(const char*);


/**
 * djb2 by Dan Bernstein.
 */
key_t str_to_key(const char* str){
    int hash = 5381;

    int i;
    for (i = 0; *(str + i); i++){
        int c = (int)str[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return (key_t)hash;
}


/**
 * Create and initiualize the shared memory segment.
 */
ShmSegment* shmalloc(key_t key, size_t size, int permissions){
    if (!(permissions & WRITE)){
        fprintf(stderr, "The permissions given must include WRITE in order to initiualize default values for the shared memory segment.\n");
        return NULL;
    }

    size_t shm_seg_size = sizeof(ShmSegment);
    if (size < shm_seg_size){
        fprintf(stderr, "The minimum required size for a shared memory segment is %zu bytes.\n", shm_seg_size);
        return NULL;
    }

    // Actual size to allocate will be the the size of the struct and
    // the buffer size after it
    size_t total_size = shm_seg_size + size;

    // Create segment
    int shmid = shmget(key, total_size, permissions);
    if (shmid == -1){
        perror("shmget");
        fprintf(stderr, "Could not create node for key %d.\n", key);
        return NULL;
    }

    // Attach segment to local memory
    void* shm_addr = shmat(shmid, NULL, 0);
    if (!shm_addr){
        perror("shmat");
        fprintf(stderr, "Could not create node for key %d.\n", key);
        return NULL;
    }

    // Create and copy the node into shared mem
    ShmSegment* seg = (ShmSegment*)shm_addr;
    if (permissions & IPC_CREAT){
        seg->status = AVAILABLE;
    }
    seg->key = key;
    seg->id = shmid;
    seg->buffer_size = size;
    seg->permissions = permissions;

    if (shmctl(shmid, IPC_RMID, NULL) == -1){
       perror("shmctl");
       exit(EXIT_FAILURE);
    }

    return (ShmSegment*)shm_addr;
}


/**
 * Resize the shared memory segment.
 * Just delete the old one and return a new one.
 */
ShmSegment* shmrealloc(ShmSegment* seg, size_t size){
    ShmSegment* new_seg = shmalloc(seg->key, size, seg->permissions);
    free_shm_seg(seg);
    return new_seg;
}


void detatch_shm_seg(ShmSegment* seg){
    if (shmdt(seg) == -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}


void free_shm_seg(ShmSegment* seg){
    int id = seg->id;
    detatch_shm_seg(seg);
    if (shmctl(id, IPC_RMID, NULL) == -1){
       perror("shmctl");
       exit(EXIT_FAILURE);
    }
}


int shm_seg_is_up(key_t key){
    int shmid = shmget(key, sizeof(ShmSegment), READ);
    if (shmid == -1){
        return 0;
    }

    // Attach segment to local memory
    void* shm_addr = shmat(shmid, NULL, 0);
    if (!shm_addr){
        return 0;
    }

    // SHould be able to detatch segment
    if (shmdt(shm_addr) == -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    return 1;
}


/**
 * Return the address of shared memory where we can start writing.
 */
void* buffer_start(const ShmSegment* seg){
    return (void*)(&(seg->buffer_start));
}


/**
 * The atomic file will be a pointer to a shm segment.
 * If it has not yet started, que
 */
AtomicFile* atomic_file(const char* filename, const char* mode){
    // Create the shared mem
    key_t key = str_to_key(filename);
    int shm_exists = shm_seg_is_up(key);

    // Create shm segment
    int permissions = WRITE | READ;
    if (!shm_exists){
        permissions |= IPC_CREAT;
    }
    ShmSegment* seg = shmalloc(key, default_buff_size, permissions);

    // Create the file
    AtomicFile* file = (AtomicFile*)SAFEMALLOC(sizeof(AtomicFile));
    char* filename_cpy = (char*)SAFEMALLOC(strlen(filename));
    file->filename = filename_cpy;

    // Create node
    AtomicFileNode* node = (AtomicFileNode*)SAFEMALLOC(sizeof(AtomicFileNode));
    node->did_create_shm_seg = !shm_exists;
    node->file_pointer = fopen(filename, mode);

    node->segment = seg;
    file->node = node;

    return file;
}


/**
 * Wait to write.
 */
void atomic_file_write(AtomicFile* file, const char* str, va_list ap){
    ShmSegment* seg = file->node->segment;
    FILE* fp = file->node->file_pointer;
    while (seg->status != AVAILABLE){
        usleep(1000);
    }
    seg->status = WRITING;

    vfprintf(fp, str, ap);
    fflush(fp);

    seg->status = AVAILABLE;
}


void atomic_file_close(AtomicFile* file){
    AtomicFileNode* node = file->node;
    ShmSegment* seg = node->segment;
    
    detatch_shm_seg(seg);

    fclose(node->file_pointer);
    free(node);
    free(file->filename);
    free(file);
}
