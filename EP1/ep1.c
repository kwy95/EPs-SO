#include "util.h"

#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int _debug = 0;
// int _sair = 0;
int _contexto = 0;
pthread_mutex_t* _mutexes;
pthread_cond_t* _conds;
int* _first_exec;
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

void printTime(struct timespec t) {
    printf("\n*        Tempo: %ld secs e %ld nsecs\n", t.tv_sec, t.tv_nsec);
    printf("*        time_dif: %d secs\n\n", time_dif(t, _t0));
}
void set_first(int id, int t) {
    if(_first_exec[id] != -1)
        return;
    _first_exec[id] = t;
}

void *proc_sim(void* p) {
    Trace proc = (Trace) p;

    pthread_mutex_lock(&_mutexes[proc->id]);
    while(!trace_done(proc)) {
        pthread_cond_wait(&_conds[proc->id], &_mutexes[proc->id]);
        nanosleep(&_quantum, NULL);
        atualizarTrace(proc, _quantum);
    }
    pthread_mutex_unlock(&_mutexes[proc->id]);

    return NULL;
}

pthread_t* escalonador_init(Fila P) {
    int N = P->size;

    pthread_t* threads= (pthread_t*) malloc(N * sizeof(pthread_t)); checkPtr(threads);

    _first_exec = (int*) malloc(N * sizeof(int)); checkPtr(_first_exec);
    _mutexes = (pthread_mutex_t*) malloc(N * sizeof(pthread_mutex_t)); checkPtr(_mutexes);
    _conds = (pthread_cond_t*) malloc(N * sizeof(pthread_cond_t)); checkPtr(_conds);
    for (int i = 0; i < N; i++) {
        _first_exec[i] = -1;
        pthread_mutex_init(&_mutexes[i], NULL);
        pthread_cond_init(&_conds[i], NULL);
    }

    clock_gettime(CLOCK_REALTIME, &_t0);

    return threads;
}

void* FirstComeFirstServed(void* proc) {
    // fprintf(stderr, "    Iniciou-se FCFS\n");
    Fila processos = (Fila) proc;
    int N = processos->size;
    int curId = -1;
    Trace cur;

    pthread_t* threads = escalonador_init(processos);

    Fila escalonador = CriaFila();

    while (!empty(processos) || !empty(escalonador) || curId != -1) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        int t = time_dif(ts, _t0);
        // printTime(ts);

        /** Chegada de processos no sistema */
        while(!empty(processos) && t >= peek(processos)->t0) {
            Trace tr = dequeue(processos);
            pthread_create(&threads[tr->id], NULL, proc_sim, tr);
            enqueue(escalonador, tr);
            if(_debug) {
                fprintf(stderr, "[%d] O processo %s acabou de chegar no sistema com a linha:\n    %s %d %ld %d\n",
                                t, tr->nome, tr->nome, tr->t0, tr->dt, tr->deadline);
            }
        }

        /** Nenhum processo rodando, 1 ou mais na espera */
        if (curId == -1 && !empty(escalonador)) {
            cur = dequeue(escalonador);
            curId = cur->id;
            set_first(curId, t);

            pthread_mutex_lock(&_mutexes[curId]);
            pthread_cond_signal(&_conds[curId]);
            pthread_mutex_unlock(&_mutexes[curId]);
            if(_debug) {
                fprintf(stderr, "O processo %s comecou a rodar na CPU %d\n",
                                    cur->nome, 1);
            }

        /** Processo retornou controle pro escalonador */
        } else if (curId != -1 && !pthread_mutex_trylock(&_mutexes[curId])) {

            /** Processo terminou de ser executado */
            if(trace_done(cur)) {
                if(_debug) {
                    fprintf(stderr, "O processo %s acabou de encerrar sua execução, a linha de saída é:\n  %s %d %d\n",
                                        cur->nome, cur->nome, t, t - _first_exec[curId]);
                }
                fprintf(output, "%s %d %d %d\n", cur->nome, t, t - _first_exec[curId], cur->deadline);
                pthread_mutex_unlock(&_mutexes[curId]);
                destroiTrace(cur);

                /** Executa pŕoximo processo */
                if (!empty(escalonador)) {
                    _contexto++;
                    cur = dequeue(escalonador);
                    curId = cur->id;
                    set_first(curId, t);

                    pthread_mutex_lock(&_mutexes[curId]);
                    pthread_cond_signal(&_conds[curId]);
                    pthread_mutex_unlock(&_mutexes[curId]);
                    if(_debug) {
                        fprintf(stderr, "Mudança de contexto! Total até o momento: %d\n", _contexto);
                        fprintf(stderr, "O processo %s comecou a rodar na CPU %d\n",
                                            cur->nome, 1);
                    }
                } else {
                    curId = -1;
                }

            /** Continua execução do processo */
            } else {
                // pthread_mutex_lock(&_mutexes[curId]);
                pthread_cond_signal(&_conds[curId]);
                pthread_mutex_unlock(&_mutexes[curId]);
            }
        }
    }

    for (int i = 0; i < N; i++) {
        if (pthread_join(threads[i], NULL))  {
            printf("\n ERROR joining threads %d\n",i);
            exit(1);
        }
    }

    fprintf(output, "%d\n", _contexto);

    free(threads);
    DestroiFila(escalonador);

    return NULL;
}

