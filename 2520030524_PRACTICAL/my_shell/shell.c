#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024

int main() {
    char input[MAX_INPUT];
    char *args[64];
    pid_t pid;
    int status;

    while (1) {
        printf("my_shell> ");   // Prompt
        fgets(input, MAX_INPUT, stdin);

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        // Exit condition
        if (strcmp(input, "exit") == 0) break;

        // Tokenize input
        int i = 0;
        args[i] = strtok(input, " ");
        while (args[i] != NULL) {
            i++;
            args[i] = strtok(NULL, " ");
        }

        // Fork a process
        pid = fork();
        if (pid == 0) {
            // Child process
            if (execvp(args[0], args) == -1) {
                perror("execvp failed");
            }
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // Parent waits
            waitpid(pid, &status, 0);
        } else {
            perror("fork failed");
        }
    }

    return 0;
}
