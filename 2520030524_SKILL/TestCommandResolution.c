#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

// Search for executable in PATH
char* resolve_command(const char *cmd) {
    char *path = getenv("PATH");
    if (!path) return NULL;

    char *paths = strdup(path);
    char *token = strtok(paths, ":");
    struct stat st;

    while (token) {
        char fullpath[512];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", token, cmd);
        if (stat(fullpath, &st) == 0 && (st.st_mode & S_IXUSR)) {
            free(paths);
            return strdup(fullpath); // found executable
        }
        token = strtok(NULL, ":");
    }
    free(paths);
    return NULL; // not found
}

int main() {
    char input[128];
    printf("Enter command to run: ");
    if (!fgets(input, sizeof(input), stdin)) return 1;
    input[strcspn(input, "\n")] = '\0'; // remove newline

    // Resolve command path
    char *cmdpath = resolve_command(input);
    if (!cmdpath) {
        fprintf(stderr, "Command '%s' not found in PATH\n", input);
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execl(cmdpath, input, NULL);
        perror("Execution failed");
        exit(1);
    } else if (pid > 0) {
        // Parent process monitors child
        int status;
        pid_t wpid = waitpid(pid, &status, 0);
        if (wpid == -1) {
            perror("waitpid failed");
        } else {
            if (WIFEXITED(status)) {
                printf("Child exited with status %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Child terminated by signal %d\n", WTERMSIG(status));
            }
        }
    } else {
        perror("Fork failed");
    }

    free(cmdpath);
    return 0;
}
