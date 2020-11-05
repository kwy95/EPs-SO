#include "util.h"

int _debug = 0;
struct timespec _quantum = { 0, QUANTUM };

int _d = 0;
int _n = 0;
int _voltas_max = 0;

_Atomic int _correndo = 0;
_Atomic int _terminaram = 0;
_Atomic int _quebraram = 0;
_Atomic int _dt = 3 * TSTEP;
_Atomic int _sprint = 1;

pthread_mutex_t* _mutexes;
pthread_mutex_t _mutex_rank;
pthread_mutex_t _mutex_print;
pthread_barrier_t _barriers[2];

Ciclista* _corredores;
pthread_t* _t_corredores;

int* _pista[LANES];
int* _t_status;
int* _passaram_volta;
int** _rank;
int* _podium;

// FILE* _output;

void set_status(int id, int status) {
    pthread_mutex_lock(&_mutex_rank);
    _t_status[id] = status;
    pthread_mutex_unlock(&_mutex_rank);
}

void get_status(int st) {
    pthread_mutex_lock(&_mutex_rank);
    pthread_mutex_lock(&_mutex_print);

    printf("Status [%2d]:\n", st);
    for (int i = 0; i < _n; i++) {
        printf("    [%3x]: %2d\n", i + 1, _t_status[i]);
    }

    pthread_mutex_unlock(&_mutex_print);
    pthread_mutex_unlock(&_mutex_rank);
}


void print_corredores(int id);

void mover_ciclista(Ciclista c, int new_p, int new_d) {
    int prev_p = c->posp;
    int prev_d = c->posd;

    _pista[prev_p][prev_d] = 0;
    _pista[new_p][new_d] = c->id;
    c->posp = new_p;
    c->posd = new_d;
}

