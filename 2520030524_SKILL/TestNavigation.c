#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Built‑in command prototypes
int builtin_cd(char **args);
int builtin_pwd(char **args);
int builtin_exit(char **args);

// Dispatch table
struct builtin {
    const char *name;
    int (*func)(char **args);
};

struct builtin builtins[] = {
    {"cd", builtin_cd},
    {"pwd", builtin_pwd},
    {"exit", builtin_exit},
    {NULL, NULL}
};

// Track previous directory
char prev_dir[256] = "";

// Built‑in: change directory
int builtin_cd(char **args) {
    char cwd[256];
    getcwd(cwd, sizeof(cwd));

    if (!args[1]) {
        fprintf(stderr, "cd: missing argument\n");
        return 1;
    }

    if (strcmp(args[1], "-") == 0) {
        if (strlen(prev_dir) == 0) {
            fprintf(stderr, "cd: no previous directory\n");
            return 1;
        }
        if (chdir(prev_dir) != 0) {
            perror("cd failed");
            return 1;
        }
        printf("%s\n", prev_dir);
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd failed");
            return 1;
        }
    }

    strcpy(prev_dir, cwd);
    return 0;
}

// Built‑in: print working directory
int builtin_pwd(char **args) {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s\n", cwd);
    else
        perror("pwd failed");
    return 0;
}

// Built‑in: exit shell
int builtin_exit(char **args) {
    printf("Exiting shell...\n");
    exit(0);
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

        // Check built‑ins
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
