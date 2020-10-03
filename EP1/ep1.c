#include "util.h"


typedef struct trace
{
    char * nome;
    int to;
    int dt;
    int deadline;
} Trace;


void FirstComeFirst(char * file_name){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    size_t read;
    char * pch;
    fp = fopen(file_name, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    int time =0;
    int total_lines = 0;
    int wait_time = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("%s\n",line );
        Trace traceroute;
        traceroute.nome = strtok(line, " ");
        traceroute.to = atoi(strtok (NULL, " "));
        traceroute.dt = atoi(strtok (NULL, " "));
        traceroute.deadline = atoi(strtok (NULL, " "));
        if (time > traceroute.to)
            wait_time += time - traceroute.to;
        while(time < traceroute.to){
            sleep(1);
            time++;
        }
        while(traceroute.dt > 0){
            traceroute.dt--;
            sleep(1);
            time++;
        }
        total_lines++;
        printf("%d\n", time);
    }

    fclose(fp);
    if (line)
        free(line);
    printf("%d\n", wait_time);
    printf("%f\n", (float) wait_time/total_lines);

}


int main(int argc, char const **argv) {
    char * file_name = argv[2];

    if (atoi(argv[1]) == 1)
        FirstComeFirst(file_name);        



}
