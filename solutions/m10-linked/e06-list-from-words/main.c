/* m10-e06-list-from-words: build a list from an array, all-or-nothing. */
#define _POSIX_C_SOURCE 200809L    /* strdup() under -std=c17; before any #include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"

typedef struct Node {
    char        *name;      /* owned */
    struct Node *next;
} Node;

/* Returns a list holding copies of words[0..n-1] in order. The caller owns it; free with
 * list_destroy. Returns NULL on allocation failure (having freed everything it allocated),
 * and also returns NULL when n == 0. */
Node *list_from_words(const char *const *words, size_t n);

/* ---- Provided, already correct. ---- */

static void list_destroy(Node **head)
{
    Node *n = *head;
    while (n) {
        Node *next = n->next;
        free(n->name);
        free(n);
        n = next;
    }
    *head = NULL;
}

int main(void)
{
    static const char *const words[] = {"the", "quick", "brown", "fox", "jumps"};
    const size_t n = sizeof words / sizeof words[0];

    check(list_from_words(words, 0) == NULL, "n == 0 gives the empty list");

    Node *list = list_from_words(words, n);
    if (!list) {                      /* n > 0, so NULL can only mean out of memory */
        fprintf(stderr, "m10-e06-list-from-words: out of memory\n");
        return EXIT_OOM;
    }
    size_t i = 0;
    int in_order = 1;
    for (const Node *p = list; p; p = p->next, i++)
        in_order &= i < n && strcmp(p->name, words[i]) == 0 && p->name != words[i];
    check(in_order && i == n, "every word copied, in order");
    list_destroy(&list);
    return checks_done("m10-e06-list-from-words");
}

/* ---- Your definition of list_from_words goes below this line. ---- */

Node *list_from_words(const char *const *words, size_t n)
{
    Node *head = NULL, **link = &head;      /* link: where the next node goes; O(1) append */
    for (size_t i = 0; i < n; i++) {
        Node *node = malloc(sizeof *node);
        if (!node)
            goto fail;
        node->name = strdup(words[i]);
        if (!node->name) {
            free(node);                     /* not linked yet: free it on its own */
            goto fail;
        }
        node->next = NULL;
        *link = node;
        link = &node->next;
    }
    return head;
fail:
    list_destroy(&head);
    return NULL;
}
