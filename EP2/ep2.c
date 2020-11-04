#include "util.h"


int _debug = 0;
int _d = 0;
int _n = 0;
int _dt = 6 * TSTEP; //! MUDAR VALOR ANTES DA ENTREGA
pthread_mutex_t* _mutexes;
pthread_barrier_t _barriers[3];
Ciclista* _corredores;
pthread_t* _t_corredores;
_Atomic int _correndo = 0;
// FILE* _output;
int* _pista[10];
_Atomic int* _passaram_volta;

void* corredor (void* args) {
    Ciclista info = (Ciclista) args;
    int elim = 0;

    pthread_barrier_wait(&_barriers[0]);

    while(!elim) {
        int avancou = 0;
        int t_vel = info->t_vel;
        int curd = info->posd;
        int curp = info->posp;
        int nextd = (curd + 1) % _d;
        int fracd = t_vel * _dt / 600;

        pthread_mutex_lock(&_mutexes[curd]);
        if (info->fracp + fracd >= 6) { // tentativa de avançar um espaço
            pthread_mutex_lock(&_mutexes[nextd]);
            if (!_pista[curp][nextd]) { // avanço simples
                _pista[curp][curd] = 0;
                _pista[curp][nextd] = info->id;
                info->dist++;
                info->posd = nextd;
                info->fracp += fracd - 6;
                info->vel = t_vel;
                avancou = 1;
            } else {
                for (int p = curp + 1; p < 10 && !avancou; p++) { // checa se pode ultrapassar
                    if (!_pista[p][curd] && !_pista[p][nextd]) {
                        _pista[curp][curd] = 0;
                        _pista[p][nextd] = info->id;
                        info->dist++;
                        info->posp = p;
                        info->posd = nextd;
                        info->fracp += fracd - 6;
                        info->vel = t_vel;
                        avancou = 1;
                    }
                }
                if (!avancou) { // não consegue ultrapassar
                    t_vel = _corredores[_pista[curp][nextd] - 1]->vel;
                    fracd = t_vel * _dt / 600;
                    info->fracp += fracd;
                    info->vel = t_vel;

                    int entrou = 0;
                    for (int p = 0; p < info->posp && !entrou; p++) {
                        if (!_pista[p][info->posd]) {
                            _pista[info->posp][info->posd] = info->id;
                            _pista[p][info->posd] = info->id;
                        }
                    }
                }
            }
            pthread_mutex_unlock(&_mutexes[nextd]);
        } else {
            int entrou = 0;
            for (int p = 0; p < info->posp && !entrou; p++) {
                if (!_pista[p][info->posd]) {
                    _pista[info->posp][info->posd] = info->id;
                    _pista[p][info->posd] = info->id;
                }
            }
        }
        pthread_mutex_unlock(&_mutexes[curd]);


        if (_pista[curp][nextd]) {
            if (_corredores[_pista[curp][nextd] - 1]->vel < t_vel)
                t_vel = _corredores[_pista[curp][nextd] - 1]->vel;
        }

        // if ((info->dist / _d) > info->volta)
        // printf(" [%3x]: vel= %2d; pd= %3d; pp= %2d;\n", info->id,
            //    info->vel, info->posd, info->posp);

        // print_pista(_d, NULL);
        pthread_barrier_wait(&_barriers[1]);
        pthread_barrier_wait(&_barriers[2]);
    }

    return NULL;
}


