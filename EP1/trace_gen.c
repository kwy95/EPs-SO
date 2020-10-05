#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PROB 3 /* A probabilidade de dois processos chegarem juntos é 1/(PROB * (1-PROB)) */
#define PRIORIDADE 20 /* Tempo extra máximo que será somado ao deadline mínimo */

int main (int argc, char ** argv) {
	time_t t;
 	srand((unsigned) time(&t));

	int numberOfProcesses;
	int traceSize = atoi(argv[1]);
	int TEMPO = atoi(argv[2]); //*Tempo maximo*/
	FILE * trace;
	if (traceSize == 1) {
		trace = fopen("small.txt", "w");
		numberOfProcesses = 10;
	}
	else if (traceSize == 2) {
		trace = fopen("medium.txt", "w");
		numberOfProcesses = 100;
	}
	else {
		trace = fopen("big.txt", "w");
		numberOfProcesses = 600;
	}

	int t0, dt, deadline;
	for (int i = 0; i < numberOfProcesses; i++) {
		t0 = rand() % PROB == 0 ? i+1 : i;
		dt = rand() % TEMPO + 1;
		deadline = t0 + dt + (rand() % PRIORIDADE + 1);
		fprintf(trace, "p%d %d %d %d\n", i, t0, dt, deadline);
	}

	fclose(trace);
	return 0;
}
