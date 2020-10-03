
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
    L->fim    = NULL;
    return(L);
}
void InsereInicio(Lista L, const char* v) {
    Node novo = (Node) malloc(sizeof(struct nd)); checkPtr(novo);

    novo->valor = (char*) malloc(strlen(v) + 1); checkPtr(novo->valor);
    strcpy(novo->valor, v);

    novo->next      = L->inicio;
    novo->prev      = NULL;
    L->inicio->prev = novo;
    L->inicio       = novo;
}

void InsereFim(Lista L, const char* v) {
    Node novo = (Node) malloc(sizeof(struct nd)); checkPtr(novo);

    //novo->valor  = v;
    novo->next   = NULL;
    novo->prev   = L->fim;
    L->fim->next = novo;
    L->fim       = novo;
}

// int rank(Lista L, const char* v) {
//     Node atual = L->inicio;
//     if (L == NULL) {
//         return -1;
//     }
//     while (atual != NULL) {
//         if (strcmp(atual->valor, v)) {
//             return atual->rank;
//         }
//         atual = atual->next;
//     }
//     return -1;
// }

void DestroiLista(Lista L) {
    Node novo  = L->inicio;
    Node atual = novo;
    while(novo->next != NULL) {
        atual = novo;
        novo  = novo->next;
        free(atual);
    }
    free(novo);
    free(L);
    L = NULL;
}
void ImprimeLista(Lista L) {
    Node atual = L->inicio;
    printf("{ ");
    while(atual != NULL) {
        printf("%s; ", atual->valor);
        atual = atual->next;
    }
    printf("}\n");
}

int test_funcs() {
    char* n[] = {"teste", "lalala", "alo", "onde", "hello"};
    char* m[] = {"como", "lalala", "aqui", "teste", "hello"};

    Lista L = CriaLista();
    Lista LL = CriaLista();
    printf("criou lista\n");
    for (int i=0; i<5; i+=1) {
        InsereInicio(L, n[i]);
        // InsereFim(LL, m[i]);
    }
    printf("inseriu 1\n");
    for (int i=0; i<5; i+=1) {
        // InsereInicio(L, n[i]);
        InsereFim(LL, m[i]);
    }
    printf("inseriu 2\n");
    ImprimeLista(L);
    ImprimeLista(LL);
    printf("L: %p\n", (void*) L);
    // for (int i = 0; i < 5; i++) {
    //     int r = rank(L, m[i]);
    //     printf("[%d]: %d; ", i, r);
    // }
    printf("\n");

    DestroiLista(L);
    DestroiLista(LL);
    return(0);
}