void init_pista() {
    _corredores = (Ciclista*) malloc(_n * sizeof(Ciclista)); checkPtr(_corredores);
    _t_corredores = (pthread_t*) malloc(_n * sizeof(pthread_t)); checkPtr(_t_corredores);
    for(int i = 0; i < 10; i++) {
        _pista[i] = (int*) malloc(_d * sizeof(int)); checkPtr(_pista[i]);
        for(int j = 0; j < _d; j++) {
            _pista[i][j] = 0;
        }
    }
    _passaram_volta = (_Atomic int*) malloc(2 * (_n-1) * sizeof(_Atomic int)); checkPtr(_passaram_volta);
    for(int i = 0; i < 2*(_n-1); i++)
        _passaram_volta[i] = 0;

    // posiciona os corredores
    int faltam = _n;
    int disp[_n];
    for (int i = 0; i < _n; i++)
        disp[i] = i+1;
    for (int pd = (int) ceil(((double) _n) / 5.0) - 1; pd >= 0; pd--) {
        for (int pp = 0; pp < 5 && faltam > 0; pp++) {
            int next = rand_lim(faltam-1);
            int id = disp[next];
            printf("corredor: %2d\n", id);
            _pista[pp][pd] = id;
            _corredores[id - 1] = novoCiclista(id, pd, pp);
            if(pthread_create(&_t_corredores[id - 1], NULL, corredor, _corredores[id - 1])) {
                printf("\n ERROR creating thread %d\n", id);
                exit(1);
            }
            disp[next] = disp[--faltam];
        }
    }
}

void init_threadctl() {
    _mutexes = (pthread_mutex_t*) malloc(_d * sizeof(pthread_mutex_t)); checkPtr(_mutexes);

    for (int i = 0; i < _d; i++) {
        pthread_mutex_init(&_mutexes[i], NULL);
    }
    for (int i = 0; i < 3; i++) {
        pthread_barrier_init(&_barriers[i], NULL, _n + 1);
    }
}

void end_threadctl() {
    for (int i = 0; i < _d; i++) {
        pthread_mutex_destroy(&_mutexes[i]);
    }
    for (int i = 0; i < 3; i++) {
        pthread_barrier_destroy(&_barriers[i]);
    }

}

void free_pista() {
    for(int i = 0; i < 10; i++) {
        free(_pista[i]);
        _pista[i] = NULL;
    }
}

void print_pista(int d, FILE* fp) {
    if(fp) {
        for (int j = 0; j < d; j++) {
            for (int i = 0; i < 10; i++) {
                fprintf(fp, " %03x |", _pista[i][j]);
            }
            fprintf(fp, "\n");
        }
            fprintf(fp, "\n\n");
            return;
    }

    for (int j = 0; j < d; j++) {
        for (int i = 0; i < 10; i++) {
            printf(" %3x |", _pista[i][j]);
        }
        printf("\n");
    }
        printf("\n\n");
}

void print_corredores() {
    for (int i = 0; i < _n; i++) {
        printf(" [%3x]: vel= %2d; pd= %3d; pp= %2d;\n", _corredores[i]->id,
               _corredores[i]->vel, _corredores[i]->posd, _corredores[i]->posp);
    }
        printf("\n");
}

void* controlador(void* args) {
    printf("pre-barreira 0 %ld controller\n", pthread_self());
    pthread_barrier_wait(&_barriers[0]);
    print_pista(_d, NULL);
    // print_corredores();

    // pthread_t test;
    // if(pthread_create(&test, NULL, corredor, _corredores[_pista[2][((int) ceil(((double) _n) / 5.0)) - 1] - 1])) {
    //     printf("\n ERROR creating thread \n");
    //     exit(1);
    // }


    while(_correndo > 0) {
        printf("pre barreira 1\n");
        pthread_barrier_wait(&_barriers[1]);
        print_pista(_d, NULL);
        sleep(5);
        pthread_barrier_wait(&_barriers[2]);
    }
    return NULL;
}

int main(int argc, char const **argv) {
    _d = atoi(argv[1]);
    _n = atoi(argv[2]);
    if(argc == 4 && !strcmp(argv[3], "d"))
        _debug = 1;

    /* if(argc == 5) {
        _output = fopen(argv[4], "w");
    } */

    _correndo = _n;

    srand(time(NULL));

    init_threadctl();
    init_pista();
    pthread_t controller;
    if(pthread_create(&controller, NULL, controlador, NULL)) {
        printf("\n ERROR creating thread controller\n");
        exit(1);
    }

    pthread_join(controller, NULL);
    free_pista();
    return 0;
}
