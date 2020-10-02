CC=gcc
EXE ?= bccsh
CFLAGS=-ansi -Wall
DEBUG=-g -O0
LDFLAGS=-lreadline
# INCLUDES=-I/usr/lib/x86_64-linux-gnu/openmpi/include/

main: bccsh #ep1

bccsh: bccsh.o
	$(CC) $(DEBUG) -o $(EXE) $^ $(LDFLAGS)

#ep1: ep1.o
#	$(CC) $(DEBUG) -o $(EXE) $^ $(LDFLAGS)

bccsh.o: bccsh.c
	$(CC) $(DEBUG) -c $<
clean:
	rm -f *.o bccsh
