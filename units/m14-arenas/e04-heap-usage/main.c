/* m14-e04-heap-usage [PREDICT]: write PREDICTION.md before you run anything.
 * Usage: build/plain/prog list|arena */
#include <stdlib.h>
#include <string.h>
#include "course.h"

#define N 1000

typedef struct Node {
    struct Node *next;
    int value;
} Node;

/* One malloc per node; one free per node. */
static long with_malloc(void)
{
    Node *head = NULL;
    for (int i = 0; i < N; i++) {
        Node *n = malloc(sizeof *n);
        if (!n)
            break;
        n->value = i;
        n->next = head;
        head = n;
    }
    long sum = 0;
    while (head) {
        Node *next = head->next;
        sum += head->value;
        free(head);
        head = next;
    }
    return sum;
}

/* One block for every node; one free at the end. */
static long with_arena(void)
{
    Node *block = malloc(N * sizeof *block);
    if (!block)
        return -1;
    Node *head = NULL;
    for (int i = 0; i < N; i++) {
        block[i].value = i;
        block[i].next = head;
        head = &block[i];
    }
    long sum = 0;
    for (Node *n = head; n; n = n->next)
        sum += n->value;
    free(block);
    return sum;
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 2;
    long sum = strcmp(argv[1], "arena") == 0 ? with_arena() : with_malloc();
    printf("sum = %ld\n", sum);
    return sum == 499500 ? 0 : 1;
}
