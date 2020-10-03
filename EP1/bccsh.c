
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

char* embutidas[] = { "mkdir", "kill", "ln" };

int fazdir(char** argv) {
    if(strcmp(argv[0], "mkdir")) {
        printf("entrou no fazdir, mas era %s\n", argv[0]);
        exit(1);
    }
    return 0;
}
int homicidio(char** argv);
int liga(char** argv);

int parse_input(char** commands, char* buffer) {


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

int main(int argc, char const **argv) {
    // int n = 1;
    // usleep(1000000);
    char command[1000];
    char* prompt = prompt_gen();



    // printf("user: %ld | %s\ncwd: %ld | %s\n", strlen(username), username,
    //                                           strlen(cwd),      cwd);
    // printf("prompt: %s\n", prompt);

    // test_funcs();

    using_history();
    while (1) {
        //type_prompt();
        if (read_input(command, prompt))
            continue;
        // printf("\n%s\n", command);

        pid_t pid = fork();
        if (pid != 0) {
            /* Codigo do pai */
            printf("Pai: %d\n", pid);
            wait(NULL);
            printf("terminou\n");
        }
        else {
            /* Codigo do filho */
            // execve(command,parameters,0);
            printf("Filho\n");
            exit(0);
        }
        // n*=10;
    }
    free(prompt);
    return 0;
}
