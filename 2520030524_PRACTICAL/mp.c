#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
    int pipe1[2], pipe2[2];
    pid_t p1, p2, p3;
    int status;

    pipe(pipe1);
    pipe(pipe2);

    /* Process 1: ls */
    p1 = fork();

    if (p1 == 0) {
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);

        execlp("ls", "ls", NULL);
        perror("ls failed");
        exit(1);
    }

    /* Process 2: grep */
    p2 = fork();

    if (p2 == 0) {
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("grep", "grep", ".c", NULL);
        perror("grep failed");
        exit(1);
    }

    /* Process 3: wc */
    p3 = fork();

    if (p3 == 0) {
        dup2(pipe2[0], STDIN_FILENO);

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("wc", "wc", "-l", NULL);
        perror("wc failed");
        exit(1);
    }

    /* Parent closes pipes */
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    /* Monitor processes */
    waitpid(p1, &status, 0);
    printf("Job 1 completed.\n");

    waitpid(p2, &status, 0);
    printf("Job 2 completed.\n");

    waitpid(p3, &status, 0);
    printf("Job 3 completed.\n");

    printf("\nPipeline execution completed successfully.\n");

    return 0;
}
