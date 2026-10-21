# m10-e01-list-basics `[WRITE]` (M)

## Goal

A singly linked list of names, each node owning a heap copy of its name. `list.h` declares:

```c
typedef struct Node { char *name; struct Node *next; } Node;

bool   list_push_front(Node **head, const char *name);
bool   list_push_back(Node **head, const char *name);
bool   list_remove_first(Node **head, const char *name);  /* true if one was removed */
size_t list_len(const Node *head);
void   list_destroy(Node **head);                         /* sets *head = NULL */
```

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.
> Write your definitions in `main.c`, below the marked line.

## Constraints

- Each node owns a copy of `name`. The caller's string is borrowed only for the duration of the call.
- A push that fails at either allocation (node or copy) leaves the list unchanged and leaks nothing.
- `list_remove_first` handles the first node, the last node, a one-node list and "not found",
  without separate code paths for the head. The reviewer will look.
- `list_destroy` frees every name and node and leaves `*head == NULL`. It accepts an empty list.
- `list_push_back` may walk the list (O(n)). No tail pointer is required.
- Under strict `-std=c17`, `strdup` is declared only because `main.c` starts with
  `#define _POSIX_C_SOURCE 200809L`, ahead of every `#include`. Keep it there.

## Verify

```bash
make check
```

The harness prints `m10-e01-list-basics: all checks passed`; ASan and Valgrind are silent.
