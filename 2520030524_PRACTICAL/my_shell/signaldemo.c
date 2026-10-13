#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Signal handler function
void handle_signal(int sig) {
    switch(sig) {
        case SIGINT:
            printf("\nCaught SIGINT (Ctrl+C). Ignoring...\n");
            break;
        case SIGTERM:
            printf("\nCaught SIGTERM. Terminating gracefully...\n");
            exit(0);
            break;
        case SIGUSR1:
            printf("\nCaught SIGUSR1. Custom user signal received!\n");
            break;
        default:
            printf("\nCaught unexpected signal: %d\n", sig);
    }
}

int main() {
    // Register signal handlers
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGUSR1, handle_signal);

    printf("Process running with PID: %d\n", getpid());
    printf("Try sending signals using:\n");
    printf("  kill -SIGUSR1 %d\n", getpid());
    printf("  kill -SIGTERM %d\n", getpid());
    printf("Press Ctrl+C to send SIGINT. (PID: %d)\n", getpid());


    // Infinite loop to keep process alive
    while (1) {
        printf("Waiting for signals...\n");
        sleep(5);
    }

    return 0;
}
