
#include "util.h"
#include "fila.h"

Fila CriaFila() {
    Fila F = (Fila) malloc(sizeof(struct fila)); checkPtr(F);

    F->traces    = (Trace*) malloc(sizeof(Trace)); checkPtr(F->traces);
    F->traces[0] = NULL;
    F->size      = 0;
    F->space     = 1;
    F->first     = 0;
    F->last      = 0;

    return F;
}
void aumentaF(Fila F) {
    int new_space = F->space * 2;
    Trace* new_array = (Trace*) malloc(new_space * sizeof(Trace));
    checkPtr(new_array);

    for (int i = 0; i < F->size; i++) {
        new_array[i] = F->traces[(F->first + i) % F->space];
        // memcpy(new_array[i], F->traces[(F->first + i) % F->space], sizeof(Trace));
    }
    for (int i = F->size; i < new_space; i++) {
        new_array[i] = NULL;
    }

    Trace* old = F->traces;
    // int old_space = F->space;
    F->traces = new_array;
    F->space = new_space;
    F->first = 0;
    F->last = F->size;

    free(old);
}
void diminuiF(Fila F) {
    int new_space = F->space / 2;
    Trace* new_array = (Trace*) malloc(new_space * sizeof(Trace));
    checkPtr(new_array);

    for (int i = 0; i < F->size; i++) {
        new_array[i] = F->traces[(F->first + i) % F->space];
        // memcpy(new_array[i], F->traces[(F->first + i) % F->space], sizeof(Trace));
    }
    for (int i = F->size; i < new_space; i++) {
        new_array[i] = NULL;
    }

    Trace* old = F->traces;
    // int old_space = F->space;
    F->traces = new_array;
    F->space = new_space;
    F->first = 0;
    F->last = F->size;

    free(old);
}

void enqueue(Fila F, Trace T) {
    if(F->size > F->space / 2)
        aumentaF(F);
    if(F->traces[F->last] != NULL)
        destroiTrace(F->traces[F->last]);
    F->traces[F->last] = T;
    F->size++;
    F->last = (F->last + 1) % F->space;
}
Trace dequeue(Fila F) {
    if(F->size <= 0) {
        printf("Fila vazia\n");
        return NULL;
    }

    Trace t = F->traces[F->first];
    F->size--;
    F->first = (F->first + 1) % F->space;

    if(F->size < F->space / 4)
        diminuiF(F);

    return t;
}

void DestroiFila(Fila F) {
    if(F != NULL) {
        destroiTraceA(F->traces, F->space);
        free(F);
        F = NULL;
    }
}
void ImprimeFila(Fila F) {
    printf("space: %d; size: %d\n", F->space, F->size);
    if(F->size == 0) {
        printf("A fila está vazia.\n");
    } else {
        for (int i = 0; i < F->size; i++) {
            Trace elemento = F->traces[(F->first + i) % F->size];
            printf("elemento %d: %s, %d, %d, %d\n", i, elemento->nome,
                                                       elemento->to,
                                                       elemento->dt,
                                                       elemento->deadline);
            printf(" point: %p\n", (void*) elemento->nome);
        }
    }
}

int teste_fila() {
    Fila F = CriaFila();

    printf("criou Fila\n");

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen("trace1.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        printf(" linha: %s\n", line);
        Trace t = novoTrace(line);
        enqueue(F, t);
    }
    printf("inseriu\n");
    fclose(fp);

    ImprimeFila(F);
    printf("F: %p\n", (void*) F);

    Trace elemento = dequeue(F);
    while(elemento != NULL) {
        ImprimeFila(F);
        printf("elemento retirado: %s, %d, %d, %d\n\n", elemento->nome,
                                                      elemento->to,
                                                      elemento->dt,
                                                      elemento->deadline);
        elemento = dequeue(F);
    }

    DestroiFila(F);

    return 0;
}
