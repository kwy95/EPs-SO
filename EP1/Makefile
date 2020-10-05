CC=gcc
EXESH ?= bccsh
EXEEP ?= ep1
CFLAGS=-std=c99 -Wall
# DEBUG=-g -O0
LDFLAGS=-lreadline -lpthread
# INCLUDES=-I/usr/lib/x86_64-linux-gnu/openmpi/include/

main: bccsh ep1
	rm -f *.o

bccsh: bccsh.c util.c util.h
	$(CC) $(CFLAGS) $^ -o $(EXESH) $(LDFLAGS)

ep1: ep1.c util.c util.h
	$(CC) $(CFLAGS) $^ -o $(EXEEP) $(LDFLAGS)

# bccsh.o: bccsh.c fila.h util.h
# 	$(CC) $(CFLAGS) $(DEBUG) -c $<

# ep1.o: ep1.c fila.h util.h
# 	$(CC) $(CFLAGS) $(DEBUG) -c $<


# util: util.o
# 	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

util.o: util.c util.h
	$(CC) $(CFLAGS) -c $<


clean:
	rm -f *.o bccsh
