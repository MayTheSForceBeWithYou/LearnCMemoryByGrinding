/* m11-e04-direct-indirect [PREDICT]: write PREDICTION.md before you run anything. */
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "course.h"

typedef struct Node {
    struct Node *next;
    char *name; /* owned */
} Node;

/* Prepends a node owning a copy of name. Returns the new head, or NULL on
 * allocation failure (the old list is untouched). */
static Node *push(Node *head, const char *name)
{
    Node *n = malloc(sizeof *n);
    if (!n)
        return NULL;
    n->name = strdup(name);
    if (!n->name) {
        free(n);
        return NULL;
    }
    n->next = head;
    return n;
}

static void list_free(Node *head)
{
    while (head) {
        Node *next = head->next;
        free(head->name);
        free(head);
        head = next;
    }
}

int main(void)
{
    /* A: a three-node list whose head pointer is simply forgotten. */
    Node *a = NULL;
    const char *names[] = {"ann", "bob", "cy"};
    for (int i = 0; i < 3; i++) {
        Node *n = push(a, names[i]);
        if (!n) {
            list_free(a);
            return EXIT_OOM;
        }
        a = n;
    }
    check(strcmp(a->name, "cy") == 0, "head is the last name pushed");
    a = NULL;

    /* B: a table of two int blocks; only the table is freed. */
    int **table = malloc(2 * sizeof *table);
    if (!table)
        return EXIT_OOM;
    table[0] = malloc(10 * sizeof **table);
    table[1] = malloc(20 * sizeof **table);
    if (!table[0] || !table[1]) {
        free(table[0]);
        free(table[1]);
        free(table);
        return EXIT_OOM;
    }
    free(table);

    return checks_done("m11-e04-direct-indirect");
}
