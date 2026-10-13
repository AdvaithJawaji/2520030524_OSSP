#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

// Tokenize command into arguments
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

void execute_command(char *input) {
    char *infile = NULL, *outfile = NULL;
    int append = 0;

    // Parse redirection
    char *cmd = strdup(input);
    char *tok = strtok(cmd, " ");
    char command[256] = "";
    while (tok) {
        if (strcmp(tok, "<") == 0) {
            tok = strtok(NULL, " ");
            infile = tok;
        } else if (strcmp(tok, ">") == 0) {
            tok = strtok(NULL, " ");
            outfile = tok;
            append = 0;
        } else if (strcmp(tok, ">>") == 0) {
            tok = strtok(NULL, " ");
            outfile = tok;
            append = 1;
        } else {
            strcat(command, tok);
            strcat(command, " ");
        }
        tok = strtok(NULL, " ");
    }

    // Tokenize command part
    char **args = tokenize(command);

    pid_t pid = fork();
    if (pid == 0) {
        // Input redirection
        if (infile) {
            int fd = open(infile, O_RDONLY);
            if (fd < 0) {
                perror("Cannot open input file");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        // Output redirection
        if (outfile) {
            int fd;
            if (append)
                fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
            else
                fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

            if (fd < 0) {
                perror("Cannot open output file");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

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

    free(args);
    free(cmd);
}

int main() {
    char input[256];
    while (1) {
        printf("> ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;

        execute_command(input);
    }
    return 0;
}
