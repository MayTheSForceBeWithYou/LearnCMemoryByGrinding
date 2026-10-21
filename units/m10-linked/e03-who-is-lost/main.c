/* m10-e03-who-is-lost: three lists, three mistakes. Predict the leak report first. */
#define _POSIX_C_SOURCE 200809L    /* strdup() under -std=c17; before any #include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"

typedef struct Node {
    char        *name;      /* owned */
    struct Node *next;
} Node;

static Node *push(Node *head, const char *name)   /* no OOM handling: this file is about leaks */
{
    Node *n = malloc(sizeof *n);
    if (!n)
        exit(EXIT_OOM);
    n->name = strdup(name);
    if (!n->name)
        exit(EXIT_OOM);
    n->next = head;
    return n;
}

static Node *build(const char *const *names, size_t count)
{
    Node *head = NULL;
    for (size_t i = 0; i < count; i++)
        head = push(head, names[i]);
    return head;
}

static void destroy_nodes_only(Node *head)          /* frees nodes, forgets the names */
{
    while (head) {
        Node *next = head->next;
        free(head);
        head = next;
    }
}

static void destroy(Node *head)
{
    while (head) {
        Node *next = head->next;
        free(head->name);
        free(head);
        head = next;
    }
}

static size_t case_a(void)            /* A: 3 names; the head pointer is overwritten */
{
    Node *head = build((const char *[]){"ant", "bee", "cat"}, 3);
    size_t len = strlen(head->name);
    head = NULL;
    return len;
}

static size_t case_b(void)            /* B: 4 names; destroy forgets the names */
{
    Node *head = build((const char *[]){"dog", "eel", "fox", "gnu"}, 4);
    size_t len = strlen(head->next->name);
    destroy_nodes_only(head);
    return len;
}

static size_t case_c(void)            /* C: 3 names; the middle node is unlinked, never freed */
{
    Node *head = build((const char *[]){"hen", "ibis", "jay"}, 3);
    Node *middle = head->next;
    head->next = middle->next;
    size_t len = strlen(middle->name);
    middle = NULL;
    destroy(head);
    return len;
}

int main(void)
{
    check(case_a() == 3, "A ran");
    check(case_b() == 3, "B ran");
    check(case_c() == 4, "C ran");
    return checks_done("m10-e03-who-is-lost");
}
