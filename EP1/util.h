#define _GNU_SOURCE

#ifndef _UTIL_H
#define _UTIL_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #define ABS(x) ((x) < 0 ? -(x) : (x))
#define TSCALE 1000000000L
#define QUANTUM 500000000L

void checkPtr(void*);

typedef struct trace {
    char* nome;
    int to;
    long dt;
    int deadline;
    // ---------
    long elapsed;
    long nelapsed;
    int id;
} trace;
typedef struct trace* Trace;

Trace novoTrace(char*);
void atualizarTrace(Trace, struct timespec);
void destroiTrace(Trace);
void destroiTraceA(Trace*, int);

typedef struct fila {
    Trace* traces;
    int size;
    int space;
    int first;
    int last;
    int n_id;
} fila;
typedef struct fila* Fila;

Fila CriaFila();
void enqueue(Fila, Trace);
Trace dequeue(Fila);
Trace peek(Fila);
void DestroiFila(Fila);
void ImprimeFila(Fila);

int time_dif(struct timespec, struct timespec);

int test_funcs();

#endif
