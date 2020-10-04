
#ifndef _FILA_H
#define _FILA_H

#include "util.h"

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

int test_fila();

#endif
