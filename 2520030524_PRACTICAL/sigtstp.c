#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t child;
int job_status = 1;   // 1 = Running, 0 = Stopped

void handle_tstp(int sig)
{
    printf("\nSIGTSTP received. Suspending job...\n");
    kill(child, SIGSTOP);
    job_status = 0;
}

int main()
{
    int status;

    signal(SIGTSTP, handle_tstp);

    child = fork();

    if (child == 0) {
        /* Create new process group */
        setpgid(0, 0);

        while (1) {
            printf("Child process running...\n");
            sleep(2);
        }
    }

    /* Parent assigns child to its own group */
    setpgid(child, child);

    printf("Job started. PID = %d\n", child);
    printf("Press Ctrl+Z to suspend the job.\n");

    while (1) {

        waitpid(child, &status, WUNTRACED);

        if (WIFSTOPPED(status)) {
            job_status = 0;
            printf("Job stopped. Status saved.\n");

            printf("Press ENTER to resume...\n");
            getchar();
            getchar();

            /* Resume process group */
            kill(-child, SIGCONT);
            job_status = 1;

            printf("Job resumed.\n");
        }
    }

    return 0;
}
