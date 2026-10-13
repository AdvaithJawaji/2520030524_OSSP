#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

// Tokenize command into arguments
char** tokenize(char *cmd) {
    char **args = malloc(32 * sizeof(char*));
    int argc = 0;
    char *tok = strtok(cmd, " \t");
    while (tok && argc < 31) {
        args[argc++] = tok;
        tok = strtok(NULL, " \t");
    }
    args[argc] = NULL;
    return args;
}

// Execute pipeline with redirection
void execute_pipeline(char *input) {
    char *commands[16];
    int cmd_count = 0;

    // Split input into pipeline segments
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
            // Handle pipe redirection
            if (i > 0) dup2(pipes[(i-1)*2], STDIN_FILENO);
            if (i < cmd_count - 1) dup2(pipes[i*2+1], STDOUT_FILENO);

            // Close all pipes
            for (int j = 0; j < 2*(cmd_count-1); j++) close(pipes[j]);

            // Parse redirection inside command
            char *outfile = NULL, *errfile = NULL;
            int append_out = 0, append_err = 0, merge_err = 0;
            char command[256] = "";
            char *tok2 = strtok(commands[i], " ");
            while (tok2) {
                if (strcmp(tok2, ">") == 0) {
                    tok2 = strtok(NULL, " ");
                    outfile = tok2; append_out = 0;
                } else if (strcmp(tok2, ">>") == 0) {
                    tok2 = strtok(NULL, " ");
                    outfile = tok2; append_out = 1;
                } else if (strcmp(tok2, "2>") == 0) {
                    tok2 = strtok(NULL, " ");
                    errfile = tok2; append_err = 0;
                } else if (strcmp(tok2, "2>>") == 0) {
                    tok2 = strtok(NULL, " ");
                    errfile = tok2; append_err = 1;
                } else if (strcmp(tok2, "2>&1") == 0) {
                    merge_err = 1;
                } else {
                    strcat(command, tok2);
                    strcat(command, " ");
                }
                tok2 = strtok(NULL, " ");
            }

            // Tokenize command part
            char **args = tokenize(command);

            // Output redirection
            if (outfile) {
                int fd = open(outfile,
                              O_WRONLY | O_CREAT | (append_out ? O_APPEND : O_TRUNC),
                              0644);
                if (fd < 0) { perror("Cannot open output file"); exit(1); }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // Error redirection
            if (errfile) {
                int fd = open(errfile,
                              O_WRONLY | O_CREAT | (append_err ? O_APPEND : O_TRUNC),
                              0644);
                if (fd < 0) { perror("Cannot open error file"); exit(1); }
                dup2(fd, STDERR_FILENO);
                close(fd);
            }

            // Merge stderr into stdout
            if (merge_err) {
                dup2(STDOUT_FILENO, STDERR_FILENO);
            }

            execvp(args[0], args);
            perror("exec failed");
            exit(1);
        }
    }

    // Parent closes pipes
    for (int j = 0; j < 2*(cmd_count-1); j++) close(pipes[j]);

    // Wait for all children
    for (int i = 0; i < cmd_count; i++) {
        int status;
        wait(&status);
    }

    for (int i = 0; i < cmd_count; i++) free(commands[i]);
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