void* corredor (void* args) {
    Ciclista info = (Ciclista) args;
    int terminou = 0;

    int e = 0;
    if ((e = pthread_barrier_wait(&_barriers[0])) > 0 || e < -1) {
        pthread_mutex_lock(&_mutex_print);
        printf("Problema {%d} na barreira de inicialização! [%3x] (%ld)\n", e, info->id, pthread_self());
        pthread_mutex_unlock(&_mutex_print);
        exit(1);
    }
    // pthread_mutex_lock(&_mutex_print);
    // printf("thread [%3d] (%ld) passou barreira 0\n", info->id, pthread_self());
    // pthread_mutex_unlock(&_mutex_print);

    while(!terminou) {
        set_status(info->id - 1, 1);

        int avancou = 0;
        int t_vel = info->t_vel;
        int curp = info->posp;
        int curd = info->posd;
        int nextd = (curd + 1) % _d;
        int fracd = t_vel * _dt / 600;

        info->tf += _dt;

        int go = 0;
        while(!go) {
            if(!pthread_mutex_trylock(&_mutexes[curd])) {
                if(!pthread_mutex_trylock(&_mutexes[nextd])) {
                    go = 1;
                } else {
                    pthread_mutex_unlock(&_mutexes[curd]);
                    nanosleep(&_quantum, NULL);
                }
            } else {
                nanosleep(&_quantum, NULL);
            }
        }

        set_status(info->id - 1, 2);

        if (!_pista[curp][nextd]) { // frente vazia
            if (info->fracp + fracd >= 6) { // avança um espaço
                mover_ciclista(info, curp, nextd);
                info->dist++;
                info->fracp += fracd - 6;
                avancou = 1;
            }
        } else { // alguém na frente
            Ciclista frente = _corredores[_pista[curp][nextd] - 1];
            if (frente->vel < t_vel) { // acerta a velocidade
                t_vel = frente->vel;
                fracd = t_vel * _dt / 600;
            }

            if (info->fracp + fracd >= 6) { // tenta avançar
                for (int p = curp + 1; p < 10 && !avancou; p++) { // checa se pode ultrapassar
                    if (!_pista[p][curd] && !_pista[p][nextd]) {
                        mover_ciclista(info, p, nextd);
                        info->dist++;
                        info->fracp += fracd - 6;
                        avancou = 1;
                    }
                }
            }
        }
        if (!avancou) { // tenta ir para uma pista mais interna
            info->fracp += fracd;
            int entrou = 0;
            for (int p = 0; p < curp && !entrou; p++) {
                if (!_pista[p][curd]) {
                    mover_ciclista(info, p, curd);
                    entrou = 1;
                }
            }
        }
        info->vel = t_vel;
        pthread_mutex_unlock(&_mutexes[curd]);
        pthread_mutex_unlock(&_mutexes[nextd]);

        set_status(info->id - 1, 3);

        if (avancou && info->dist / _d > info->volta) { // completou uma volta
            int vt = info->volta;
            info->volta++;

            pthread_mutex_lock(&_mutex_print);
            printf("\n  -> O ciclista [%3x] iniciou a volta %d.\n", info->id, vt + 2);
            pthread_mutex_unlock(&_mutex_print);

            pthread_mutex_lock(&_mutex_rank);
            int rk = _passaram_volta[vt] + 1;
            _passaram_volta[vt]++;
            _rank[info->id - 1][vt] = rk;
            pthread_mutex_unlock(&_mutex_rank);

            if (vt % 2 != 0 && rk >= (_n+1) - ((vt+1)/2)) { // eliminação
                terminou = 1;
                _correndo--;
                _podium[rk - 1] = info->id;
                _terminaram++;
                pthread_mutex_lock(&_mutex_print);
                printf("\n  -> O ciclista [%3x] foi eliminado na volta %d.\n", info->id, vt + 1);
                pthread_mutex_unlock(&_mutex_print);
            }
            if (!terminou && vt % 6 == 5 && _correndo > 5 && bernoulli(0.05)) { // quebras ou desistencia
                terminou = 1;
                _correndo--;
                _quebraram++;
                pthread_mutex_lock(&_mutex_print);
                printf("\n  -> O ciclista [%3x] quebrou na volta %d.\n", info->id, vt + 1);
                pthread_mutex_unlock(&_mutex_print);
            }
            if (!terminou && vt == _voltas_max - 1) { // ganhou
                terminou = 1;
                _correndo--;
                _podium[rk - 1] = info->id;
                _terminaram++;
                pthread_mutex_lock(&_mutex_print);
                printf("\n  -> O ciclista [%3x] ganhou a corrida.\n", info->id);
                pthread_mutex_unlock(&_mutex_print);
            }

            if (!terminou) { // nova velocidade
                if (info->t_vel == 30) {
                    info->t_vel = bernoulli(0.2) ? (1 * VSTEP) : (2 * VSTEP);
                } else if (info->t_vel == 60) {
                    info->t_vel = bernoulli(0.4) ? (1 * VSTEP) : (2 * VSTEP);
                }
                if (info->volta == _voltas_max - 2 && !_sprint) {
                    _sprint = bernoulli(0.9) ? 3 : (bernoulli(0.5) ? 1 : 2);
                }
                if (vt == _voltas_max - 2 &&_rank[info->id - 1][vt] == _sprint) {
                    info->t_vel = 3 * VSTEP;
                    // _dt = 1 * TSTEP;
                    _sprint = 4;
                }
            }
            print_corredores(info->id -1);
        }

        set_status(info->id - 1, 4);

        pthread_mutex_lock(&_mutexes[info->posd]);
        if (terminou) {
            _pista[info->posp][info->posd] = 0;
        }
        pthread_mutex_unlock(&_mutexes[info->posd]);

        // pthread_mutex_unlock(&_mutexes[curd]);
        // pthread_mutex_unlock(&_mutexes[nextd]);

        // e = 0;
        if ((e = pthread_barrier_wait(&_barriers[0])) > 0 || e < -1) {
            pthread_mutex_lock(&_mutex_print);
            printf("Problema {%d} na barreira 1 da thread [%3x] (%ld)\n", e, info->id, pthread_self());
            pthread_mutex_unlock(&_mutex_print);
            exit(1);
        }

        set_status(info->id - 1, 5);

        // e = 0;
        if ((e = pthread_barrier_wait(&_barriers[1])) > 0 || e < -1) {
            pthread_mutex_lock(&_mutex_print);
            printf("Problema {%d} na barreira 2 da thread [%3x] (%ld)\n", e, info->id, pthread_self());
            pthread_mutex_unlock(&_mutex_print);
            exit(1);
        }
        set_status(info->id - 1, 6);
    }
    pthread_mutex_lock(&_mutex_print);
    printf("thread [%3x] (%ld) terminou\n", info->id, pthread_self());
    pthread_mutex_unlock(&_mutex_print);

    set_status(info->id - 1, -1);


    return NULL;
}

