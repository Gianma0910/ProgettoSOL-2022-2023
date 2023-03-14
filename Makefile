SHELL = /bin/bash
CC = gcc
INCLUDES = -I./lib/*.h
LIBS = -lpthread
FARM_OUT = ./farm
GENERAFILE_OUT = ./generafile
FLAGS = -std=c99 -Wall -D_POSIX_C_SOURCE -D_GNU_SOURCE

farm_lib = ./libs/src/Master_Thread.c \
	   ./libs/src/Collector.c \
	   ./libs/src/Thread_Worker.c \
	   ./libs/src/utilities.c \
	   ./libs/src/queue.c \
	   ./libs/src/ascending_queue.c \
	   ./libs/src/mystring.c

.SUFFIXES: .c .h
.PHONY: all

compilation: compilation_generafile compilation_farm

compilation_generafile: generafile.c
	$(CC) generafile.c -o $(GENERAFILE_OUT)

compilation_farm: Main.c
	$(CC) $(INCLUDES) -o $(FARM_OUT) Main.c $(farm_lib) $(LIBS) $(FLAGS)

compilation_farm_debug_signals: Main.c
	$(CC) $(INCLUDES) -o $(FARM_OUT) Main.c $(farm_lib) $(LIBS) $(FLAGS) -DDEBUG

test: farm
	@chmod +x ./test.sh
	./test.sh

test1: farm
	@chmod +x ./script/test1.sh
	./script/test1.sh

test2: farm
	@chmod +x ./script/test2.sh
	./script/test2.sh

clean:
	rm *.dat expected.txt | rm -r testdir