void* ShortestRemainingTimeNext(void* proc) {
    // fprintf(stderr, "    Iniciou-se RR\n");
    Fila processos = (Fila) proc;
    int N = processos->size;
    int curId = -1;
    Trace cur;

    pthread_t* threads = escalonador_init(processos);

    Fila escalonador = CriaFila();

    while (!empty(processos) || !empty(escalonador) || curId != -1) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        int t = time_dif(ts, _t0);

        /** Chegada de processos no sistema */
        while(!empty(processos) && t >= peek(processos)->t0) {
            Trace tr = dequeue(processos);
            pthread_create(&threads[tr->id], NULL, proc_sim, tr);
            insert(escalonador, tr);
            if(_debug) {
                fprintf(stderr, "[%d] O processo %s acabou de chegar no sistema com a linha:\n    %s %d %ld %d\n",
                                t, tr->nome, tr->nome, tr->t0, tr->dt, tr->deadline);
            }
        }

        /** Nenhum processo rodando, 1 ou mais na espera */
        if (curId == -1 && !empty(escalonador)) {
            cur = get_min(escalonador);
            curId = cur->id;
            set_first(curId, t);

            pthread_mutex_lock(&_mutexes[curId]);
            pthread_cond_signal(&_conds[curId]);
            pthread_mutex_unlock(&_mutexes[curId]);
            if(_debug) {
                fprintf(stderr, "O processo %s comecou a rodar na CPU %d\n",
                                    cur->nome, 1);
            }

        /** Processo retornou controle pro escalonador */
        } else if (curId != -1 && !pthread_mutex_trylock(&_mutexes[curId])) {

            /** Processo terminou de ser executado */
            if(trace_done(cur)) {
                if(_debug) {
                    fprintf(stderr, "O processo %s acabou de encerrar sua execução, a linha de saída é:\n  %s %d %d\n",
                                        cur->nome, cur->nome, t, t - _first_exec[curId]);
                }
                fprintf(output, "%s %d %d %d\n", cur->nome, t, t - _first_exec[curId], cur->deadline);
                pthread_mutex_unlock(&_mutexes[curId]);
                destroiTrace(cur);

                /** Executa pŕoximo processo */
                if (!empty(escalonador)) {
                    _contexto++;
                    cur = get_min(escalonador);
                    curId = cur->id;
                    set_first(curId, t);

                    pthread_mutex_lock(&_mutexes[curId]);
                    pthread_cond_signal(&_conds[curId]);
                    pthread_mutex_unlock(&_mutexes[curId]);
                    if(_debug) {
                        fprintf(stderr, "Mudança de contexto! Total até o momento: %d\n", _contexto);
                        fprintf(stderr, "O processo %s comecou a rodar na CPU %d\n", cur->nome, 1);
                    }
                /** nada a fazer por enquanto */
                } else {
                    curId = -1;
                }

            /** Processo ainda não terminou */
            } else {
                /** Retorna processo pra fila e executa o próximo */
                if(!empty(escalonador) && remaining(peek(escalonador)) < remaining(cur)) {
                    _contexto++;
                    pthread_mutex_unlock(&_mutexes[curId]);
                    insert(escalonador, cur);
                    cur = get_min(escalonador);
                    curId = cur->id;
                    set_first(curId, t);

                    pthread_mutex_lock(&_mutexes[curId]);
                    pthread_cond_signal(&_conds[curId]);
                    pthread_mutex_unlock(&_mutexes[curId]);
                    if(_debug) {
                        fprintf(stderr, "Mudança de contexto! Total até o momento: %d\n", _contexto);
                        fprintf(stderr, "O processo %s comecou a rodar na CPU %d\n", cur->nome, 1);
                    }

                /** Continua a execução do processo */
                } else {
                    pthread_cond_signal(&_conds[curId]);
                    pthread_mutex_unlock(&_mutexes[curId]);
                }
            }
        }
    }

    for (int i = 0; i < N; i++) {
        if (pthread_join(threads[i], NULL))  {
            printf("\n ERROR joining threads %d\n",i);
            exit(1);
        }
    }

    fprintf(output, "%d\n", _contexto);

    free(threads);
    DestroiFila(escalonador);

    return NULL;
}

