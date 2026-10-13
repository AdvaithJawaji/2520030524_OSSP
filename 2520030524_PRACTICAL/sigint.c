#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t child = -1;
int job_running = 0;

/* SIGINT handler */
void handle_sigint(int sig)
{
    if (child > 0 && job_running) {
        printf("\nSIGINT received. Terminating foreground job...\n");

        /* Forward SIGINT to child */
        kill(child, SIGINT);
    }
    else {
        printf("\nShell protected from SIGINT.\n");
    }
}

int main()
{
    signal(SIGINT, handle_sigint);

    printf("Simple Signal Handling Shell\n");

    while (1) {
        printf("\nEnter 1 to start job, 2 to exit: ");

        int choice;
        scanf("%d", &choice);

        if (choice == 2)
            break;

        if (choice == 1) {

            child = fork();

            if (child == 0) {
                printf("Foreground job running...\n");

                /* Simulate long-running job */
                while (1)
                    sleep(1);
            }

            job_running = 1;

            /* Shell waits while protecting itself from SIGINT */
            waitpid(child, NULL, 0);

            job_running = 0;
            child = -1;

            printf("Foreground job terminated.\n");
        }
    }

    printf("Shell exiting...\n");
    return 0;
}
