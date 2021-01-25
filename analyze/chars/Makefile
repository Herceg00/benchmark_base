
CC=g++
EXEC=out
CPPFLAGS=-O2

all: src/*.h src/*.cpp
	$(CC) $(CPPFLAGS) -o $(EXEC) src/*.cpp
	mv $(EXEC) bin/

clean:
	rm -f bin/*.txt *bin/*.log *.csv

clean_all: clean
	rm -f bin/$(EXEC)

backup:
	tar -czf backup/$(shell date +"%d_%m_%y").tgz all_run.sh input Makefile README reduce res run src
