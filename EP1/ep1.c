#include "util.h"

#include <pthread.h>
#include <signal.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int _debug = 0;
int _sair = 0;
int _contexto = 0;
pthread_mutex_t* _mutexes;
pthread_cond_t* _conds;

FILE* output;

struct timespec _t0;
// struct timespec _disponivel, _resto;
struct timespec _quantum = { 0, QUANTUM };

Fila init_fila(const char* file_name) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    size_t read;
    fp = fopen(file_name, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    Fila F = CriaFila();

    while ((read = getline(&line, &len, fp)) != -1) {
        Trace t = novoTrace(line);
        enqueue(F, t);
    }
    free(line);
    line = NULL;
    fclose(fp);

    return F;
}

void *proc_sim(void* p) {
    Trace proc = (Trace) p;

    pthread_mutex_lock(&_mutexes[proc->id]);
    while(proc->elapsed < proc->dt) {
        pthread_cond_wait(&_conds[proc->id], &_mutexes[proc->id]);
        printf("rodando proc %s", proc->nome);
        nanosleep(&_quantum, NULL);
        atualizarTrace(proc, _quantum);
        // if(_debug) {
        //     fprintf(stderr, "O processo %s acabou de parar de rodar na CPU %d\n",
        //                         proc->nome, 1);
        //     }
    }

    pthread_mutex_unlock(&_mutexes[proc->id]);
    // if(_debug) {
    //     fprintf(stderr, "O processo %s acabou de encerrar sua execução, a linha de saída é:\n    %s %d %d",
    //                         proc->nome, proc->nome, proc->to, proc->deadline);
    // }

    return NULL;
}

pthread_t* escalonador_init(Fila P) {
    int N = P->size;

    pthread_t* threads= (pthread_t*) malloc(N * sizeof(pthread_t)); checkPtr(threads);

    _mutexes = (pthread_mutex_t*) malloc(N * sizeof(pthread_mutex_t)); checkPtr(_mutexes);
    _conds = (pthread_cond_t*) malloc(N * sizeof(pthread_cond_t)); checkPtr(_conds);
    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&_mutexes[i], NULL);
        pthread_cond_init(&_conds[i], NULL);
        pthread_mutex_lock(&_mutexes[i]);
    }

    clock_gettime(CLOCK_MONOTONIC, &_t0);

    return threads;
}

void* FirstComeFirstServed(void* proc) {
    Fila processos = (Fila) proc;
    int curId = -1;
    Trace cur;


    pthread_t* threads = escalonador_init(processos);

    Fila escalonador = CriaFila();

    while (1) {
        struct timespec t;
        clock_gettime(CLOCK_MONOTONIC, &t);

        while(peek(processos) != NULL && peek(processos)->to <= time_dif(t, _t0)) {
            Trace tr = dequeue(processos);
            pthread_create(&threads[tr->id], NULL, proc_sim, tr);
            enqueue(escalonador, tr);
            if(_debug) {
                fprintf(stderr, "O processo %s acabou de chegar no sistema com a linha:\n    %s %d %ld %d\n",
                                tr->nome, tr->nome, tr->to, tr->dt, tr->deadline);
            }
        }

        if(curId == -1 && peek(escalonador) != NULL) {
            cur = dequeue(escalonador);
            curId = cur->id;

            pthread_mutex_unlock(&_mutexes[curId]);
        }

        if(curId != -1 && !pthread_mutex_trylock(&_mutexes[curId])) {
            // if()
        }

        // if(_debug) {
        //     fprintf(stderr, "O processo %s acabou de comecou a rodar na CPU %d\n",
        //                         proc->nome, 1);
        // }

    }

    return NULL;
}

void* ShortestRemainingTimeNext(void* proc) {
    Fila processos = (Fila) proc;
    // int N = processos->size;

    escalonador_init(processos);

    while (0) {
        // tracelist[total_lines].nome = strtok(line, " ");
        // tracelist[total_lines].to = atoi(strtok (NULL, " "));
        // tracelist[total_lines].dt = atoi(strtok (NULL, " "));
        // tracelist[total_lines].deadline = atoi(strtok (NULL, " "));

        // total_lines++;
    }
    //for (int i = 0; i < total_lines; ++i)
    //{

    //}




    // printf("%d\n", wait_time);
    // printf("%f\n", (float) wait_time/total_lines);

    return NULL;
}

void* RoundRobin(void* proc) {
    Fila processos = (Fila) proc;
    // int N = processos->size;

    escalonador_init(processos);

    return NULL;
}

int main(int argc, char const **argv) {
    int mode = atoi(argv[1]);
    const char* file_name = argv[2];
    const char* out_file  = argv[3];
    if(argc == 5 && !strcmp(argv[4], "d"))
        _debug = 1;

    Fila processos = init_fila(file_name);
    output = fopen(out_file, "a");
    if (output == NULL)
        exit(EXIT_FAILURE);

    pthread_t escalonador;
    if (mode == 1)
        pthread_create(&escalonador, NULL, FirstComeFirstServed, (void*) processos);
    if (mode == 2)
        pthread_create(&escalonador, NULL, ShortestRemainingTimeNext, (void*) processos);
    if (mode == 3)
        pthread_create(&escalonador, NULL, RoundRobin, (void*) processos);

    fclose(output);

    return 0;
}
