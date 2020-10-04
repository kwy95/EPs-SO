#include "util.h"

#include <pthread.h>
#include <signal.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int _debug = 0;
int _sair = 0;
clock_t _t0;
pthread_mutex_t* _mutexes;

struct timespec _disponivel, _resto;
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
    // struct timespec tempo = { _disponivel, _ndisponivel };
    // struct timespec resto;
    while(1) {
        pthread_mutex_lock(&_mutexes[proc->id]);
        if(_debug) {
            fprintf(stderr, "O processo %s acabou de comecou a rodar na CPU %d\n",
                                proc->nome, 1);
        }

        printf("rodando proc %s", proc->nome);
        if(nanosleep(&_disponivel, &_resto) == -1) {
            pthread_mutex_unlock(&_mutexes[proc->id]);
            nanosleep(&_quantum, NULL);
            if(_debug) {
                fprintf(stderr, "O processo %s acabou de comecou a rodar na CPU %d\n",
                                    proc->nome, 1);
            }
            continue;
        }

        pthread_mutex_unlock(&_mutexes[proc->id]);
        break;
    };

    if(_debug) {
        fprintf(stderr, "O processo %s acabou de encerrar sua execução, a linha de saída é:\n    %s %d %d",
                            proc->nome, proc->nome, proc->to);
    }

    return NULL;
}

void FirstComeFirstServed(void* proc){
    Fila processos = (Fila) proc;
    int N = processos->size;

    pthread_t threads[N];//= (pthread_t*) malloc(N * sizeof(pthread_t)); checkPtr(threads);

    _mutexes = (pthread_mutex_t*) malloc(N * sizeof(pthread_mutex_t)); checkPtr(_mutexes);
    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&_mutexes[i], NULL);
        pthread_mutex_lock(&_mutexes[i]);
    }

    _t0 = clock();

    while (peek(processos) != NULL) {
        Fila escalonador = CriaFila();
        time_t t = time(NULL) - _t0;

        while(peek(processos)->to <= t) {
            Trace tr = dequeue(processos);
            pthread_create(&threads[tr->id], NULL, proc_sim, tr);
            enqueue(escalonador, tr);
            if(_debug) {
                fprintf(stderr, "O processo %s acabou de chegar no sistema com a linha:\n    %s %d %d %d\n",
                                tr->nome, tr->nome, tr->to, tr->dt, tr->deadline);
            }
        }

        while(peek(escalonador)->to <= t) {

        }
        // if (time > traceroute.to)
        //     wait_time += time - traceroute.to;
        // while(time < traceroute.to){
        //     sleep(1);
        //     time++;
        // }
        // while(traceroute.dt > 0){
        //     traceroute.dt--;
        //     sleep(1);
        //     time++;
        // }
        // total_lines++;
        // printf("%d\n", time);
    }


    // printf("%d\n", wait_time);
    // printf("%f\n", (float) wait_time/total_lines);

}

void ShortestRemainingTimeNext(void* file_name){
    Fila processos = (Fila) proc;
    int N = processos->size;

    int time =0;

    int wait_time = 0;
    int i = 0;
    int segment = 0;

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




    printf("%d\n", wait_time);
    // printf("%f\n", (float) wait_time/total_lines);

}


int main(int argc, char const **argv) {
    int mode = atoi(argv[1]);
    const char* file_name = argv[2];
    const char* out_file  = argv[3];
    if(argc == 5 && !strcmp(argv[4], "d"))
        _debug = 1;

    Fila processos = init_fila(file_name);
    if (mode == 1)
        FirstComeFirstServed((void*) processos));
    if (mode == 2)
        ShortestRemainingTime((void*) processos));
}
