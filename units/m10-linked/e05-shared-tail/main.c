/* m10-e05-shared-tail: build a combined running order from two lists. */
#define _POSIX_C_SOURCE 200809L    /* strdup() under -std=c17; before any #include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"

typedef struct Node {
    char        *name;      /* owned */
    struct Node *next;
} Node;

static void list_destroy(Node *head)
{
    while (head) {
        Node *next = head->next;
        free(head->name);
        free(head);
        head = next;
    }
}

static Node *list_from(const char *const *names, size_t n)   /* front-to-back; NULL on OOM */
{
    Node *head = NULL, **link = &head;
    for (size_t i = 0; i < n; i++) {
        Node *node = malloc(sizeof *node);
        char *name = strdup(names[i]);
        if (!node || !name) {
            free(node);
            free(name);
            list_destroy(head);
            return NULL;
        }
        node->name = name;
        node->next = NULL;
        *link = node;
        link = &node->next;
    }
    return head;
}

/* Returns a new list: a copy of a, followed by b. */
static Node *list_concat(const Node *a, Node *b)
{
    Node *head = NULL, **link = &head;
    for (; a; a = a->next) {
        Node *node = malloc(sizeof *node);
        char *name = strdup(a->name);
        if (!node || !name) {
            free(node);
            free(name);
            list_destroy(head);
            return NULL;
        }
        node->name = name;
        node->next = NULL;
        *link = node;
        link = &node->next;
    }
    *link = b;
    return head;
}

int main(void)
{
    Node *openers = list_from((const char *[]){"Juno", "Vesta"}, 2);
    Node *headliners = list_from((const char *[]){"Ceres", "Pallas"}, 2);
    Node *bill = (openers && headliners) ? list_concat(openers, headliners) : NULL;
    if (!bill) {
        list_destroy(openers);
        list_destroy(headliners);
        fprintf(stderr, "m10-e05-shared-tail: out of memory\n");
        return EXIT_OOM;
    }
    size_t n = 0;
    for (const Node *p = bill; p; p = p->next)
        printf("%zu. %s\n", ++n, p->name);
    check(n == 4, "four acts on the bill");

    list_destroy(bill);
    list_destroy(headliners);
    list_destroy(openers);
    return checks_done("m10-e05-shared-tail");
}
