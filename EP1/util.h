#define  _GNU_SOURCE

#ifndef _UTIL_H
#define _UTIL_H

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <readline/readline.h>
#include <readline/history.h>

// #define ABS(x) ((x) < 0 ? -(x) : (x))

typedef struct trace {
    char* nome;
    int to;
    int dt;
    int deadline;
    int elapsed;
} trace;
typedef struct trace* Trace;

Trace novoTrace(char*);
void atualizarTrace(Trace, int);
void destroiTrace(Trace);
void destroiTraceA(Trace*, int);

typedef struct arg {
    Trace process;
    pthread_mutex_t cpu_access;
} arg;
typedef struct arg* Args;

typedef struct fila {
    Trace* traces;
    int size;
    int space;
    int first;
    int last;
} fila;
typedef struct fila* Fila;

Fila CriaFila();
void enqueue(Fila, Trace);
Trace dequeue(Fila);
void DestroiFila(Fila);
void ImprimeFila(Fila);

int test_funcs();

#endif
