
#include "util.h"

#include <stdio.h>
#include <string.h>


void checkPtr(void* ptr) {
    if (ptr == NULL) {
        printf("Pointer error!\n");
        exit(1);
    }
}

/**
 * Implementação simples de lista ligada
 */
Lista CriaLista() {
    Lista L = (Lista) malloc(sizeof(struct lst)); checkPtr(L);
    L->inicio = NULL;
    return(L);
}
void InsereInicio(Lista L, char* v) {
    Node novo = (Node) malloc(sizeof(struct nd)); checkPtr(novo);
    novo->valor = v;
    novo->rank = L->inicio != NULL ? L->inicio->rank + 1 : 0;
    novo->prox = L->inicio;
    L->inicio = novo;
}
int rank(Lista L, const char* v) {
    Node atual = L->inicio;
    if (L == NULL) {
        return -1;
    }
    while (atual != NULL) {
        if (strcmp(atual->valor, v)) {
            return atual->rank;
        }
        atual = atual->prox;
    }
    return -1;
}
void DestroiLista(Lista L) {
    Node novo  = L->inicio;
    Node atual = novo;
    while(novo->prox != NULL) {
        atual = novo;
        novo  = novo->prox;
        free(atual);
    }
    free(novo);
    free(L);
    L = NULL;
}
void ImprimeLista(Lista L) {
    Node atual = L->inicio;
    while(atual != NULL) {
        printf("r: %d | %s; ", atual->rank, atual->valor);
        atual = atual->prox;
    }
    printf("\n");
}

int test_funcs() {
    char* n[] = {"teste", "lalala", "alo", "onde", "hello"};
    char* m[] = {"como", "lalala", "aqui", "teste", "hello"};

    Lista L = CriaLista();
    Lista LL = CriaLista();
    printf("criou lista\n");
    for (int i=0; i<5; i+=1) {
        InsereInicio(L, n[i]);
        InsereInicio(LL, m[i]);
    }
    printf("inseriu\n");
    ImprimeLista(L);
    ImprimeLista(LL);
    printf("L: %p\n", (void*) L);
    for (int i = 0; i < 5; i++) {
        int r = rank(L, m[i]);
        printf("[%d]: %d; ", i, r);
    }
    printf("\n");

    DestroiLista(L);
    DestroiLista(LL);
    return(0);
}
