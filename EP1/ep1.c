#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "util.h"


int main(int argc, char const **argv) {

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char * pch;

    fp = fopen(argv[1], "r");
    printf("%s\n",argv[1] );
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
    	pch = strtok(line, " ");
    	while (pch != NULL)
		{
		    printf ("%s\n",pch);
		    pch = strtok (NULL, " ,.-");
		}
    }

    fclose(fp);
    if (line)
        free(line);

}