#include <string.h>
#include <stdlib.h>

#include "memory.h"
#include "utils.h"

void memory_pool_init(memory_pool *mp, const uint32_t elem_size, const uint32_t block_size)
{
    uint32_t i;

    mp->elem_size = MAX(elem_size, sizeof(memory_pool_freed));
    mp->block_size = block_size;

    memory_pool_free_all(mp);

    mp->blocks_used = POOL_BLOCKS_INITIAL;
    mp->blocks = xmalloc(sizeof(uint8_t*)* mp->blocks_used);

    for (i = 0; i < mp->blocks_used; ++i)
        mp->blocks[i] = NULL;
}

void memory_pool_destroy(memory_pool *mp)
{
    uint32_t i;
    for (i = 0; i < mp->blocks_used; ++i) {
        if (mp->blocks[i] == NULL)
            break;
        else
            free(mp->blocks);
    }
}

void *memory_pool_alloc_arena(memory_pool *mp, size_t size)
{
    if (mp->blocks_used != POOL_BLOCKS_INITIAL) {
        enum {DEFAULT_BLOCK = 8};
        memory_pool_init(mp, size, DEFAULT_BLOCK);
    }
    
    return memory_pool_alloc(mp);
}

void *memory_pool_alloc(memory_pool *mp)
{
    if (mp->freed != NULL) {
        void *recycle = mp->freed;
        mp->freed = mp->freed->next;

        return recycle;
    }

    if (++mp->used == mp->block_size) {
        mp->used = 0;
        if (++mp->block == (int32_t)mp->blocks_used) {
            uint32_t i;
            mp->blocks_used <<= 1;
            mp->blocks = realloc(mp->blocks, sizeof(uint8_t *) * mp->blocks_used);

            for (i = mp->blocks_used >> 1; i < mp->blocks_used; ++i)
                mp->blocks[i] = NULL;
        }

        if (mp->blocks[mp->block] == NULL)
            mp->blocks[mp->block] = malloc(mp->elem_size * mp->block_size);
    }

    return mp->blocks[mp->block] + mp->used * mp->elem_size;
}

void memory_pool_free(memory_pool *mp, void *ptr)
{
    memory_pool_freed *mp_freed = mp->freed;

    mp->freed = ptr;
    mp->freed->next = mp_freed;
}

void memory_pool_free_all(memory_pool *mp)
{
    mp->used = mp->block_size - 1;
    mp->block = -1;
    mp->freed = NULL;
}