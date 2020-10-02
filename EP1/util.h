
#ifndef _UTIL_H
#define _UTIL_H

#include <stdlib.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))

typedef struct nd {
    char* valor;
    int rank;
    struct nd* prox;
} node;
typedef node* Node;

typedef struct lst {
    Node inicio;
} lista;
typedef lista* Lista;

void checkPtr(void*);

Lista CriaLista();
void InsereInicio(Lista, char*);
int rank(Lista, const char*);
void DestroiLista(Lista);
void ImprimeLista(Lista);

int test_funcs();

#endif
