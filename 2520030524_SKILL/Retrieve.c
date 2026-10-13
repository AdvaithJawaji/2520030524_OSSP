#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Built-in prototypes
int builtin_pwd(char **args);
int builtin_exit(char **args);
int builtin_export(char **args);

// Dispatch table
struct builtin {
    const char *name;
    int (*func)(char **args);
};

struct builtin builtins[] = {
    {"pwd", builtin_pwd},
    {"exit", builtin_exit},
    {"export", builtin_export},
    {NULL, NULL}
};

// Built-in: print current directory
int builtin_pwd(char **args) {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s\n", cwd);
    else
        perror("pwd failed");
    return 0;
}

// Built-in: exit shell
int builtin_exit(char **args) {
    printf("Exiting shell...\n");
    // cleanup resources if needed
    exit(0);
}

// Built-in: export environment variable
int builtin_export(char **args) {
    if (!args[1]) {
        fprintf(stderr, "export: missing argument (VAR=VALUE)\n");
        return 1;
    }

    char *eq = strchr(args[1], '=');
    if (!eq) {
        fprintf(stderr, "export: invalid syntax, use VAR=VALUE\n");
        return 1;
    }

    *eq = '\0';
    char *name = args[1];
    char *value = eq + 1;

    if (setenv(name, value, 1) != 0) {
        perror("export failed");
        return 1;
    }

    printf("Exported %s=%s\n", name, value);
    return 0;
}

// Tokenize input
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

        char **args = tokenize(input);
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

        // External command
        if (!handled) {
            pid_t pid = fork();
            if (pid == 0) {
                execvp(args[0], args);
                perror("exec failed");
                exit(1);
            } else if (pid > 0) {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status))
                    printf("Process exited with %d\n", WEXITSTATUS(status));
            } else {
                perror("fork failed");
            }
        }

        free_args(args);
    }
    return 0;
}
