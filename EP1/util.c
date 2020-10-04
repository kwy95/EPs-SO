
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

    printf("    criando: %p | %s\n", (void*) novo, novo->nome);

    return novo;
}
void atualizarTrace(Trace t,int dt) {
    t->elapsed += dt;
}
void destroiTrace(Trace t) {
    if (t != NULL) {
        printf("    destruindo: %p | %s\n", (void*) t, t->nome);
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
    // char* n[] = {"teste", "lalala", "alo", "onde", "hello"};
    // char* m[] = {"como", "lalala", "aqui", "teste", "hello"};

    // Lista L = CriaLista();
    // Lista LL = CriaLista();
    // printf("criou lista\n");
    // for (int i=0; i<5; i+=1) {
        // InsereInicio(L, n[i]);
        // InsereFim(LL, m[i]);
    // }
    // printf("inseriu 1\n");
    // for (int i=0; i<5; i+=1) {
        // InsereInicio(L, n[i]);
        // InsereFim(LL, m[i]);
    // }
    // printf("inseriu 2\n");
    // ImprimeLista(L);
    // ImprimeLista(LL);
    // printf("L: %p\n", (void*) L);
    // for (int i = 0; i < 5; i++) {
    //     int r = rank(L, m[i]);
    //     printf("[%d]: %d; ", i, r);
    // }
    // printf("\n");

    // DestroiLista(L);
    // DestroiLista(LL);

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

    return(0);
}
