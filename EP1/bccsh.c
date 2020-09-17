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
void read_command(char* cmd, char* args) {
    char* buf;

    buf = readline("\n>>> ");
    if (strlen(buf) != 0) {
        add_history(buf);
        // strcpy(str, buf);
    }
    // cmd = (char*) malloc();
}

int main(int argc, char const **argv) {
    char* command;
    char* parameters;
    while (1) {
        //type_prompt();
        read_command(command, parameters);
        printf("%s", command);
        if (fork() != 0) {
            /* Codigo do pai */
        } else {
            /* Codigo do filho */
            // execve(command,parameters,0);
        }
    }

    return 0;
}
