CC=gcc
EXESH ?= bccsh
EXEEP ?= ep1
CFLAGS=-std=c99 -Wall
DEBUG=-g -O0
LDFLAGS=-lreadline -lpthread
# INCLUDES=-I/usr/lib/x86_64-linux-gnu/openmpi/include/

main: bccsh ep1
	rm -f *.o

bccsh: bccsh.o util.o
	$(CC) $(DEBUG) -o $(EXESH) $^ $(LDFLAGS)

ep1: ep1.o util.o
	$(CC) $(DEBUG) -o $(EXEEP) $^ $(LDFLAGS)


bccsh.o: bccsh.c util.h
	$(CC) $(DEBUG) -c $<

ep1.o: ep1.c util.h
	$(CC) $(DEBUG) -c $<


util: util.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

util.o: util.c util.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o bccsh
