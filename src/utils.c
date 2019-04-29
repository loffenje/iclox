
#include "utils.h"

/**** common utils ****/

void die(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void *xmalloc(size_t size)
{
    void *ret = malloc(size);
    if (!ret) {
        die("xmalloc failed");
    }

    return ret;
}

void *xcalloc(size_t n, size_t size)
{
    void *ret = calloc(n, size);
    if (!ret) {
        die("xcalloc failed");
    }

    return ret;
}

void report(const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);

    while (*fmt != '\0') {

        if (*fmt == 'l') {
            int line = va_arg(args, int);
            printf("Line: %d\n", line);
        } else if (*fmt == 'x') {
            char *ux = va_arg(args, char*);
            printf("Error: Unexpected %s in argument list", ux);
        } else if (*fmt == 't') {
            char token = va_arg(args, int);
            printf("Error: Unexpected token %c", token);
        }

        ++fmt;
    }

    va_end(args);
} 
// end common


/**** vect implementation ****/

void *vect_new()
{
   return vect_alloc(DEFAULT_CAPACITY);
}

void *vect_alloc(size_t size) 
{
   vect *v = xmalloc(sizeof(vect));

   v->cap = size;
   v->data = xmalloc(sizeof(void *) * size);
   v->len = 0;

   return v;
}

void vect_resize(vect *v, size_t size)
{
   v->cap = size;
   v->data = realloc(v->data, sizeof(void *) * size);
}

void vect_shrink_to_fit(vect *v)
{
   assert(v->len);
   v->cap = v->len;
}

void vect_push_back(vect *v, void *item)
{
   if (v->len == v->cap) {
      v->cap *= 2;
      v = realloc(v, sizeof(void *) * v->cap);
      assert(v);
   }

      v->data[v->len++] = item; 
}

void *vect_pop_back(vect *v)
{
   assert(v->len);
   return v->data[--v->len];
}

size_t vect_size(vect *v)
{
   return v->cap;
}

size_t vect_len(vect *v)
{
   return v->len;
}

bool vect_contains(vect *v, void *item)
{
   for (int i = 0; i < v->len; i++) {
      if (v->data[i] == item) {
         return true;
      }
   }

   return false;
}

void vect_free(vect *v)
{
   free(v->data);
   free(v);
}


void vect_pushi_back(vect *v, int item)
{
   vect_push_back(v, (void *)(intptr_t)item);
} 

// end vect


/****  hashmap implementation ****/

static unsigned long crc32(const unsigned char *s, unsigned int len)
{
    unsigned int i;
    unsigned long crc32val;

    crc32val = 0;
    for (i = 0;  i < len;  i ++) {
        crc32val = crc32_tab[(crc32val ^ s[i]) & 0xff] ^ (crc32val >> 8);
    }
    return crc32val;
}

static unsigned int hash_int(map *m, char *keystr)
{
    unsigned long key = crc32((unsigned char *)(keystr), strlen(keystr));

    /* Robert Jenkins' 32 bit Mix Function */
    key += (key << 12);
    key ^= (key >> 22);
    key += (key << 4);
    key ^= (key >> 9);
    key += (key << 10);
    key ^= (key >> 2);
    key += (key << 7);
    key ^= (key >> 12);

    /* Knuth's Multiplicative Method */
    key = (key >> 3) * 2654435761;

    return key % m->cap;

}

static int hash(map *in, void *key)
{
    int curr;

    if (in->len >= (in->cap / 2)) return MAP_FULL;

    curr = hash_int(in, key);

    for (int i = 0; i < MAX_CHAIN_LENGTH; i++) {
        if (in->buckets[curr].in_use == 0) return curr;

        if (in->buckets[curr].in_use == 1 && (strcmp(in->buckets[curr].key, key) == 0)) return curr;

        curr = (curr + 1) % in->cap;
    }

    return MAP_FULL;
}

static int rehash(map *m)
{
    unsigned long old_cap;
    map_bucket *curr;

    map_bucket *temp = xcalloc(2 * m->cap, sizeof(map_bucket));
    if (!temp) return MAP_ERR;
    
    curr = m->buckets;
    m->buckets = temp;
    old_cap = m->cap;
    m->cap *= 2;
    m->len = 0;

    for (unsigned int i = 0; i < old_cap; i++) {
        int status;
        if (curr[i].in_use == 0) continue;

        status = map_put(m, curr[i].key, curr[i].val);
        if (status != MAP_OK) return status;
    }

    free(curr);

    return MAP_OK;
}

map *map_new()
{
    map *m = xmalloc(sizeof(map));

    m->buckets = (map_bucket *) xcalloc(INITIAL_SIZE, sizeof(map_bucket));
    if (!m->buckets) {
        if (m) map_destroy(m);

        return NULL;
    }

    m->cap = INITIAL_SIZE;
    m->len = 0;

    return m;
}

int map_put(map *m, void *key, void *value)
{
    int index = hash(m, key);
    while (index == MAP_FULL) {
        if (rehash(m) == MAP_ERR) return MAP_ERR;
        index = hash(m, key);
    }

    m->buckets[index].val = value;
    m->buckets[index].key = key;
    if (m->buckets[index].in_use == 0) {
        m->buckets[index].in_use = 1;
        m->len++;
    }

    return MAP_OK;
}

int map_puti(map *m, void *key, int value)
{
    return map_put(m, key, (void *)(intptr_t)value);
}

int map_del(map *m, void *key)
{
    int curr = hash_int(m, key);

    for (int i = 0; i < MAX_CHAIN_LENGTH; i++) {
        if (m->buckets[curr].in_use == 0) {
            curr = (curr + 1) % m->cap;
            continue;
        } 

        if (strcmp(m->buckets[curr].key, key) == 0) {
            m->buckets[curr].in_use = 0;
            m->len--;

            return MAP_OK;
        }
    }

    return MAP_ERR;
}

void *map_get(map *m, void *key)
{
    int curr = hash_int(m, key);

    for (int i = 0; i < MAX_CHAIN_LENGTH; i++) {
        if (m->buckets[curr].in_use == 0) {
            curr = (curr + 1) % m->cap;
            continue;
        } 

        if (strcmp(m->buckets[curr].key, key) == 0) return (m->buckets[curr].val);
    }

    return NULL;
}


map_bucket *map_get_bucket(map *m, void *key)
{
        int curr = hash_int(m, key);

    for (int i = 0; i < MAX_CHAIN_LENGTH; i++) {
        if (m->buckets[curr].in_use == 0) {
            curr = (curr + 1) % m->cap;
            continue;
        } 

        if (strcmp(m->buckets[curr].key, key) == 0) return &m->buckets[curr];
    }

    return NULL;
}

void map_destroy(map *m)
{
    free(m->buckets);
    free(m);
} // end hashmap