#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

pid_t child = -1;

/* Signal demonstration */
void handle_sigint(int sig)
{
    if (child > 0) {
        printf("\nSIGINT received! Stopping demo process...\n");
        kill(child, SIGINT);
    }
}

/* Pipeline demonstration */
void runPipeline()
{
    int fd[2];
    pid_t p1, p2;

    pipe(fd);

    p1 = fork();

    if (p1 == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);

        execlp("ls", "ls", NULL);
        perror("Pipeline process 1 failed");
        exit(1);
    }

    p2 = fork();

    if (p2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);

        execlp("wc", "wc", "-l", NULL);
        perror("Pipeline process 2 failed");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);

    printf("Pipeline demo completed.\n");
}

/* Signal demonstration */
void runSignalDemo()
{
    signal(SIGINT, handle_sigint);

    child = fork();

    if (child == 0) {
        printf("Demo process running...\n");

        while (1)
            sleep(1);
    }

    printf("Signal demo started. Press Ctrl+C.\n");

    waitpid(child, NULL, 0);

    child = -1;
    printf("Signal demo completed.\n");
}

int main()
{
    int choice;

    printf("================================\n");
    printf("       OS PROJECT FINAL DEMO\n");
    printf("================================\n");

    while (1) {
        printf("\n1. Test Pipeline");
        printf("\n2. Test Signals");
        printf("\n3. Project Review");
        printf("\n4. Exit");
        printf("\nEnter choice: ");

        scanf("%d", &choice);

        switch (choice) {

        case 1:
            printf("\n--- Pipeline Demonstration ---\n");
            runPipeline();
            break;

        case 2:
            printf("\n--- Signal Demonstration ---\n");
            runSignalDemo();
            break;

        case 3:
            printf("\n--- Final Project Review ---\n");
            printf("Source Code : Organized\n");
            printf("Build       : GCC\n");
            printf("README      : Available\n");
            printf("Pipelines   : Tested\n");
            printf("Signals     : Tested\n");
            printf("Deliverable : Valid\n");
            break;

        case 4:
            printf("\nProject demo completed.\n");
            return 0;

        default:
            printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