void* RoundRobin(void* proc) {
    // fprintf(stderr, "    Iniciou-se RR\n");
    Fila processos = (Fila) proc;
    int N = processos->size;
    int curId = -1;
    Trace cur;

    pthread_t* threads = escalonador_init(processos);

    Fila escalonador = CriaFila();

    while (!empty(processos) || !empty(escalonador) || curId != -1) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        int t = time_dif(ts, _t0);

        /** Chegada de processos no sistema */
        while(!empty(processos) && t >= peek(processos)->t0) {
            Trace tr = dequeue(processos);
            pthread_create(&threads[tr->id], NULL, proc_sim, tr);
            enqueue(escalonador, tr);
            if(_debug) {
                fprintf(stderr, "[%d] O processo %s acabou de chegar no sistema com a linha:\n    %s %d %ld %d\n",
                                t, tr->nome, tr->nome, tr->t0, tr->dt, tr->deadline);
            }
        }

        /** Nenhum processo rodando, 1 ou mais na espera */
        if (curId == -1 && !empty(escalonador)) {
            cur = dequeue(escalonador);
            curId = cur->id;
            set_first(curId, t);

            pthread_mutex_lock(&_mutexes[curId]);
            pthread_cond_signal(&_conds[curId]);
            pthread_mutex_unlock(&_mutexes[curId]);
            if(_debug) {
                fprintf(stderr, "O processo %s comecou a rodar na CPU %d\n",
                                    cur->nome, 1);
            }

        /** Processo retornou controle pro escalonador */
        } else if (curId != -1 && !pthread_mutex_trylock(&_mutexes[curId])) {

            /** Processo terminou de ser executado */
            if(trace_done(cur)) {
                if(_debug) {
                    fprintf(stderr, "O processo %s acabou de encerrar sua execução, a linha de saída é:\n  %s %d %d\n",
                                        cur->nome, cur->nome, t, t - _first_exec[curId]);
                }
                fprintf(output, "%s %d %d %d\n", cur->nome, t, t - _first_exec[curId], cur->deadline);
                pthread_mutex_unlock(&_mutexes[curId]);
                destroiTrace(cur);

                /** Executa pŕoximo processo */
                if (!empty(escalonador)) {
                    _contexto++;
                    cur = dequeue(escalonador);
                    curId = cur->id;
                    set_first(curId, t);

                    pthread_mutex_lock(&_mutexes[curId]);
                    pthread_cond_signal(&_conds[curId]);
                    pthread_mutex_unlock(&_mutexes[curId]);
                    if(_debug) {
                        fprintf(stderr, "Mudança de contexto! Total até o momento: %d\n", _contexto);
                        fprintf(stderr, "O processo %s comecou a rodar na CPU %d\n", cur->nome, 1);
                    }
                } else {
                    curId = -1;
                }

            /** Processo ainda não terminou */
            } else {

                /** Continua a execução do processo */
                if(empty(escalonador)) {
                    pthread_cond_signal(&_conds[curId]);
                    pthread_mutex_unlock(&_mutexes[curId]);

                /** Retorna processo pra fila e executa o próximo */
                } else {
                    _contexto++;
                    pthread_mutex_unlock(&_mutexes[curId]);
                    enqueue(escalonador, cur);
                    cur = dequeue(escalonador);
                    curId = cur->id;
                    set_first(curId, t);

                    pthread_mutex_lock(&_mutexes[curId]);
                    pthread_cond_signal(&_conds[curId]);
                    pthread_mutex_unlock(&_mutexes[curId]);
                    if(_debug) {
                        fprintf(stderr, "Mudança de contexto! Total até o momento: %d\n", _contexto);
                        fprintf(stderr, "O processo %s comecou a rodar na CPU %d\n", cur->nome, 1);
                    }
                }
            }
        }
    }

    for (int i = 0; i < N; i++) {
        if (pthread_join(threads[i], NULL))  {
            printf("\n ERROR joining threads %d\n",i);
            exit(1);
        }
    }

    fprintf(output, "%d\n", _contexto);

    free(threads);
    DestroiFila(escalonador);

    return NULL;
}

int main(int argc, char const **argv) {
    int mode = atoi(argv[1]);
    const char* file_name = argv[2];
    const char* out_file  = argv[3];
    if(argc == 5 && !strcmp(argv[4], "d"))
        _debug = 1;

    Fila processos = init_fila(file_name);
    // ImprimeFila(processos);
    output = fopen(out_file, "w"); checkPtr(output);

    pthread_t escalonador;
    if (mode == 1)
        pthread_create(&escalonador, NULL, FirstComeFirstServed,      (void*) processos);
    if (mode == 2)
        pthread_create(&escalonador, NULL, ShortestRemainingTimeNext, (void*) processos);
    if (mode == 3)
        pthread_create(&escalonador, NULL, RoundRobin,                (void*) processos);

    pthread_join(escalonador, NULL);

    DestroiFila(processos);
    fclose(output);
    free(_conds);
    free(_first_exec);
    free(_mutexes);

    return 0;
}
