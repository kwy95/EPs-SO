#include "util.h"

#include <pthread.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int _debug = 0;

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
    while(0) {};

    return NULL;
}

void FirstComeFirstServed(const char * file_name){
    Fila processos = init_fila(file_name);

    pthread_t threads[processos->size];

    int time = 0;
    // int total_lines = 0;
    int wait_time = 0;
    while (0) {

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


    printf("%d\n", wait_time);
    // printf("%f\n", (float) wait_time/total_lines);

}

void ShortestRemainingTime(const char * file_name){
    Fila processos = init_fila(file_name);

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

    if (mode == 1)
        FirstComeFirstServed(file_name);
    if (mode == 2)
        ShortestRemainingTime(file_name);
}
