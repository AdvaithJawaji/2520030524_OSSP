#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    pipe(fd);  // Create a pipe

    if (fork() == 0) {
        // Child 1: executes "ls -l"
        dup2(fd[1], STDOUT_FILENO);  // Redirect stdout to pipe write end
        close(fd[0]);
        close(fd[1]);
        execlp("ls", "ls", "-l", NULL);
    }

    if (fork() == 0) {
        // Child 2: executes "grep .c"
        dup2(fd[0], STDIN_FILENO);   // Redirect stdin to pipe read end
        close(fd[0]);
        close(fd[1]);
        execlp("grep", "grep", ".c", NULL);
    }

    // Parent closes both ends and waits
    close(fd[0]);
    close(fd[1]);
    wait(NULL);
    wait(NULL);

    return 0;
}
