
#define _GNU_SOURCE

#ifndef _UTIL_H
#define _UTIL_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TSCALE 1000000000L
#define QUANTUM 500000000L

#define VSTEP 30
#define TSTEP 20

void checkPtr(void*);
int rand_lim(int);

int time_dif(struct timespec, struct timespec);

typedef struct ciclista {
    int id;
    int vel;
    int posd;
    int posp;
    int dist;
    pthread_t thread;
} ciclista;
typedef struct ciclista* Ciclista;

Ciclista novoCiclista(int, int, int, pthread_t);
void atualizarCiclista(Ciclista, int);
void destroiCiclista(Ciclista);
void destroiCiclistaA(Ciclista*, int);


typedef struct fila {
    Ciclista* ciclistas;
    int size;
    int space;
    int first;
    int last;
    int n_id;
} fila;
typedef struct fila* Fila;

Fila CriaFila();
int empty(Fila);

void insert(Fila, Ciclista);
Ciclista get_min(Fila);

void enqueue(Fila, Ciclista);
Ciclista dequeue(Fila);

void DestroiFila(Fila);
void ImprimeFila(Fila);

int test_funcs();

#endif
