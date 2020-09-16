CC=gcc
EXE ?= bccsh
CFLAGS=-ansi -Wall
LDFLAGS=-fopenmp -lm -lmpi
INCLUDES=-I/usr/lib/x86_64-linux-gnu/openmpi/include/

main: bccsh

bccsh: bccsh.o
	$(CC) $(CFLAGS) $(INCLUDES) -o $(EXE) $^ 

bccsh.o: bccsh.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 
clean:
	rm -f *.o bccsh
