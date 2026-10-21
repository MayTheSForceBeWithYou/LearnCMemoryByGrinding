#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stddef.h>

/* A singly linked list of names. The list owns every node; each node owns its name.
 * A list is a Node * head; NULL is the empty list. Functions that may change the first
 * node take Node **head. */
typedef struct Node {
    char        *name;
    struct Node *next;
} Node;

/* Adds a copy of name at the front / back. name is borrowed for the call.
 * On failure returns false; the list is unchanged and nothing leaked. */
bool   list_push_front(Node **head, const char *name);
bool   list_push_back(Node **head, const char *name);
/* Unlinks and frees the first node whose name equals name. True if one was removed. */
bool   list_remove_first(Node **head, const char *name);
size_t list_len(const Node *head);
/* Frees every name and node; sets *head = NULL. Accepts an empty list. */
void   list_destroy(Node **head);

#endif
