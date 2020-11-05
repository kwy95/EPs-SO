
#define _GNU_SOURCE

#ifndef _UTIL_H
#define _UTIL_H

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <signal.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TSCALE 1000000000L
#define QUANTUM      1000L

#define VSTEP 30
#define TSTEP 20
#define LANES 10

void checkPtr(void*);
int rand_lim(int lim);
int bernoulli(double p);

int time_dif(struct timespec, struct timespec);

typedef struct ciclista {
    int id;
    int t_vel;
    int vel;
    int posd;
    int posp;
    long int dist;
    int volta;
    int tf;
    int fracp;
} ciclista;
typedef struct ciclista* Ciclista;

Ciclista novoCiclista(int id, int pd, int pp);
void atualizarCiclista(Ciclista, int);
void destroiCiclista(Ciclista);
void destroiCiclistaA(Ciclista*, int size);


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
