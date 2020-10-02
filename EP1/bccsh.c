#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "util.h"

void type_prompt();
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

int main(int argc, char const **argv) {
    using_history();
    char command[1000];
    // char* parameters;
    char* username = getenv("USER");
    char* cwd = getcwd(NULL, 0);
    int n = 0;
    const char* prompt = "certinho>> ";

    printf("user: %ld | %s\ncwd: %ld | %s\n", strlen(username), username, strlen(cwd), cwd);

    test_funcs();

    while (1) {
        //type_prompt();
        if (read_input(command, prompt))
            continue;
        printf("\n%s\n", command);
        if (fork() != 0) {
            /* Codigo do pai */
            printf("Pai\n");
        }
        else {
            /* Codigo do filho */
            // execve(command,parameters,0);
            printf("Filho\n");
            exit(0);
        }
        // n++;
    }
    // free(command);
    return 0;
}
