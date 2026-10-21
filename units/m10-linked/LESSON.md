# M10: Linked structures

## Organizer

A vector is one block that moves. A linked structure is many small blocks that never move,
wired together by pointers stored *inside* the blocks. That changes which bugs you get.
Interior pointers into a list node stay valid as long as the node lives. In exchange, every
node is a separate allocation with a separate lifetime, and the pointer you need to reach the
next node is stored in memory you are about to free.

Prerequisites: M05 (ownership contracts), M06 (cleanup on failure), M07 (heap strings),
and the LeakSanitizer direct/indirect distinction from M02b and M08 e03.

This unit covers:

- a singly linked list: create, insert, remove, free;
- the pointer-to-pointer (`Node **`) technique that removes head special cases;
- why the obvious free loop is a use-after-free;
- freeing a tree recursively, and the cost of recursion depth;
- shared nodes, and the double free that follows.

## Foundations

### A node owns what hangs off it

```c
typedef struct Node {
    char        *name;   /* owned by the node */
    struct Node *next;   /* the rest of the list */
} Node;
```

The contract for most lists: **the list owns its nodes, and each node owns its payload.**
Freeing a node therefore means freeing its payload first, then the node. Lose the head
pointer and LeakSanitizer reports the head node as a *direct* leak, and everything reachable
only through it (the other nodes, every `name`) as *indirect*. One mistake, many lines of report.
Fix the direct leak and the indirect ones go with it.

### The head is a pointer the caller owns

Functions that can change the first node (push-front, remove, insert-sorted) must be able to
change the caller's head pointer. Two styles:

- **return the new head**: `head = list_push_front(head, x);`. Easy to call wrong (forgetting
  the assignment is a leak or a dangling head), and there's no room for an error return.
- **take `Node **head`** and return a status: `if (!list_push_front(&head, x)) ...`.

This course uses the second style. It composes with error handling.

### Pointer to pointer: one code path for every position

Walk a `Node **pp` that points *at the link* you might change, starting at `&head`. Then
"remove this node" is always `*pp = victim->next`, whether the link is `head` or some
node's `next` field. There's no special case for the first node, and no `prev` variable.

### Freeing a list

The node holds the only pointer to the rest of the list. After `free(n)`, reading `n->next` is
a use-after-free. What the plain build does next depends on where `next` sits. Since glibc 2.26, a
small freed chunk goes into the tcache, which writes a free-list pointer into the chunk's first 8 bytes.
Since 2.29 it also writes a key into the next 8, and since 2.32 the pointer is mangled (safe-linking). In the `Node` above, `next` is at offset 8, so it gets
overwritten: on the course machine, the naive loop over this struct segfaulted on the plain build.
Put `next` past offset 16, as e02's `Track` does, and the stale value survives, so the same bug ran
clean on every plain run we tried. Neither outcome is a guarantee. Read `next` **before** freeing:

```c
while (n) {
    Node *next = n->next;
    free(n->name);
    free(n);
    n = next;
}
```

### Trees

A binary tree node has two owned children. Freeing it must free both subtrees, then the node.
That is post-order, and recursion expresses it directly:

```c
void tree_free(TreeNode *t)
{
    if (!t)
        return;
    tree_free(t->left);
    tree_free(t->right);
    free(t->key);
    free(t);
}
```

The catch is depth. A binary search tree fed sorted keys degenerates into a list, and the
recursion depth equals the number of nodes. The main thread's stack is limited, and 8 MiB is the usual
`ulimit -s` default, not a guarantee. Run `ulimit -s` to see yours. On the course machine (8192 KiB),
`lesson-demo/` recursively freed a degenerate tree of this node shape at 250,000 nodes and crashed at 300,000,
in both the plain build (segfault) and the ASan build (`stack-overflow` in `tree_free`); see its `capture-*.txt`. ASan frames are usually bigger, so expect it to overflow sooner
under ASan, although at that resolution we saw no difference. Production code uses an explicit stack
or a rotation trick. Here it is enough to know the limit exists and to state it in the contract.

### Shared nodes

Two lists may end up pointing at the same tail. Maybe someone "saved memory" by splicing, or
a copy function linked the other list in instead of copying its nodes. If both lists believe they own
the tail, destroying both is a bug, but the first error is usually not the double free. The second destroy
must read `name` and `next` from the first shared node, which the first destroy already freed. So ASan's
first report is a **heap-use-after-free READ** inside the destroy loop, and with `halt_on_error=1` it stops
there, before any `free` runs twice. e05 is exactly this. A double free with nothing read in between,
such as `free(p); free(p);`, is what ASan reports as `attempting double-free`, and plain glibc as
`free(): double free detected in tcache 2` (both captured on the course machine; M12 covers reading them).

