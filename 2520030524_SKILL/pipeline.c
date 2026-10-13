#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define HISTORY_CAPACITY 10

// Command history
char *history[HISTORY_CAPACITY];
int history_size = 0;

// Add command to history
void add_history(const char *cmd) {
    if (history_size < HISTORY_CAPACITY) {
        history[history_size++] = strdup(cmd);
    } else {
        free(history[0]);
        for (int i = 1; i < HISTORY_CAPACITY; i++) {
            history[i-1] = history[i];
        }
        history[HISTORY_CAPACITY-1] = strdup(cmd);
    }
}

// Display history
void show_history() {
    printf("Command History:\n");
    for (int i = 0; i < history_size; i++) {
        printf("%d: %s\n", i+1, history[i]);
    }
}

// Free history
void free_history() {
    for (int i = 0; i < history_size; i++) {
        free(history[i]);
    }
}

// Execute pipeline
void execute_pipeline(char *input) {
    char *commands[10];
    int cmd_count = 0;

    char *tok = strtok(input, "|");
    while (tok && cmd_count < 10) {
        commands[cmd_count++] = strdup(tok);
        tok = strtok(NULL, "|");
    }

    int pipes[2*cmd_count];
    for (int i = 0; i < cmd_count-1; i++) {
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
            if (i < cmd_count-1) {
                dup2(pipes[i*2+1], 1);
            }
            // Close all pipes
            for (int j = 0; j < 2*(cmd_count-1); j++) {
                close(pipes[j]);
            }
            // Tokenize command
            char *args[10];
            int argc = 0;
            char *arg = strtok(commands[i], " \t");
            while (arg && argc < 9) {
                args[argc++] = arg;
                arg = strtok(NULL, " \t");
            }
            args[argc] = NULL;
            execvp(args[0], args);
            perror("exec failed");
            exit(1);
        }
    }

    // Close all pipes in parent
    for (int j = 0; j < 2*(cmd_count-1); j++) {
        close(pipes[j]);
    }

    // Wait for children
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

        add_history(input);

        if (strcmp(input, "history") == 0) {
            show_history();
            continue;
        }

        execute_pipeline(input);
    }

    free_history();
    return 0;
}

