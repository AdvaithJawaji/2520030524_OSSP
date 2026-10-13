#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_END
} TokenType;

// Token structure
typedef struct Token {
    TokenType type;
    char *value;
    struct Token *next;
} Token;

// Parse tree node
typedef struct Node {
    char *command;
    struct Node *pipe_to;
} Node;

// Create a new token
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

// Tokenizer: split input by whitespace and delimiters
Token* tokenize(const char *input) {
    Token *head = NULL, *tail = NULL;
    const char *p = input;
    while (*p) {
        while (isspace(*p)) p++; // skip whitespace
        if (*p == '\0') break;

        if (*p == '|') {
            Token *t = new_token(TOKEN_PIPE, "|");
            if (!head) head = t; else tail->next = t;
            tail = t;
            p++;
        } else {
            const char *start = p;
            while (*p && !isspace(*p) && *p != '|') p++;
            char buf[128];
            int len = p - start;
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

// Parser: build simple pipeline structure
Node* parse(Token *tokens) {
    Node *root = NULL, *current = NULL;
    Token *t = tokens;
    while (t && t->type != TOKEN_END) {
        if (t->type == TOKEN_WORD) {
            Node *node = malloc(sizeof(Node));
            node->command = strdup(t->value);
            node->pipe_to = NULL;
            if (!root) root = node;
            if (current) current->pipe_to = node;
            current = node;
        } else if (t->type == TOKEN_PIPE) {
            // Expect next word
            if (!t->next || t->next->type != TOKEN_WORD) {
                fprintf(stderr, "Syntax error: pipe without command\n");
                return NULL;
            }
        }
        t = t->next;
    }
    return root;
}

// Free parse tree
void free_tree(Node *n) {
    while (n) {
        Node *next = n->pipe_to;
        free(n->command);
        free(n);
        n = next;
    }
}

// Debug: print parse tree
void print_tree(Node *n) {
    while (n) {
        printf("Command: %s\n", n->command);
        if (n->pipe_to) printf("  |-> ");
        n = n->pipe_to;
    }
}

int main() {
    char input[256];
    printf("Enter command line: ");
    if (!fgets(input, sizeof(input), stdin)) return 1;
    input[strcspn(input, "\n")] = '\0'; // remove newline

    Token *tokens = tokenize(input);
    Node *tree = parse(tokens);

    if (tree) {
        printf("Parse tree:\n");
        print_tree(tree);
        free_tree(tree);
    }

    free_tokens(tokens);
    return 0;
}
