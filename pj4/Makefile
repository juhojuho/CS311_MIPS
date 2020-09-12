CC = clang

all: cs311cache

cs311cache: cs311cache.c cache.c util.c
	$(CC) -g -O2 $^ -o $@

.PHONY: clean
clean:
	rm -rf cs311cache cs311cache.dSYM
