
#include "util.h"

#include <signal.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <readline/readline.h>
#include <readline/history.h>

#define MAXLEN 100
#define MAXARG 10
#define EMBLEN 4

char* prompt_gen() {
    char* username = getenv("USER");
    char* cwd = getcwd(NULL, 0);

    int u_len = strlen(username);
    int d_len = strlen(cwd);

    int prompt_len = u_len + d_len + 4 + 1;

    char* prompt = (char*) malloc(prompt_len); checkPtr(prompt);

    strcpy(prompt, "{");
    strncpy(prompt + 1, username, u_len);
    strcpy(prompt + 1 + u_len, "@");
    strncpy(prompt + 2 + u_len, cwd, d_len);
    strcpy(prompt + 2 + u_len + d_len, "} ");

    free(cwd);
    cwd = NULL;

    return prompt;
}

/** Implementações com syscalls das funcoes embutidadas */
char* embutidas[] = { "mkdir", "kill", "ln", "quit" };

int fazdir(char** argv) {
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
int homicidio(char** argv) {
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
int liga(char** argv) {
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
int execcommand(char** argv) {
    int fid = func_id(argv[0]);
    switch (fid) {
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
            kill(0, SIGKILL);
            break;
        default:
            if(execve(argv[0], argv, NULL)) {
                printf("erro ao rodar %s\n", argv[0]);
                exit(1);
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

        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}
int parse_input(char** commands, char* buffer) {
    char* saveptr;
    char* temp;
    int i = 0;
    temp = strtok_r(buffer, " ", &saveptr);
    while (temp != NULL) {
        if(i == MAXARG) {
            printf("muitos argumentos, encerrando execução\n");
            exit(1);
        }

        strncpy(commands[i], temp, MAXLEN);
        i++;
        temp = strtok_r(NULL, " ", &saveptr);
    }
    commands[i] = NULL;
    return 0;
}

int main(int argc, char const **argv) {

    char entrada[MAXLEN*MAXARG + 1];
    char* args[MAXARG + 1];
    for(int i = 0; i < MAXARG; i++) {
        args[i] = (char*) malloc(MAXLEN + 1); checkPtr(args[i]);
    }
    args[MAXARG] = NULL;


    char* prompt = prompt_gen();

    using_history();
    while (1) {
        if (read_input(entrada, prompt))
            continue;
        parse_input(args, entrada);

        pid_t pid = fork();
        if (pid != 0) {
            wait(NULL);
        }
        else {
            execcommand(args);
            exit(0);
        }
    }
    free(prompt);
    for(int i = 0; i < MAXARG; i++) {
        free(args[i]);
        args[i] = NULL;
    }
    return 0;
}
