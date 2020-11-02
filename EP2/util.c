
#include "util.h"

void checkPtr(void* ptr) {
    if (ptr == NULL) {
        printf("Pointer error!\n");
        exit(1);
    }
}

void swap(Fila F, int id1, int id2) {
    Trace t;
    t = F->traces[id1];
    F->traces[id1] = F->traces[id2];
    F->traces[id2] = t;
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
    }
    for (int i = F->size; i < new_space; i++) {
        new_array[i] = NULL;
    }


    Trace* old = F->traces;
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
    }
    for (int i = F->size; i < new_space; i++) {
        new_array[i] = NULL;
    }


    Trace* old = F->traces;
    F->traces = new_array;
    F->space = new_space;
    F->first = 0;
    F->last = F->size;

    free(old);
}

int empty(Fila F) {
    return F->size <= 0;
}

double remaining(Trace T) {
    double t = T->remaining;
    t = t + ((double) T->nremaining / (double) TSCALE);
    return t;
}

double rem(Fila F, int id) {
    int i = indice(F, id);
    Trace t = F->traces[i];
    return remaining(t);
}

/** funções de heap, evite misturar com as de fila */
int parent(Fila F, int id) {
    if(id > 0 && id < F->size)
        return (id - 1) / 2;
    return -1;
}
int filho_esq(Fila F, int id) {
    if((2 * id) + 1 < F->size && id >= 0)
        return (2 * id) + 1;
    return -1;
}
int filho_dir(Fila F, int id) {
    if((2 * id) + 2 < F->size && id >= 0)
        return (2 * id) + 2;
    return -1;
}

void ascender(Fila F, int id) {
    while(id > 0 && rem(F, id) < rem(F, parent(F,id))) {
        swap(F, indice(F, id), indice(F, parent(F,id)));
        id = parent(F, id);
    }
}
void descender(Fila F) {
    int id = 0;
    int id_l = filho_esq(F, id);
    int id_r = filho_dir(F, id);

    int menor = id;
    while(id_l != -1 || id_r != -1) {
        if(id_l != -1 && rem(F, menor) > rem(F, id_l))
            menor = id_l;
        if(id_r != -1 && rem(F, menor) > rem(F, id_r))
            menor = id_r;

        if(menor == id)
            return;

        swap(F, indice(F, id), indice(F, menor));

        id = menor;
        id_l = filho_esq(F, id);
        id_r = filho_dir(F, id);
    }
}

void insert(Fila F, Trace T) {
    enqueue(F, T);
    ascender(F, F->size - 1);
}
Trace get_min(Fila F) {
    if(empty(F)) {
        return NULL;
    }

    Trace min = F->traces[F->first];
    F->traces[F->first] = F->traces[indice(F, F->size - 1)];
    F->traces[indice(F, F->size - 1)] = NULL;
    F->last = (F->last + F->space - 1) % F->space; // equivalente a subtrair

    F->size--;

    descender(F);

    if(F->size < F->space / 4)
        diminuiF(F);

    return min;
}

void enqueue(Fila F, Trace T) {
    if(F->size >= F->space)
        aumentaF(F);

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
        // enqueue(F, t);
        insert(F, t);
    }
    free(line);
    line = NULL;
    fclose(fp);
    // printf("inseriu\n");

    ImprimeFila(F);
    // printf("F: %p\n", (void*) F);

    printf("\nRemovendo dados\n");

    // Trace elemento = dequeue(F);
    Trace elemento = get_min(F);
    while(elemento != NULL) {
        ImprimeFila(F);
        printf("\nelemento retirado: %s, %d, %ld, %d\n\n", elemento->nome,
                                                      elemento->t0,
                                                      elemento->dt,
                                                      elemento->deadline);
        destroiTrace(elemento);
        // elemento = dequeue(F);
        elemento = get_min(F);
        printf("\n\n");
    }

    DestroiFila(F);

    return(0);
}
