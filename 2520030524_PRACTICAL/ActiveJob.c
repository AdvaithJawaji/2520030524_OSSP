#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

struct Job {
    int id;
    pid_t pid;
    int running;
};

struct Job jobs[5];
int count = 0;

void listJobs() {
    printf("\nActive Jobs:\n");

    for (int i = 0; i < count; i++) {
        printf("[%d] PID=%d  Status=%s\n",
               jobs[i].id,
               jobs[i].pid,
               jobs[i].running ? "Running" : "Completed");
    }
}

void foreground(int id) {
    for (int i = 0; i < count; i++) {

        if (jobs[i].id == id) {
            printf("Bringing Job [%d] to foreground...\n", id);

            /* Resume process */
            kill(jobs[i].pid, SIGCONT);

            /* Wait for completion */
            waitpid(jobs[i].pid, NULL, 0);

            /* Update state */
            jobs[i].running = 0;

            printf("Job [%d] completed.\n", id);
            return;
        }
    }

    printf("Job not found.\n");
}

int main() {
    int choice, id;
    pid_t pid;

    printf("Job Control System\n");

    while (1) {
        printf("\n1. Start Job");
        printf("\n2. List Jobs");
        printf("\n3. Foreground Job");
        printf("\n4. Exit");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            pid = fork();

            if (pid == 0) {
                execlp("sleep", "sleep", "10", NULL);
            }

            jobs[count].id = count + 1;
            jobs[count].pid = pid;
            jobs[count].running = 1;
            count++;

            printf("Job [%d] started.\n", count);
        }

        else if (choice == 2) {
            listJobs();
        }

        else if (choice == 3) {
            printf("Enter Job ID: ");
            scanf("%d", &id);
            foreground(id);
        }

        else if (choice == 4) {
            break;
        }

        else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
