# m10-e04-tree-free `[WRITE]` (M)

## Goal

A binary search tree mapping string keys to `int` counts. Each node owns a copy of its key.
`tree.h` declares:

```c
typedef struct TreeNode { char *key; int count; struct TreeNode *left, *right; } TreeNode;

bool tree_add(TreeNode **root, const char *key);   /* insert with count 1, or increment */
int  tree_count(const TreeNode *root, const char *key);  /* 0 if absent */
void tree_free(TreeNode *root);
```

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.
> Write your definitions in `main.c`, below the marked line.

## Constraints

- Ordering by `strcmp`. Duplicate keys increment the count and allocate nothing.
- A failed `tree_add` leaves the tree unchanged and leaks nothing.
- `tree_free` frees every key and every node and accepts NULL. Recursion is allowed.
- Add a comment to `tree.h` stating the depth limit of your `tree_free` and the input that hits it.
- `tree_add` and `tree_count` must not recurse.
- Under strict `-std=c17`, `strdup` is declared only because `main.c` starts with
  `#define _POSIX_C_SOURCE 200809L`, ahead of every `#include`. Keep it there.

## Verify

```bash
make check
```

The harness prints `m10-e04-tree-free: all checks passed`. It inserts a few thousand keys,
3,000 of them sorted, so a degenerate subtree occurs. That still fits the default stack.

## Afterwards (optional)

Raise `SORTED` in the harness until `tree_free` crashes. Note the count on the plain build and on
`make asan` (ASan names it `stack-overflow`), and compare it with `ulimit -s`. Don't commit the change.
