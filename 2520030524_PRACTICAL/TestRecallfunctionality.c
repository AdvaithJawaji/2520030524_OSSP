#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define INIT_HISTORY_SIZE 4
#define MAX_CMD_LEN 256

// Command history structure
typedef struct {
    char **commands;
    int size;
    int capacity;
    int current; // index for navigation
} History;

// Initialize history
History* init_history() {
    History *h = malloc(sizeof(History));
    h->commands = malloc(INIT_HISTORY_SIZE * sizeof(char*));
    h->size = 0;
    h->capacity = INIT_HISTORY_SIZE;
    h->current = -1;
    return h;
}

// Add command to history
void add_history(History *h, const char *cmd) {
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->commands = realloc(h->commands, h->capacity * sizeof(char*));
    }
    h->commands[h->size] = strdup(cmd);
    h->size++;
    h->current = h->size; // reset navigation
}

// Navigate previous
const char* prev_command(History *h) {
    if (h->current > 0) {
        h->current--;
        return h->commands[h->current];
    }
    return NULL;
}

// Navigate next
const char* next_command(History *h) {
    if (h->current < h->size - 1) {
        h->current++;
        return h->commands[h->current];
    }
    return NULL;
}

// Free history
void free_history(History *h) {
    for (int i = 0; i < h->size; i++) {
        free(h->commands[i]);
    }
    free(h->commands);
    free(h);
}

// Enable raw mode for escape sequences
void enable_raw_mode(struct termios *orig) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, orig);
    raw = *orig;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Restore terminal
void disable_raw_mode(struct termios *orig) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig);
}

int main() {
    History *history = init_history();
    struct termios orig;
    enable_raw_mode(&orig);

    char buffer[MAX_CMD_LEN];
    int pos = 0;

    printf("Simple shell (use ↑ ↓ for history, Enter to submit, Ctrl+C to quit)\n");

    while (1) {
        int c = getchar();
        if (c == '\n') {
            buffer[pos] = '\0';
            if (pos > 0) {
                add_history(history, buffer);
                printf("\n[Stored]: %s\n", buffer);
                pos = 0;
            }
            printf("> ");
        } else if (c == 27) { // Escape sequence
            getchar(); // skip '['
            int arrow = getchar();
            const char *cmd = NULL;
            if (arrow == 'A') cmd = prev_command(history); // Up
            else if (arrow == 'B') cmd = next_command(history); // Down
            if (cmd) {
                printf("\r> %s", cmd);
                fflush(stdout);
                strncpy(buffer, cmd, MAX_CMD_LEN);
                pos = strlen(cmd);
            }
        } else {
            if (pos < MAX_CMD_LEN - 1) {
                buffer[pos++] = c;
                putchar(c);
            }
        }
    }

    disable_raw_mode(&orig);
    free_history(history);
    return 0;
}
