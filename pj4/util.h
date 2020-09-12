#ifndef _UTIL_H_
#define _UTIL_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FALSE 0
#define TRUE  1

#define TYPE_READ  'R'
#define TYPE_WRITE 'W'

#define BYTES_PER_WORD 4

extern int DUMP_CACHE_CONTENT;
extern int total_reads, total_writes, write_backs,
       reads_hits, write_hits, reads_misses, write_misses, write_backs;

char** str_split(char *a_str, const char a_delim);
void cdump(int capacity, int assoc, int blocksize);
void sdump(int total_reads, int total_writes, int write_backs,
           int reads_hits, int write_hits, int reads_misses, int write_misses);
void xdump(int set, int way, uint32_t** cache);

#endif
