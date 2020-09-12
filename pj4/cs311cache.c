/***************************************************************/
/*                                                             */
/* CS311 Project 4 : Cache Design                              */
/*                                                             */
/* TEAM 14                                                     */
/* 20120533 Juho Sun                                           */
/* 20130551 Youngjae Chang                                     */
/*                                                             */
/***************************************************************/

#include "util.h"
#include "cache.h"

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/***************************************************************/
void initialize() {
    init_cache();
    total_reads = total_writes = write_backs = 0;
    reads_hits = write_hits = reads_misses = write_misses = 0;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {

    char** tokens;
    int count = 1;
    int i, j, k;

    /* File reading */
    char *filename;
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    /* Trace info */
    char type;
    uint32_t addr;

    /* Default cache settings */
    capacity = 256;
    assoc = 4;
    blocksize = 8;
    set = capacity / assoc / blocksize;
    DUMP_CACHE_CONTENT = FALSE;

    /* Error checking */
    if (argc < 2) {
        printf("Error: usage: %s [-c cap:assoc:bsize] [-x] input_trace\n", argv[0]);
        exit(1);
    }

    /* Argument parsing */
    while (count != argc - 1) {
        if (strcmp(argv[count], "-c") == 0) {
            tokens = str_split(argv[++count], ':');
            /* get args */
            capacity = atoi(tokens[0]);
            assoc = atoi(tokens[1]);
            blocksize = atoi(tokens[2]);
            set = capacity / assoc / blocksize;
        }
        else if (strcmp(argv[count], "-x") == 0)
            DUMP_CACHE_CONTENT = TRUE;
        else {
            printf("Error: usage: %s [-c cap:assoc:bsize] [-x] input_trace\n", argv[0]);
            exit(1);
        }
        count++;
    }

    /* Initialize cache & stats */
    initialize();

    /* Load trace file */
    filename = argv[argc-1];
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Can't open program file %s\n", filename);
        exit(-1);
    }

    /* Execute the trace */
    int idx, hit_flag;
    while ((read = getline(&line, &len, fp)) != -1) {
        type = line[0];
        addr = (uint32_t) strtol(&line[2], NULL, 16);
        /* read or write according to type */
        if (type == TYPE_READ) cache_read(addr);
        if (type == TYPE_WRITE) cache_write(addr);
    }

    /* Dump statistics */
    cdump(capacity, assoc, blocksize);
    sdump(total_reads, total_writes, write_backs,
          reads_hits, write_hits, reads_misses, write_misses);
    if (DUMP_CACHE_CONTENT) {
        xdump(set, assoc, cache);
    }

    /* Free the cache, close the file */
    for (i = 0; i < set; i++) {
        free(cache[i]);
    }
    free(cache);
    fclose(fp);

    return 0;
}
