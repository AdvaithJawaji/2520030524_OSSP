#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum {
    TOKEN_WORD,
    TOKEN_QUOTED_SINGLE,
    TOKEN_QUOTED_DOUBLE,
    TOKEN_END
} TokenType;

// Token structure
typedef struct Token {
    TokenType type;
    char *value;
    struct Token *next;
} Token;

// Create new token
Token* new_token(TokenType type, const char *val) {
    Token *t = malloc(sizeof(Token));
    t->type = type;
    t->value = val ? strdup(val) : NULL;
    t->next = NULL;
    return t;
}

// Free tokens
void free_tokens(Token *t) {
    while (t) {
        Token *next = t->next;
        free(t->value);
        free(t);
        t = next;
    }
}

// Tokenizer with single/double quote handling
Token* tokenize(const char *input) {
    Token *head = NULL, *tail = NULL;
    const char *p = input;

    while (*p) {
        while (isspace(*p)) p++; // skip whitespace
        if (*p == '\0') break;

        if (*p == '\'') { // single quotes
            p++;
            const char *start = p;
            while (*p && *p != '\'') p++;
            int len = p - start;
            char buf[256];
            strncpy(buf, start, len);
            buf[len] = '\0';
            Token *t = new_token(TOKEN_QUOTED_SINGLE, buf);
            if (!head) head = t; else tail->next = t;
            tail = t;
            if (*p == '\'') p++; // skip closing
        } else if (*p == '"') { // double quotes
            p++;
            const char *start = p;
            while (*p && *p != '"') p++;
            int len = p - start;
            char buf[256];
            strncpy(buf, start, len);
            buf[len] = '\0';
            Token *t = new_token(TOKEN_QUOTED_DOUBLE, buf);
            if (!head) head = t; else tail->next = t;
            tail = t;
            if (*p == '"') p++; // skip closing
        } else { // normal word
            const char *start = p;
            while (*p && !isspace(*p) && *p != '\'' && *p != '"') p++;
            int len = p - start;
            char buf[256];
            strncpy(buf, start, len);
            buf[len] = '\0';
            Token *t = new_token(TOKEN_WORD, buf);
            if (!head) head = t; else tail->next = t;
            tail = t;
        }
    }

    Token *end = new_token(TOKEN_END, NULL);
    if (!head) head = end; else tail->next = end;
    return head;
}

// Debug print tokens
void print_tokens(Token *t) {
    while (t && t->type != TOKEN_END) {
        switch (t->type) {
            case TOKEN_WORD: printf("WORD: %s\n", t->value); break;
            case TOKEN_QUOTED_SINGLE: printf("SINGLE QUOTED: '%s'\n", t->value); break;
            case TOKEN_QUOTED_DOUBLE: printf("DOUBLE QUOTED: \"%s\"\n", t->value); break;
            default: break;
        }
        t = t->next;
    }
}

int main() {
    char input[256];
    printf("Enter command: ");
    if (!fgets(input, sizeof(input), stdin)) return 1;
    input[strcspn(input, "\n")] = '\0'; // remove newline

    Token *tokens = tokenize(input);
    printf("Parsed tokens:\n");
    print_tokens(tokens);

    free_tokens(tokens);
    return 0;
}
