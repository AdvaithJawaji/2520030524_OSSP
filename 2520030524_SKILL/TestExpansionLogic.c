#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>


// Expand variables like $HOME or $USER
char* expand_variables(const char *input) {
    char buf[512];
    int pos = 0;
    for (int i = 0; input[i]; i++) {
        if (input[i] == '$') {
            i++;
            char var[128];
            int vpos = 0;
            while (input[i] && (isalnum(input[i]) || input[i] == '_')) {
                var[vpos++] = input[i++];
            }
            var[vpos] = '\0';
            i--; // step back one
            char *val = getenv(var);
            if (val) {
                int len = strlen(val);
                strncpy(&buf[pos], val, len);
                pos += len;
            } else {
                // Undefined variable -> empty string
            }
        } else {
            buf[pos++] = input[i];
        }
    }
    buf[pos] = '\0';
    return strdup(buf);
}

// Built-in command handler
int builtin_cd(char **args) {
    if (!args[1]) {
        fprintf(stderr, "cd: missing argument\n");
        return 1;
    }
    if (chdir(args[1]) != 0) {
        perror("cd failed");
        return 1;
    }
    return 0;
}

int builtin_exit(char **args) {
    exit(0);
}

int builtin_echo(char **args) {
    for (int i = 1; args[i]; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
    return 0;
}

// Dispatch table
struct builtin {
    const char *name;
    int (*func)(char **args);
};

struct builtin builtins[] = {
    {"cd", builtin_cd},
    {"exit", builtin_exit},
    {"echo", builtin_echo},
    {NULL, NULL}
};

// Tokenize input into arguments
char** tokenize(const char *input) {
    char *copy = strdup(input);
    int capacity = 8;
    char **args = malloc(capacity * sizeof(char*));
    int argc = 0;

    char *tok = strtok(copy, " \t");
    while (tok) {
        if (argc >= capacity) {
            capacity *= 2;
            args = realloc(args, capacity * sizeof(char*));
        }
        args[argc++] = strdup(tok);
        tok = strtok(NULL, " \t");
    }
    args[argc] = NULL;
    free(copy);
    return args;
}

void free_args(char **args) {
    for (int i = 0; args[i]; i++) free(args[i]);
    free(args);
}

int main() {
    char input[256];
    while (1) {
        printf("> ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';

        // Expand variables
        char *expanded = expand_variables(input);

        // Tokenize
        char **args = tokenize(expanded);
        free(expanded);

        if (!args[0]) {
            free_args(args);
            continue;
        }

        // Check built-ins
        int handled = 0;
        for (int i = 0; builtins[i].name; i++) {
            if (strcmp(args[0], builtins[i].name) == 0) {
                builtins[i].func(args);
                handled = 1;
                break;
            }
        }

        if (!handled) {
            // External command
            pid_t pid = fork();
            if (pid == 0) {
                execvp(args[0], args);
                perror("exec failed");
                exit(1);
            } else if (pid > 0) {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status)) {
                    printf("Process exited with %d\n", WEXITSTATUS(status));
                }
            } else {
                perror("fork failed");
            }
        }

        free_args(args);
    }
    return 0;
}
