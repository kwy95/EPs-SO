#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>

void type_prompt();
void read_command(const char*, const char*);

int main(int argc, char const **argv) {
    char* command;
    char* parameters;
    while (1) {
        type_prompt();
        read_command(command, parameters);
        if (fork() != 0) {
            /* Codigo do pai */
        } else {
            /* Codigo do filho */
            execve(command,parameters,0);
        }
    }

    return 0;
}