void init_pista() {
    _corredores = (Ciclista*) malloc(_n * sizeof(Ciclista)); checkPtr(_corredores);
    _t_corredores = (pthread_t*) malloc(_n * sizeof(pthread_t)); checkPtr(_t_corredores);
    _rank = (int**) malloc(_n * sizeof(int*)); checkPtr(_rank);
    _podium = (int*) malloc(_n * sizeof(int)); checkPtr(_podium);
    _t_status = (int*) malloc(_n * sizeof(int)); checkPtr(_t_status);
    _passaram_volta = (int*) malloc(_voltas_max * sizeof(int)); checkPtr(_passaram_volta);

    for(int i = 0; i < _n; i++) {
        _rank[i] = (int*) malloc(_voltas_max * sizeof(int)); checkPtr(_rank[i]);
        for(int j = 0; j < _voltas_max; j++) {
            _rank[i][j] = 0;
        }
        _podium[i] = 0;
        _t_status[i] = 0;
    }
    for(int i = 0; i < LANES; i++) {
        _pista[i] = (int*) malloc(_d * sizeof(int)); checkPtr(_pista[i]);
        for(int j = 0; j < _d; j++) {
            _pista[i][j] = 0;
        }
    }
    for(int i = 0; i < _voltas_max; i++)
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
            // printf("corredor: %2d\n", id);
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

void destroi_pista() {
    destroiCiclistaA(_corredores, _n);
    free(_t_corredores);
    free(_passaram_volta);
    for (int i = 0; i < _n; i++) {
        free(_rank[i]);
    }
    free(_rank);
    free(_podium);
    free(_t_status);
    for (int i = 0; i < LANES; i++) {
        free(_pista[i]);
    }
}

void init_threadctl() {
    _mutexes = (pthread_mutex_t*) malloc(_d * sizeof(pthread_mutex_t)); checkPtr(_mutexes);

    pthread_mutex_init(&_mutex_rank, NULL);
    pthread_mutex_init(&_mutex_print, NULL);
    for (int i = 0; i < _d; i++) {
        pthread_mutex_init(&_mutexes[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_barrier_init(&_barriers[i], NULL, _n + 1);
    }
}

void end_threadctl() {
    pthread_mutex_destroy(&_mutex_rank);
    pthread_mutex_destroy(&_mutex_print);
    for (int i = 0; i < 2; i++) {
        while(pthread_barrier_destroy(&_barriers[i]) == EBUSY) {
            nanosleep(&_quantum, NULL);
        }
        // if (pthread_barrier_destroy(&_barriers[i])) {
        //     pthread_mutex_lock(&_mutex_print);
        //     printf("Problema ao destruir barreira 1");
        //     pthread_mutex_unlock(&_mutex_print);
        //     exit(1);
        // }
    }
    for (int i = 0; i < _d; i++) {
        pthread_mutex_destroy(&_mutexes[i]);
    }
    free(_mutexes);
    _mutexes = NULL;
}

void print_pista() {
    pthread_mutex_lock(&_mutex_print);
    printf("Pista [%4d]:\n", _correndo);
    for (int j = 0; j < _d; j++) {
        for (int i = 0; i < 10; i++) {
            printf(" %3x |", _pista[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
    pthread_mutex_unlock(&_mutex_print);
}

void print_rank() {
    pthread_mutex_lock(&_mutex_print);
    printf("Rank:\n");
    for (int i = 0; i < _n; i++) {
        printf(" %3x :", _corredores[i]->id);
        for (int j = 0; j < _voltas_max; j++) {
            printf(" %3d |", _rank[i][j]);
        }
        printf("\n");
    }
        printf("\n\n");
    pthread_mutex_unlock(&_mutex_print);
}

void print_corredores(int id) {
    pthread_mutex_lock(&_mutex_print);
    if (id == -1) {
        printf("*** Corredores:\n");
        for (int i = 0; i < _n; i++) {
            printf("  [%3x]: vel= %2d; pp= %2d; pd= %3d; vt= %d\n", _corredores[i]->id,
                _corredores[i]->t_vel, _corredores[i]->posp, _corredores[i]->posd,
                _corredores[i]->volta);
        }
        // printf("\n");
        printf("***\n");
    } else {
        printf("***\n");
        printf("  [%3x]: t_vel= %2d; pp= %2d; pd= %3d; vt= %d\n", _corredores[id]->id,
            _corredores[id]->t_vel, _corredores[id]->posp, _corredores[id]->posd,
            _corredores[id]->volta);
        printf("           vel= %2d; dist= %5ld; tf= %5d; frac= %d\n",
            _corredores[id]->vel, _corredores[id]->dist, _corredores[id]->tf,
            _corredores[id]->fracp);
        printf("***\n");
    }
    pthread_mutex_unlock(&_mutex_print);
}

void* controlador(void* args) {
    int iterations = 0;
    int corr = _correndo;
    print_pista();
    // print_rank();
    // printf("pre-barreira 0 controller %ld\n", pthread_self());
    int e = 0;
    if ((e = pthread_barrier_wait(&_barriers[0])) > 0 || e < -1) {
        pthread_mutex_lock(&_mutex_print);
        printf("Problema {%d} na barreira de inicialização! [%3x] (%ld)\n", e, 0, pthread_self());
        pthread_mutex_unlock(&_mutex_print);
        exit(1);
    }
    // print_corredores();

    while(_correndo > 0) {
        iterations++;

        get_status(1);

        // int e = 0;
        if ((e = pthread_barrier_wait(&_barriers[0])) > 0 || e < -1) {
            pthread_mutex_lock(&_mutex_print);
            printf("Problema {%d} na barreira 1 da thread [%3x] (%ld)\n", e, 0, pthread_self());
            pthread_mutex_unlock(&_mutex_print);
            exit(1);
        }
        if (_sprint == 4)
            _dt = 1 * TSTEP;

        if (_debug)
            print_pista();
        // print_corredores();

        if (corr != _correndo) {
            get_status(3);
            while(pthread_barrier_destroy(&_barriers[0]) == EBUSY) {
                nanosleep(&_quantum, NULL);
            }

            // if (pthread_barrier_destroy(&_barriers[0])) {
            //     pthread_mutex_lock(&_mutex_print);
            //     printf("Problema ao destruir barreira 2");
            //     pthread_mutex_unlock(&_mutex_print);
            //     exit(1);
            // }
            pthread_barrier_init(&_barriers[0], NULL, _correndo + 1);
            print_rank();
        }

        get_status(2);

        // int e = 0;
        if ((e = pthread_barrier_wait(&_barriers[1])) > 0 || e < -1) {
            pthread_mutex_lock(&_mutex_print);
            printf("Problema {%d} na barreira 2 da thread [%3x] (%ld)\n", e, 0, pthread_self());
            pthread_mutex_unlock(&_mutex_print);
            exit(1);
        }

        if (corr != _correndo) {
            get_status(0);
            while(pthread_barrier_destroy(&_barriers[1]) == EBUSY) {
                nanosleep(&_quantum, NULL);
            }

            // if (pthread_barrier_destroy(&_barriers[1])) {
            //     pthread_mutex_lock(&_mutex_print);
            //     printf("Problema ao destruir barreira 1");
            //     pthread_mutex_unlock(&_mutex_print);
            //     exit(1);
            // }
            pthread_barrier_init(&_barriers[1], NULL, _correndo + 1);
            corr = _correndo;
        }
    }
    print_rank();
    for (int i = 0; i < _n; i++)
    {
        printf(" %d ;", _podium[i]);
    }
    printf("\n");

    return NULL;
}

int main(int argc, char const **argv) {
    _d = atoi(argv[1]);
    _n = atoi(argv[2]);
    if(argc == 4 && !strcmp(argv[3], "d"))
        _debug = 1;

    // _output = fopen("out/output.txt", "w");
    _voltas_max = 2 * (_n - 1);
    _correndo = _n;

    srand(time(NULL));

    // printf("\n ** retorno bom [%ld]", EINVAL)

    init_threadctl();
    init_pista();

    pthread_t controller;
    if(pthread_create(&controller, NULL, controlador, NULL)) {
        printf("\n ERROR creating thread controller\n");
        exit(1);
    }

    // fclose(_output);
    pthread_join(controller, NULL);
    for (int i = 0; i < _n; i++)
        pthread_join(_t_corredores[i], NULL);
    destroi_pista();
    end_threadctl();
    return 0;
}
