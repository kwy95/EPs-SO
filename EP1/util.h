#define _GNU_SOURCE

#ifndef _UTIL_H
#define _UTIL_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #define ABS(x) ((x) < 0 ? -(x) : (x))
#define TSCALE 1000000000L
#define QUANTUM 1000L

void checkPtr(void*);

typedef struct trace {
    char* nome;
    int to;
    int dt;
    int deadline;
    // ---------
    long elapsed;
    int id;
} trace;
typedef struct trace* Trace;

Trace novoTrace(char*);
void atualizarTrace(Trace, long);
void destroiTrace(Trace);
void destroiTraceA(Trace*, int);

typedef struct arg {
    Trace process;
    useconds_t allowed;
} arg;
typedef struct arg* Args;

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

int test_funcs();

#endif
