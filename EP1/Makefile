CC=gcc
EXE ?= bccsh
CFLAGS=-ansi -Wall
DEBUG=-ggdb
LDFLAGS=-lreadline
# INCLUDES=-I/usr/lib/x86_64-linux-gnu/openmpi/include/

main: bccsh

bccsh: bccsh.o
	$(CC) $(DEBUG) -o $(EXE) $^

bccsh.o: bccsh.c
	$(CC) $(DEBUG) -c $<
clean:
	rm -f *.o bccsh
