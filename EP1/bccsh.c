#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include <readline/readline.h>
#include <readline/history.h>

void type_prompt();
void read_command(const char* prompt, char** cmd) {
    char* buf;

    buf = readline(prompt);
    if (strlen(buf) != 0) {
        add_history(buf);
        // strcpy(cmd, buf);
        // cmd = &buf;
    }
    // free(buf);
}

int main(int argc, char const **argv) {
    char** command;
    // char* parameters;
    char* username = getenv("USER");
    char* cwd = getcwd(NULL, 0);
    int n = 0;

    printf("user: %ld | %s\ncwd: %ld | %s\n", strlen(username), username, strlen(cwd), cwd);

    while (1) {
        //type_prompt();
        read_command("certinho>> ", command);
        // printf("\n%s\n", *command);
        // if (fork() != 0) {
            /* Codigo do pai */
        // } else {
            /* Codigo do filho */
            // execve(command,parameters,0);
        // }
        // n++;
    }
    // free(command);

    return 0;
}
