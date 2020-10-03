
#include "util.h"

int main(int argc, char const **argv) {

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    size_t read;
    char * pch;
    int num_lines = 0;
    int line_i = 0;

    fp = fopen(argv[1], "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, fp)) != -1) {
        num_lines++;
    }

    fclose(fp);
    fp = fopen(argv[1], "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    Trace traceroute[num_lines];

    while ((read = getline(&line, &len, fp)) != -1) {
        traceroute[line_i].nome = strtok(line, " ");
        traceroute[line_i].to = atoi(strtok (NULL, " "));
        traceroute[line_i].dt = atoi(strtok (NULL, " "));
        traceroute[line_i].deadline = atoi(strtok (NULL, " "));
        line_i++;
    }

    fclose(fp);
    if (line)
        free(line);

}
