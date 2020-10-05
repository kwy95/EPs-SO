
#define _GNU_SOURCE

#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TSCALE 1000000000L
#define QUANTUM 500000000L

void checkPtr(void*);

int time_dif(struct timespec, struct timespec);

typedef struct trace {
    char* nome;
    int t0;
    long dt;
    int deadline;
    // ---------
    long remaining;
    long nremaining;
    int id;
} trace;
typedef struct trace* Trace;

Trace novoTrace(char*);
void atualizarTrace(Trace, struct timespec);
void destroiTrace(Trace);
void destroiTraceA(Trace*, int);
int trace_done(Trace);

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
int empty(Fila);
double remaining(Trace);

Trace peek(Fila);

void insert(Fila, Trace);
Trace get_min(Fila);

void enqueue(Fila, Trace);
Trace dequeue(Fila);

void DestroiFila(Fila);
void ImprimeFila(Fila);

int test_funcs();

#endif
