
#include "util.h"

int _debug = 0;

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
        Trace traceroute;
        traceroute->nome = strtok(line, " ");
        traceroute->to = atoi(strtok (NULL, " "));
        traceroute->dt = atoi(strtok (NULL, " "));
        traceroute->deadline = atoi(strtok (NULL, " "));
    }

    fclose(fp);
    if (line)
        free(line);
}

int main(int argc, char const **argv) {
    int mode = atoi(argv[1]);
    char* file_name = argv[2];
    char* out_file  = argv[3];
    if(argc == 5 && !strcmp(argv[4], "d"))
        _debug = 1;

    if (atoi(argv[1]) == 1)
        FirstComeFirst(file_name);
}
