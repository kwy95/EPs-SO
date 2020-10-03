
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
#define MAXLEN 100
#define MAXARG 10
#define EMBLEN 4

typedef struct trace {
    char* nome;
    int to;
    int dt;
    int deadline;
};
typedef struct trace* Trace;

typedef struct arg {
    Trace process;
    pthread_mutex_t cpu_access;
};
typedef struct arg* Args;

typedef struct nd {
    char* valor;
    struct nd* next;
    struct nd* prev;
} node;
typedef node* Node;

typedef struct lst {
    Node inicio;
    Node fim;
} lista;
typedef lista* Lista;

void checkPtr(void*);

Lista CriaLista();
void InsereInicio(Lista, const char*);
void InsereFim(Lista, const char*);
int rank(Lista, const char*);
void DestroiLista(Lista);
void ImprimeLista(Lista);

int test_funcs();

#endif
