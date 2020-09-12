#include "cache.h"

/* LRU update situations */
#define LRU_SITU_INSERT 0
#define LRU_SITU_UPDATE 1
#define LRU_SITU_DELETE 2

/***************************************************************/
/* Cache settings                                              */
/***************************************************************/
int capacity, assoc, blocksize, set;

/***************************************************************/
/* Cache data structures                                       */
/***************************************************************/
uint32_t **cache;
block_aux **cache_aux;

/***************************************************************/
/* Helper functions                                            */
/***************************************************************/
int allocate_cache(int set);
void evict_cache(int set, int way);
void update_lru_order(int set, int way, int situation);

/***************************************************************/
/*                                                             */
/* Procedure : init_cache                                      */
/*                                                             */
/***************************************************************/
void init_cache() {
    int i;
    int set = capacity / assoc / blocksize;
    cache     = (uint32_t**)  malloc(sizeof(uint32_t*) * set);
    cache_aux = (block_aux**) malloc(sizeof(block_aux*) * set);
    for (i = 0; i < set; i++) {
        cache[i]     = (uint32_t*)  calloc(assoc, sizeof(uint32_t));
        cache_aux[i] = (block_aux*) calloc(assoc, sizeof(block_aux));
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : cache_read                                      */
/*                                                             */
/***************************************************************/
void cache_read(uint32_t addr) {
    int i, idx, victim;
    total_reads++;
    /* mask block size bits with 0 */
    addr = addr & ~(blocksize - 1);
    /* find index */
    idx = (addr / blocksize) % set;
    /* compare tags */
    for (i = 0; i < assoc; i++) {
        if (addr == cache[idx][i] && cache_aux[idx][i].valid) {
            /* HIT */
            reads_hits++;
            update_lru_order(idx, i, LRU_SITU_UPDATE);
            return;
        }
    }
    /* MISS */
    reads_misses++;
    victim = allocate_cache(idx);
    cache[idx][victim] = addr;
    cache_aux[idx][victim].valid = TRUE;
    update_lru_order(idx, victim, LRU_SITU_INSERT);
}

/***************************************************************/
/*                                                             */
/* Procedure : cache_write                                     */
/*                                                             */
/***************************************************************/
void cache_write(uint32_t addr) {
    int i, idx, victim;
    total_writes++;
    /* mask block size bits with 0 */
    addr = addr & ~(blocksize - 1);
    /* find index */
    idx = (addr / blocksize) % set;
    /* compare tags */
    for (i = 0; i < assoc; i++) {
        if (addr == cache[idx][i] && cache_aux[idx][i].valid) {
            /* HIT */
            write_hits++;
            cache[idx][i] = addr;
            cache_aux[idx][i].dirty = TRUE;
            update_lru_order(idx, i, LRU_SITU_UPDATE);
            return;
        }
    }
    /* MISS */
    write_misses++;
    victim = allocate_cache(idx);
    cache[idx][victim] = addr;
    cache_aux[idx][victim].valid = TRUE;
    cache_aux[idx][victim].dirty = TRUE;
    update_lru_order(idx, victim, LRU_SITU_INSERT);
}

/***************************************************************/
/*                                                             */
/* Procedure : allocate_cache                                  */
/*                                                             */
/***************************************************************/
int allocate_cache(int line) {
    int i, cur;
    int max = 0, max_idx = -1;

    //DEBUG/////////////////////////////////////////////////
    /* printf("DEBUG :: set[%d] ", line);                 */
    /* for (i=0; i<assoc; i++) {                          */
    /*     if (cache_aux[line][i].valid)                  */
    /*       printf("%d ", cache_aux[line][i].lru_order); */
    /*     else                                           */
    /*       printf(". ");                                */
    /* }                                                  */
    /* printf("\n");                                      */
    ////////////////////////////////////////////////////////

    for (i = 0; i < assoc; i++) {
        /* if there exists invalid block, return it */
        if (!cache_aux[line][i].valid) return i;
        /* find max order block */
        cur = cache_aux[line][i].lru_order;
        if (cur > max) { max = cur; max_idx = i; }
    }
    evict_cache(line, max_idx);
    return max_idx;
}

/***************************************************************/
/*                                                             */
/* Procedure : evict_cache                                     */
/*                                                             */
/***************************************************************/
void evict_cache(int line, int way) {
    block_aux *aux = &cache_aux[line][way];
    if (!aux->valid) return;
    if (aux->dirty) write_backs++;
    /* reset block aux info */
    update_lru_order(line, way, LRU_SITU_DELETE);
    aux->valid = FALSE;
    aux->dirty = FALSE;
    aux->lru_order = 0;
}

/***************************************************************/
/*                                                             */
/* Procedure : update_lru_order                                */
/*                                                             */
/***************************************************************/
void update_lru_order(int line, int way, int situation) {
    int i, max_order;
    block_aux *aux;
    max_order = cache_aux[line][way].lru_order;
    for (i = 0; i < assoc; i++) {
        aux = &cache_aux[line][i];
        if (!aux->valid) continue;
        else if (i == way) aux->lru_order = 0;
        else {
            if (situation == LRU_SITU_INSERT) {
                aux->lru_order++;
            }
            else if (situation == LRU_SITU_UPDATE) {
                if (aux->lru_order <= max_order)
                  aux->lru_order++;
            }
            else if (situation == LRU_SITU_DELETE) {
                if (aux->lru_order > max_order)
                  aux->lru_order--;
            }
        }
    }
}
