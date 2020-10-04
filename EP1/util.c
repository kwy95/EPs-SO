
#include "util.h"

void checkPtr(void* ptr) {
    if (ptr == NULL) {
        printf("Pointer error!\n");
        exit(1);
    }
}

Trace novoTrace(char* t) {
    Trace novo = (Trace) malloc(sizeof(struct trace)); checkPtr(novo);

    char* saveptr;
    char* buf = strtok_r(t, " ", &saveptr);
    novo->nome = (char*) malloc(strlen(buf) + 1); checkPtr(novo->nome);
    strcpy(novo->nome, buf);

    novo->to       = atoi(strtok_r(NULL, " ", &saveptr));
    novo->dt       = atoi(strtok_r(NULL, " ", &saveptr));
    novo->deadline = atoi(strtok_r(NULL, " ", &saveptr));
    novo->elapsed  = 0;
    novo->id = -1;

    // printf("    criando: %p | %s\n", (void*) novo, novo->nome);

    return novo;
}
void atualizarTrace(Trace t, long dt) {
    t->elapsed += dt;
}
void destroiTrace(Trace t) {
    if (t != NULL) {
        // printf("    destruindo: %p | %s\n", (void*) t, t->nome);
        free(t->nome);
        t->nome = NULL;
        free(t);
        t = NULL;
    }
}
void destroiTraceA(Trace* T, int size) {
    if(T != NULL) {
        for (int i = 0; i < size; i++) {
            destroiTrace(T[i]);
            T[i] = NULL;
        }
        free(T);
        T = NULL;
    }
}

Fila CriaFila() {
    Fila F = (Fila) malloc(sizeof(struct fila)); checkPtr(F);

    F->traces    = (Trace*) malloc(sizeof(Trace)); checkPtr(F->traces);
    F->traces[0] = NULL;
    F->size      = 0;
    F->space     = 1;
    F->first     = 0;
    F->last      = 0;
    F->n_id      = 0;

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
    printf("\n  aumentou  \n");


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
    printf("\n  diminuiu  \n");


    Trace* old = F->traces;
    // int old_space = F->space;
    F->traces = new_array;
    F->space = new_space;
    F->first = 0;
    F->last = F->size;

    free(old);
}

void enqueue(Fila F, Trace T) {
    if(F->size >= F->space)
        aumentaF(F);
    // if(F->traces[F->last] != NULL)
    //     destroiTrace(F->traces[F->last]);
    if(T->id == -1) {
        T->id = F->n_id;
        F->n_id++;
    }
    F->traces[F->last] = T;
    F->size++;
    F->last = (F->last + 1) % F->space;
}
Trace dequeue(Fila F) {
    if(F->size <= 0) {
        // printf("Fila vazia\n");
        return NULL;
    }

    Trace t = F->traces[F->first];
    F->size--;
    F->first = (F->first + 1) % F->space;

    if(F->size < F->space / 4)
        diminuiF(F);

    return t;
}
Trace peek(Fila F) {
    if(F->size <= 0)
        return NULL;
    return F->traces[F->first];
}


void DestroiFila(Fila F) {
    if(F != NULL) {
        destroiTraceA(F->traces, F->space);
        free(F);
        F = NULL;
    }
}
void ImprimeFila(Fila F) {
    // printf("space: %d; size: %d\n", F->space, F->size);
    if(F->size == 0) {
        printf("A fila está vazia.\n");
    } else {
        printf("Fila de tamanho %d com %d elementos\n", F->space, F->size);
        printf("  first: %d; last: %d | n_id: %d\n", F->first, F->last, F->n_id);
        for (int i = 0; i < F->size; i++) {
            Trace elemento = F->traces[(F->first + i) % F->space];
            printf("  elemento %d: [ %s, %d, %d, %d, %d, %d ]\n", i,
                        elemento->nome,     elemento->to,      elemento->dt,
                        elemento->deadline, elemento->elapsed, elemento->id);
            // printf("    loc: %p\n", (void*) elemento);
        }
    }
}


int test_funcs() {

    Fila F = CriaFila();

    printf("criou Fila\n");

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen("trace1.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    printf("\nEntrando dados\n");
    while ((read = getline(&line, &len, fp)) != -1) {
        ImprimeFila(F);
        // printf(" linha: %s\n", line);
        Trace t = novoTrace(line);
        enqueue(F, t);
    }
    free(line);
    line = NULL;
    fclose(fp);
    // printf("inseriu\n");

    ImprimeFila(F);
    // printf("F: %p\n", (void*) F);

    printf("\nRemovendo dados\n");

    Trace elemento = dequeue(F);
    while(elemento != NULL) {
        ImprimeFila(F);
        printf("\nelemento retirado: %s, %d, %d, %d\n\n", elemento->nome,
                                                      elemento->to,
                                                      elemento->dt,
                                                      elemento->deadline);
        destroiTrace(elemento);
        elemento = dequeue(F);
        printf("\n\n");
    }

    DestroiFila(F);

    return(0);
}
