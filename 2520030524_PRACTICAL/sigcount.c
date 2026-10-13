#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

struct Job {
    int id;
    pid_t pid;
    int status;       // 1 = Running, 0 = Stopped
};

struct Job job;

void checkStatus() {
    int status;
    pid_t r = waitpid(job.pid, &status, WNOHANG | WUNTRACED);

    if (r > 0) {
        if (WIFSTOPPED(status)) {
            job.status = 0;
            printf("Job [%d] Stopped\n", job.id);
        }
        else if (WIFCONTINUED(status)) {
            job.status = 1;
            printf("Job [%d] Running\n", job.id);
        }
    }
}

int main() {
    pid_t pid;

    printf("Signal and Job Recovery System\n");

    pid = fork();

    if (pid == 0) {
        /* Create a separate process group */
        setpgid(0, 0);

        while (1) {
            printf("Child process running...\n");
            sleep(2);
        }
    }

    job.id = 1;
    job.pid = pid;
    job.status = 1;

    sleep(2);

    /* Stop the job */
    printf("\nStopping Job [1]...\n");
    kill(pid, SIGSTOP);
    sleep(1);

    checkStatus();

    /* Resume the stopped job */
    printf("\nSending SIGCONT...\n");
    kill(pid, SIGCONT);

    sleep(1);

    /* Check updated status */
    checkStatus();

    printf("\nJob resumed successfully.\n");

    /* Stop and terminate child */
    kill(pid, SIGTERM);
    waitpid(pid, NULL, 0);

    return 0;
}
