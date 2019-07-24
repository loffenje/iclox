#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#include "memory.h"

#define DEFAULT_CAPACITY 16

#define MAP_ERR -1
#define MAP_FULL -2
#define MAP_MISSING -3
#define MAP_OK 0

#define MAX_CHAIN_LENGTH 8

#define INITIAL_SIZE (256)

#define MAX(a,b) ((a) < (b) ? (b) : (a))

typedef int (*func)(void *, void *);

typedef void* Object;

typedef struct map_bucket_t {
    void *key;
    void *val;
    int in_use;
} map_bucket;

typedef struct map_t {
    size_t len;
    size_t cap;
    map_bucket *buckets;
} map;

typedef struct vect_t {
    void **data;
    size_t len;
    size_t cap;
} vect;


void *vect_new();
void *vect_alloc(size_t size);
void vect_resize(vect *v, size_t size);
void *vect_at(vect *v, int index);
void vect_shrink_to_fit(vect *v);
size_t vect_size(vect *v);
size_t vect_len(vect *v);

void *vect_begin(vect *v);
void *vect_end(vect *v);
void vect_next(vect *v);

void vect_push_back(vect *v, void *item);
void *vect_pop_back(vect *v);
bool vect_contains(vect *v, void *item);
void vect_pushi_back(vect *v, int item);
void vect_free(vect *v);

void die(char *msg);

void *xmalloc(size_t size);
void *xcalloc(size_t n, size_t size);

void report(const char *fmt, ...);

extern unsigned long crcr32_tab[];

map *map_new();
void map_destroy(map *);
int map_put(map *, void *, void *);
int map_puti(map *m, void *key, int value);
int map_del(map *, void *);
void *map_get(map *, void *);
map_bucket *map_get_bucket(map *, void *);

#endif
