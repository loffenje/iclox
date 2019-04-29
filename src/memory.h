#ifndef MEMORY_H
#define MEMORY_H

#define POOL_BLOCKS_INITIAL 1

typedef struct memory_pool_freed_t {
    struct memory_pool_freed_t *next;
} memory_pool_freed;

typedef struct memory_pool_t {
    uint32_t elem_size;
    uint32_t block_size;
    uint32_t used;
    int32_t block;
    memory_pool_freed *freed;
    uint32_t blocks_used;
    uint8_t **blocks;
} memory_pool;

void memory_pool_init(memory_pool *mp, const uint32_t elem_size, const uint32_t block_size);
void memory_pool_destroy(memory_pool *mp);

void *memory_pool_alloc(memory_pool *mp);
void *memory_pool_alloc_arena(memory_pool *mp, size_t size);
void memory_pool_free(memory_pool *mp, void *ptr);

void memory_pool_free_all(memory_pool *mp);

#endif