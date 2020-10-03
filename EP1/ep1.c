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

    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("%s\n",line );
        Trace traceroute;
        traceroute.nome = strtok(line, " ");
        traceroute.to = atoi(strtok (NULL, " "));
        traceroute.dt = atoi(strtok (NULL, " "));
        traceroute.deadline = atoi(strtok (NULL, " "));
    }

    fclose(fp);
    if (line)
        free(line);

}


int main(int argc, char const **argv) {
    char * file_name = argv[2];

    if (atoi(argv[1]) == 1)
        FirstComeFirst(file_name);        



}
