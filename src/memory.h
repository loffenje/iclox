#ifndef MEMORY_H
#define MEMORY_H

#include <pthread.h>

#define ALIGNMENT 8
#define PAGE_SIZE (1 << 12)
#define MINIMUM  24  /* minimum block size */

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)
#define MMAP(sz) mmap(NULL, (size_t)(sz), PROT_READ | PROT_WRITE, \
    MAP_ANON | MAP_PRIVATE, -1, (off_t) 0)

#define malloc memory_malloc
#define realloc memory_realloc
#define calloc memory_calloc
#define free memory_free

typedef struct memory_block_t {
    size_t size;
    struct memory_block_t *next;
    int _free;
} memory_block;

#define MEM_BLOCK_SIZE sizeof(memory_block)

extern void *glob_base_ptr;
extern pthread_mutex_t memorylock;


#define MLOCK(void) do { \
    pthread_mutex_lock(&memorylock); \
} while(0)

#define MUNLOCK(void) do { \
    pthread_mutex_unlock(&memorylock); \
} while(0)


void *memory_malloc(size_t size);
void *memory_realloc(void *ptr, size_t size);
void *memory_calloc(size_t num, size_t nsize);
void memory_free(void *ptr);

size_t memory_pagesize(size_t pages);

#endif