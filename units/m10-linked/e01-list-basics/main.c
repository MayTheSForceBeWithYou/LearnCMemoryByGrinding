/* m10-e01-list-basics: a singly linked list of owned names. */
#define _POSIX_C_SOURCE 200809L    /* strdup() under -std=c17; before any #include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"
#include "list.h"

static int names_are(const Node *n, const char *const *want, size_t count)
{
    for (size_t i = 0; i < count; i++, n = n->next)
        if (!n || strcmp(n->name, want[i]) != 0)
            return 0;
    return n == NULL;
}

static int oom(Node **head)
{
    list_destroy(head);
    fprintf(stderr, "m10-e01-list-basics: out of memory\n");
    return EXIT_OOM;
}

int main(void)
{
    Node *head = NULL;
    check(list_len(head) == 0, "empty list has length 0");
    check(!list_remove_first(&head, "x"), "remove from empty list");
    list_destroy(&head);

    char buf[16];
    strcpy(buf, "bravo");
    if (!list_push_back(&head, buf)) return oom(&head);          /* push_back onto empty */
    strcpy(buf, "XXXXX");
    check(strcmp(head->name, "bravo") == 0, "push copies the name");
    if (!list_push_front(&head, "alpha")) return oom(&head);
    if (!list_push_back(&head, "charlie")) return oom(&head);
    if (!list_push_back(&head, "dup")) return oom(&head);
    if (!list_push_back(&head, "dup")) return oom(&head);
    if (!list_push_back(&head, "echo")) return oom(&head);
    check(names_are(head, (const char *[]){"alpha", "bravo", "charlie", "dup", "dup", "echo"}, 6), "order");
    check(list_len(head) == 6, "length 6");

    check(list_remove_first(&head, "alpha"), "remove head");
    check(list_remove_first(&head, "echo"), "remove tail");
    check(list_remove_first(&head, "dup"), "remove first of two equal names");
    check(!list_remove_first(&head, "zulu"), "not found");
    check(names_are(head, (const char *[]){"bravo", "charlie", "dup"}, 3), "order after removals");
    check(list_remove_first(&head, "charlie") && list_remove_first(&head, "dup")
          && list_remove_first(&head, "bravo"), "remove down to empty");
    check(head == NULL, "head is NULL after the last removal");

    if (!list_push_front(&head, "solo")) return oom(&head);
    list_destroy(&head);
    check(head == NULL, "destroy sets head to NULL");
    return checks_done("m10-e01-list-basics");
}

/* ---- Your definitions of the list.h functions go below this line. ---- */
