#ifndef _CACHE_H_
#define _CACHE_H_

#include "util.h"

typedef struct block_aux_s {
    char valid;
    char dirty;
    short lru_order;
} block_aux;

extern int capacity;
extern int assoc;
extern int blocksize;
extern int set;

extern uint32_t **cache;
extern block_aux **cache_aux;

void init_cache();
void cache_read(uint32_t addr);
void cache_write(uint32_t addr);

#endif