The fix is not a smarter free function. It's a contract: **each node has exactly one owner.**
If sharing is genuinely needed, the shared part needs an owner of its own, or a reference count.
That is outside this unit.

## Worked example: remove every match

A neighbour of e01: remove *all* nodes whose name equals `key`, freeing them.

```c
#include <string.h>

/* Removes and frees every node whose name equals key. Returns the number removed.
   head: the caller's head pointer; may be updated. */
size_t list_remove_all(Node **head, const char *key)
{
    size_t removed = 0;
    Node **pp = head;
    while (*pp) {
        Node *n = *pp;
        if (strcmp(n->name, key) == 0) {
            *pp = n->next;          /* unlink first: the list never points at freed memory */
            free(n->name);
            free(n);
            removed++;              /* pp stays put: *pp is now the next candidate */
        } else {
            pp = &n->next;
        }
    }
    return removed;
}
```

**The plausible wrong reading.** "After removing a node I should advance, like in the else branch.
Otherwise I re-examine the same position forever." But `*pp` already changed: it now names the
node *after* the one removed. Advancing would skip that node, so two adjacent matches leave the
second one in the list. Tests that only remove isolated matches pass. The harness for e01 has adjacent
matches for this reason.

A second one: "unlinking after the free is fine, it's the next line." `free(n)` then `*pp = n->next`
reads `n->next` from freed memory, which is exactly the free-loop bug in a different shape.

## `man` pointers

- `man 3 free`: "if `ptr` has already been freed, undefined behavior occurs."
- `man 3 strcmp`, `man 3 strdup`: payload handling.
- `man 1 bash`, search `ulimit`: stack size, for the tree-depth question.
- `man 3 mallopt`, `M_PERTURB` / `MALLOC_PERTURB_` (also `GLIBC_TUNABLES=glibc.malloc.perturb=N`):
  fills freed memory with a pattern. Caveat measured on the course machine: it did **not** change e02's
  plain run, because chunks freed into the tcache are not perturbed. With the tcache disabled as well
  (`GLIBC_TUNABLES=glibc.malloc.tcache_count=0`), the same run segfaulted. It has no effect under ASan,
  which uses its own allocator.

## Check yourself

1. A 4-node list, each node with a heap `name`, loses its head pointer. How many blocks leak?
   How many does LeakSanitizer call direct?
2. Why does `list_remove_all` need `Node **` instead of `Node *`?
3. What goes wrong if the tree free function frees the node before recursing into its children?
4. Your plain-build run of a buggy free loop exits 0 with correct output. What does that tell you?
5. Two lists share a tail of 3 nodes. You destroy both with the usual read-`next`-first loop. What is
   the first error ASan reports, and where?

<details><summary>Answers</summary>

1. 8 blocks (4 nodes + 4 names). One direct (the head node). Seven indirect.
2. It may have to change the caller's `head` when the first node matches.
3. It reads `t->left`/`t->right` from freed memory: a use-after-free on every non-leaf node.
4. Nothing about correctness. Freed memory often still holds its old bytes. Use ASan (or Valgrind).
   Allocator tricks such as `MALLOC_PERTURB_` help only for chunks that aren't cached.
5. A heap-use-after-free READ in the second destroy, when it reads the first shared node's fields. That
   node was freed by the first destroy. ASan halts there (`halt_on_error=1`), so no `free` ever runs twice.

</details>

## Takeaways

- Each node has exactly one owner. Each node owns its payload.
- Read `next` before you free the node. Unlink before you free.
- `Node **pp` turns head insertion and removal into the ordinary case.
- Recursive tree free is post-order, and its depth is the tree's height. Say so in the contract.
- A plain run that "works" says nothing about a use-after-free.

## Exercises

| # | Folder | Tag | Size | What |
|---|---|---|---|---|
| 1 | `e01-list-basics` | `[WRITE]` | M | Push front/back, remove first match, destroy, with owned names |
| 2 | `e02-free-loop` | `[BUG]` | | A destroy function that works on every plain run |
| 3 | `e03-who-is-lost` | `[PREDICT]` | | Predict direct vs indirect leaks for three lost lists |
| 4 | `e04-tree-free` | `[WRITE]` | M | BST of owned keys: insert, find, recursive free |
| 5 | `e05-shared-tail` | `[REPORT]` | | Pinpoint a shared-node double free from an ASan report |
| 6 | `e06-list-from-words` | `[FAULT]` | | Build a list from an array, all-or-nothing, under every `FAIL_AT` |
