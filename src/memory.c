#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "memory.h"


void *glob_base_ptr = NULL;
pthread_mutex_t memorylock = PTHREAD_MUTEX_INITIALIZER;

static int page_size = -1;
static int page_count = 16;

static memory_block *find_free_block(memory_block **last, size_t size) {
    memory_block *curr = glob_base_ptr;
    while (curr && !(curr->_free && curr->size >= size)) {
        *last = curr;
        curr = curr->next;
    }

    return curr;
}

static memory_block *map_block(memory_block *last, size_t size) {
    memory_block *block;
    char *p = MMAP(size);
    if (p == MAP_FAILED) {
        return NULL;
    }

    if (mprotect(p, size, PROT_READ | PROT_WRITE) != 0) {
        munmap(p, size);
        return NULL;
    }

    block = (memory_block *)p;
    if (last) {
        last->next = block;
    }

    block->size = size;
    block->_free = 0;
    block->next = NULL;

    return block;
}


void *memory_malloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }

    MLOCK();

    memory_block *block;
    size_t asize, total_size;
    int alloc_size = 0, pages = 0;

    asize = MAX(ALIGN(size), MINIMUM);

    if (asize < PAGE_SIZE) {
        alloc_size = asize;
    } else {
        total_size = asize + MEM_BLOCK_SIZE;
        pages = total_size / PAGE_SIZE;
        if ((pages % PAGE_SIZE) != 0) pages += 1;
        if (pages < page_count) pages = page_count;
        alloc_size = memory_pagesize(pages);
    }

    if (!glob_base_ptr) {
        block = map_block(NULL, alloc_size);
        if (!block) {
            MUNLOCK();
            return NULL;
        }

        glob_base_ptr = block;
    } else {
        memory_block *last = glob_base_ptr;
        block = find_free_block(&last, alloc_size);
        if (!block) {
            block = map_block(last, alloc_size);
            if (!block) {
                MUNLOCK();
                return NULL;
            }
        } else {
            block->_free = 0;
        }
    }

    MUNLOCK();

    return block + 1;
}

void *memory_realloc(void *ptr, size_t size)
{
    size_t asize, total_size;
    int pages = 0, alloc_size = 0;

    if (!ptr) {
        return memory_malloc(size);
    }

    asize = MAX(ALIGN(size), MINIMUM);

    if (asize >= PAGE_SIZE) {
        total_size = asize + MEM_BLOCK_SIZE;
        pages = total_size / PAGE_SIZE;
        if ((pages % PAGE_SIZE) != 0) pages += 1;
        if (pages < page_count) pages = page_count;
        alloc_size = memory_pagesize(pages);
    } else {
        alloc_size = asize;
    }

    memory_block *block = (memory_block *)ptr - 1;

    if (block->size >= alloc_size) {
        return ptr;
    }

    void *new_ptr;
    new_ptr = memory_malloc(size);
    if (!new_ptr) {
        return NULL;
    }


    memcpy(new_ptr, ptr, block->size);
    free(ptr);


    return new_ptr;
}

void *memory_calloc(size_t num, size_t nsize) {
    size_t size;
    void *ptr;
    if (!num || !nsize) {
        return NULL;
    }

    size = num * nsize;
    ptr = malloc(size);
    if (!ptr) {
        return NULL;
    }

    memset(ptr, 0, size);

    return ptr;
}

size_t memory_pagesize(size_t pages) {
    if (page_size < 0) page_size = getpagesize();

    return page_size * pages;
}

void memory_free(void *ptr) {
    if (!ptr) return;

    memory_block *block = (memory_block *)ptr - 1;
    size_t pages = block->size / PAGE_SIZE;

	if ((pages % PAGE_SIZE) != 0) pages += 1;
	if (pages < PAGE_SIZE) pages = page_count;

    munmap(ptr, pages * page_size);

    assert(block->_free == 0);
    block->_free = 1;
}