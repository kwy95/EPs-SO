
#include "util.h"

void checkPtr(void* ptr) {
    if (ptr == NULL) {
        printf("Pointer error!\n");
        exit(1);
    }
}

void swap( Trace a, Trace b ) {
    Trace t;
    t = a;
    a = b;
    b = t;
}

int indice(Fila F, int i) {
    return (F->first + i) % F->space;
}

/** Calcula o tempo passado em segundos */
int time_dif(struct timespec t, struct timespec t0) {
    long s = t.tv_sec - t0.tv_sec;
    long ns = t.tv_nsec - t0.tv_nsec;
    long e = ((s*TSCALE) + ns) / TSCALE;
    return e;
}


Trace novoTrace(char* t) {
    Trace novo = (Trace) malloc(sizeof(struct trace)); checkPtr(novo);

    char* saveptr;
    char* buf = strtok_r(t, " ", &saveptr);
    novo->nome = (char*) malloc(strlen(buf) + 1); checkPtr(novo->nome);
    strcpy(novo->nome, buf);

    novo->t0       = atoi(strtok_r(NULL, " ", &saveptr));
    novo->dt       = atoi(strtok_r(NULL, " ", &saveptr));
    novo->deadline = atoi(strtok_r(NULL, " ", &saveptr));
    novo->remaining  = novo->dt;
    novo->nremaining = 0;
    novo->id       = -1;

    // printf("    criando: %p | %s\n", (void*) novo, novo->nome);

    return novo;
}
void atualizarTrace(Trace t, struct timespec dt) {
    t->remaining -= dt.tv_sec;
    t->nremaining -= dt.tv_nsec;
    if(t->nremaining < 0) {
        long s = (t->remaining * TSCALE) + t->nremaining;
        t->remaining = s / TSCALE;
        t->nremaining = s % TSCALE;
    }
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
        }
        free(T);
        T = NULL;
    }
}
int trace_done(Trace t) {
    return (t->remaining <= 0 && t->nremaining <= 0);
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
    // printf("\n  aumentou  \n");


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
    // printf("\n  diminuiu  \n");


    Trace* old = F->traces;
    // int old_space = F->space;
    F->traces = new_array;
    F->space = new_space;
    F->first = 0;
    F->last = F->size;

    free(old);
}

int empty(Fila F) {
    return F->size <= 0;
}

double remaining(Fila F, int id) {
    int i = indice(F, id);
    double t = F->traces[i]->remaining;
    t = t + ((double) F->traces[i]->nremaining / (double) TSCALE);
    return t;
}

int parent(Fila F, int id) {
    if(id > 1 && id < F->size)
        return id / 2;
    return -1;
}
int filho_esq(Fila F, int id) {
    if(2*id < F->size && id >= 1)
        return 2*id;
    return -1;
}
int filho_dir(Fila F, int id) {
    if(((2 * id) + 1) < F->size && id >= 1)
        return ((2 * id) + 1);
    return -1;
}

void ascender(Fila F) {
    int id = F->size - 1;
    while(id > 0 && F->traces[indice(F, id)] < F->traces[indice(F, parent(F,id))]) {
        swap(F->traces[indice(F, id)], F->traces[indice(F, parent(F,id))]);
        id = parent(F, id);
    }
}
void descender(Fila F) {
    int id = 0;
    int id_l = 1 ? F->size > 1 : -1;
    int id_r = 2 ? F->size > 2 : -1;

    int menor = id;
    while((id_l != -1 || id_r != -1)) {
        if(id_l != -1 && F->traces[indice(F, menor)] > F->traces[indice(F, id_l)])
            menor = id_l;
        if(id_r != -1 && F->traces[indice(F, menor)] > F->traces[indice(F, id_r)])
            menor = id_r;

        if(menor == id)
            return;

        swap(F->traces[indice(F, id)], F->traces[indice(F, menor)]);

        id = menor;
        id_l = filho_esq(F, id);
        id_r = filho_dir(F, id);
    }
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
    if(empty(F)) {
        // printf("Fila vazia\n");
        return NULL;
    }

    Trace t = F->traces[F->first];
    F->size--;
    F->traces[F->first] = NULL;
    F->first = (F->first + 1) % F->space;

    if(F->size < F->space / 4)
        diminuiF(F);

    return t;
}
Trace peek(Fila F) {
    if(empty(F))
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
    if(empty(F)) {
        printf("A fila está vazia.\n");
    } else {
        printf("Fila de tamanho %d com %d elementos\n", F->space, F->size);
        printf("  first: %d; last: %d | n_id: %d\n", F->first, F->last, F->n_id);
        for (int i = 0; i < F->size; i++) {
            Trace elemento = F->traces[(F->first + i) % F->space];
            printf("  elemento %d: [ %s, %d, %ld, %d, %ld, %d ]\n", i,
                        elemento->nome,     elemento->t0,      elemento->dt,
                        elemento->deadline, elemento->remaining, elemento->id);
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
        printf("\nelemento retirado: %s, %d, %ld, %d\n\n", elemento->nome,
                                                      elemento->t0,
                                                      elemento->dt,
                                                      elemento->deadline);
        destroiTrace(elemento);
        elemento = dequeue(F);
        printf("\n\n");
    }

    DestroiFila(F);

    return(0);
}
