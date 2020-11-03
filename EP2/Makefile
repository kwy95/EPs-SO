CC=gcc
EXE ?= raceSim
CFLAGS=-std=c99 -Wall
DEBUG=-g -O0
LDFLAGS= -lm -lpthread
# INCLUDES=-I/usr/lib/x86_64-linux-gnu/openmpi/include/

main: raceSim
	rm -f *.o

raceSim: ep2.c util.c util.h
	$(CC) $(CFLAGS) $^ -o $(EXE) $(DEBUG) $(LDFLAGS)


# util: util.o
# 	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

util.o: util.c util.h
	$(CC) $(CFLAGS) -c $<


clean:
	rm -f *.o raceSim
