#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX 10

struct Job {
    int id;
    pid_t pid;
    char command[50];
    char status[15];
};

struct Job jobs[MAX];
int count = 0;

void showJobs() {
    int status;
    for (int i = 0; i < count; i++) {
        if (strcmp(jobs[i].status, "Running") == 0) {
            if (waitpid(jobs[i].pid, &status, WNOHANG) > 0)
                strcpy(jobs[i].status, "Completed");
        }
        printf("[%d] PID:%d  %s  %s\n",
               jobs[i].id, jobs[i].pid,
               jobs[i].command, jobs[i].status);
    }
}

int main() {
    char command[50];

    printf("Background Job Manager\n");

    while (1) {
        printf("\n> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0)
            break;

        if (strcmp(command, "jobs") == 0) {
            showJobs();
            continue;
        }

        if (command[strlen(command) - 1] == '&') {
            command[strlen(command) - 1] = '\0';

            pid_t pid = fork();

            if (pid == 0) {
                execlp(command, command, NULL);
                exit(1);
            }

            jobs[count].id = count + 1;
            jobs[count].pid = pid;
            strcpy(jobs[count].command, command);
            strcpy(jobs[count].status, "Running");
            count++;

            printf("[%d] Started in background\n", count);
        }
    }

    return 0;
}
