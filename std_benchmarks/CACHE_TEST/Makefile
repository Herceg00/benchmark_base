#VARS
CC=g++
CFLAGS=-O2
GRAIN=
#END OF VARS

all: cache_comp

cache_comp: cache.c
	$(CC) $(CFLAGS) cache.c -o _cache
wb: cache.c
	$(CC) $(CFLAGS) write_cache.c -o _cache
clean:
	rm -f *.o* *.rep* _cache res.txt
res:
	rm -f res.txt res.csv

go: cache_comp
	./_cache
