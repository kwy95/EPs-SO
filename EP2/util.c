
#include "util.h"

void checkPtr(void* ptr) {
    if (ptr == NULL) {
        printf("Pointer error!\n");
        exit(1);
    }
}

int rand_lim(int limit) {
    /* fonte: https://stackoverflow.com/questions/2999075/generate-a-random-number-within-range/2999130#2999130
        return a random number between 0 and limit inclusive.
    */

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do {
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

int bernoulli(double p) {
    int ret = 0;
    double pr = (double) rand() / (double) RAND_MAX;
    if (pr <= p) {
        ret = 1;
    }
    return ret;
}

/** Calcula o tempo passado em segundos */
int time_dif(struct timespec t, struct timespec t0) {
    long s = t.tv_sec - t0.tv_sec;
    long ns = t.tv_nsec - t0.tv_nsec;
    long e = ((s*TSCALE) + ns) / TSCALE;
    return e;
}

Ciclista novoCiclista(int i, int pd, int pp) {
    Ciclista novo = (Ciclista) malloc(sizeof(struct ciclista)); checkPtr(novo);

    novo->id = i;
    novo->t_vel = 1 * VSTEP;
    novo->vel = 1 * VSTEP;
    novo->posd = pd;
    novo->posp = pp;
    novo->dist = pd;
    novo->quebrou = 0;
    novo->volta = 0;
    novo->tf = 0;
    novo->fracp = 0;

    return novo;
}

void destroiCiclista(Ciclista c) {
    if (c != NULL) {
        free(c);
        c = NULL;
    }
}
void destroiCiclistaA(Ciclista* C, int size) {
    if(C != NULL) {
        for (int i = 0; i < size; i++) {
            destroiCiclista(C[i]);
        }
        free(C);
        C = NULL;
    }
}


// int test_funcs() {

//     Fila F = CriaFila();

//     printf("criou Fila\n");

//     FILE * fp;
//     char * line = NULL;
//     size_t len = 0;
//     ssize_t read;
//     fp = fopen("trace1.txt", "r");
//     if (fp == NULL)
//         exit(EXIT_FAILURE);

//     printf("\nEntrando dados\n");
//     while ((read = getline(&line, &len, fp)) != -1) {
//         ImprimeFila(F);
//         // printf(" linha: %s\n", line);
//         Trace t = novoTrace(line);
//         // enqueue(F, t);
//         insert(F, t);
//     }
//     free(line);
//     line = NULL;
//     fclose(fp);
//     // printf("inseriu\n");

//     ImprimeFila(F);
//     // printf("F: %p\n", (void*) F);

//     printf("\nRemovendo dados\n");

//     // Trace elemento = dequeue(F);
//     Trace elemento = get_min(F);
//     while(elemento != NULL) {
//         ImprimeFila(F);
//         printf("\nelemento retirado: %s, %d, %ld, %d\n\n", elemento->nome,
//                                                       elemento->t0,
//                                                       elemento->dt,
//                                                       elemento->deadline);
//         destroiTrace(elemento);
//         // elemento = dequeue(F);
//         elemento = get_min(F);
//         printf("\n\n");
//     }

//     DestroiFila(F);

//     return(0);
// }
