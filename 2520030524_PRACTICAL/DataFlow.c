#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Tokenize a command string into arguments
char** tokenize(char *cmd) {
    char **args = malloc(16 * sizeof(char*));
    int argc = 0;
    char *tok = strtok(cmd, " \t");
    while (tok && argc < 15) {
        args[argc++] = tok;
        tok = strtok(NULL, " \t");
    }
    args[argc] = NULL;
    return args;
}

// Execute pipeline of commands separated by '|'
void execute_pipeline(char *input) {
    char *commands[16];
    int cmd_count = 0;

    // Split input into commands
    char *tok = strtok(input, "|");
    while (tok && cmd_count < 16) {
        commands[cmd_count++] = strdup(tok);
        tok = strtok(NULL, "|");
    }

    int pipes[2 * (cmd_count - 1)];
    for (int i = 0; i < cmd_count - 1; i++) {
        if (pipe(pipes + i*2) < 0) {
            perror("pipe failed");
            exit(1);
        }
    }

    for (int i = 0; i < cmd_count; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Redirect input
            if (i > 0) {
                dup2(pipes[(i-1)*2], 0);
            }
            // Redirect output
            if (i < cmd_count - 1) {
                dup2(pipes[i*2+1], 1);
            }
            // Close all pipes
            for (int j = 0; j < 2*(cmd_count-1); j++) {
                close(pipes[j]);
            }
            // Tokenize and exec
            char **args = tokenize(commands[i]);
            execvp(args[0], args);
            perror("exec failed");
            exit(1);
        }
    }

    // Close all pipes in parent
    for (int j = 0; j < 2*(cmd_count-1); j++) {
        close(pipes[j]);
    }

    // Wait for all children
    for (int i = 0; i < cmd_count; i++) {
        int status;
        wait(&status);
    }

    for (int i = 0; i < cmd_count; i++) {
        free(commands[i]);
    }
}

int main() {
    char input[256];
    while (1) {
        printf("> ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;

        execute_pipeline(input);
    }
    return 0;
}
