#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Split input into arguments, handling escape sequences
char** tokenize(const char *input, int *argc) {
    int capacity = 8;
    char **args = malloc(capacity * sizeof(char*));
    *argc = 0;

    const char *p = input;
    char buf[256];
    int pos = 0;

    while (*p) {
        if (*p == ' ') {
            if (pos > 0) {
                buf[pos] = '\0';
                args[(*argc)++] = strdup(buf);
                pos = 0;
                if (*argc >= capacity) {
                    capacity *= 2;
                    args = realloc(args, capacity * sizeof(char*));
                }
            }
            p++;
        } else if (*p == '\\') { // escape sequence
            p++;
            if (*p) {
                buf[pos++] = *p; // preserve escaped char
                p++;
            }
        } else {
            buf[pos++] = *p++;
        }
    }

    if (pos > 0) {
        buf[pos] = '\0';
        args[(*argc)++] = strdup(buf);
    }

    args[*argc] = NULL; // execvp requires NULL-terminated array
    return args;
}

// Free arguments
void free_args(char **args, int argc) {
    for (int i = 0; i < argc; i++) {
        free(args[i]);
    }
    free(args);
}

int main() {
    char input[256];
    printf("Enter command: ");
    if (!fgets(input, sizeof(input), stdin)) return 1;
    input[strcspn(input, "\n")] = '\0'; // remove newline

    int argc;
    char **args = tokenize(input, &argc);

    if (argc == 0) {
        fprintf(stderr, "Empty command\n");
        free_args(args, argc);
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execvp(args[0], args);
        perror("Execution failed");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        } else {
            printf("Child terminated abnormally\n");
        }
    } else {
        perror("Fork failed");
    }

    free_args(args, argc);
    return 0;
}
