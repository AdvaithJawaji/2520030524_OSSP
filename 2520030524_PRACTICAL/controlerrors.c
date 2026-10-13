#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void logError(const char *msg)
{
    FILE *fp = fopen("error.log", "a");

    if (fp != NULL) {
        fprintf(fp, "ERROR: %s\n", msg);
        fclose(fp);
    }
}

int executeCommand(char *cmd)
{
    if (strcmp(cmd, "start") == 0) {
        printf("Command executed successfully.\n");
        return 0;
    }

    if (strcmp(cmd, "stop") == 0) {
        printf("Process stopped successfully.\n");
        return 0;
    }

    if (strcmp(cmd, "status") == 0) {
        printf("System is running.\n");
        return 0;
    }

    printf("Error: Invalid command syntax.\n");
    logError("Invalid command entered");
    return -1;
}

int main()
{
    char command[50];

    printf("Simple Error Handling System\n");

    while (1) {
        printf("\nEnter command (start/stop/status/exit): ");
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0)
            break;

        if (strlen(command) == 0) {
            printf("Error: Empty command.\n");
            logError("Empty command");
            continue;
        }

        if (executeCommand(command) != 0) {
            printf("Recovering... Please enter a valid command.\n");
        }
    }

    printf("Program terminated safely.\n");
    return 0;
}
