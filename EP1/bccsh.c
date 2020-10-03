
#include "util.h"

char* prompt_gen() {
    char* username = getenv("USER");
    char* cwd = getcwd(NULL, 0);

    int u_len = strlen(username);
    int d_len = strlen(cwd);

    int prompt_len = u_len + d_len + 4 + 1;

    char* prompt = (char*) malloc(prompt_len); checkPtr(prompt);

    strncpy(prompt, "{", 1);
    strncpy(prompt + 1, username, u_len);
    strncpy(prompt + 1 + u_len, "@", 1);
    strncpy(prompt + 2 + u_len, cwd, d_len);
    strcpy(prompt + 2 + u_len + d_len, "} ");

    return prompt;
}

/** Implementações com syscalls das funcoes embutidadas */
char* embutidas[] = { "mkdir", "kill", "ln", "quit" };
int fazdir(const char** argv) {
    if(strcmp(argv[0], "mkdir")) {
        printf("entrou no fazdir, mas era %s\n", argv[0]);
        exit(1);
    }

    if(mkdir(argv[1], 0775)) {
        printf("erro ao criar diretório\n");
        exit(1);
    }
    return 0;
}
int homicidio(const char** argv) {
    if(strcmp(argv[0], "kill")) {
        printf("entrou no homicidio, mas era %s\n", argv[0]);
        exit(1);
    }
    int signal = atoi(argv[1]) * (-1);
    pid_t pid = atoi(argv[2]);

    if(kill(pid, signal)) {
        printf("erro ao enviar sinal\n");
        exit(1);
    }
    return 0;
}
int liga(const char** argv) {
    if(strcmp(argv[0], "ln")) {
        printf("entrou no liga, mas era %s\n", argv[0]);
        exit(1);
    }

    if(symlink(argv[2], argv[3])) {
        printf("erro ao criar link\n");
        exit(1);
    }
    return 0;
}

int func_id(const char* command) {
    for (int i = 0; i < EMBLEN; i++) {
        if(!strcmp(command, embutidas[i])) {
            return i;
        }
    }
    return -1;
}
int execcommand(const char** argv) {
    int fid = func_id(argv[0]);
    switch (func_id(argv[0])) {
        case 0:
            fazdir(argv);
            break;
        case 1:
            homicidio(argv);
            break;
        case 2:
            liga(argv);
            break;
        case 3:
            kill(0, 9);
            break;
        default:
            if(execve(argv[0], argv, NULL)) {
                printf("erro ao rodar %s\n", argv[0]);
                exit(1);
                // return errno;
            }
            break;
    }
    return 0;
}

int read_input(char* str,const char* prompt) {
    char* buf;

    buf = readline(prompt);
    if (strlen(buf) != 0) {
        add_history(buf);

        // parse_input(commands, buf);
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}
int parse_input(char** commands, char* buffer) {
    char* temp;
    int i = 0;
    temp = strtok(buffer, " ");
    while (temp != NULL) {
        // printf ("%s\n",temp);
        strcpy(commands[i], temp);
        i++;
        temp = strtok(NULL, " ");
    }
    return 0;
}

int main(int argc, char const **argv) {
    // int n = 1;
    // usleep(1000000);
    char entrada[MAXLEN*MAXARG + 1];
    char* args[MAXARG + 1];
    for(int i = 0; i < MAXARG; i++) {
        args[i] = (char*) malloc(MAXLEN); checkPtr(args[i]);
    }


    char* prompt = prompt_gen();

    // printf("user: %ld | %s\ncwd: %ld | %s\n", strlen(username), username,
    //                                           strlen(cwd),      cwd);
    // printf("prompt: %s\n", prompt);

    // test_funcs();

    using_history();
    while (1) {
        if (read_input(entrada, prompt))
            continue;
        parse_input(args, entrada);


        pid_t pid = fork();
        if (pid != 0) {
            /* Codigo do pai */
            // printf("Pai: %d\n", pid);
            wait(NULL);
            // printf("terminou\n");
        }
        else {
            /* Codigo do filho */
            execcommand(args);
            // printf("Filho\n");
            exit(0);
        }
        // n*=10;
    }
    free(prompt);
    for(int i = 0; i < MAXARG; i++) {
        free(args[i]);
    }
    return 0;
}
