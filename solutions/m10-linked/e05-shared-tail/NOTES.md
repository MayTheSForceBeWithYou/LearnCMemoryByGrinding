# m10-e05-shared-tail: answer key

From `capture.txt` in the exercise:

1. **heap-use-after-free, READ of size 8**, at `main.c:17` (`Node *next = head->next;` in `list_destroy`),
   during the second destroy, `list_destroy(headliners)` at `main.c:83`.
2. **Freed by** `free(head)` at `main.c:19`, during the first destroy, `list_destroy(bill)` at `main.c:82`.
   Allocated by `list_from` (`main.c:28`) called from `main.c:69`, the `headliners` list.
3. The node is **"Ceres"**, the first node of `headliners`. `list_concat` linked `headliners` in as the tail
   of `bill` (`*link = b;`) instead of copying it, so both `bill` and `headliners` believed they owned it.
4. The first double free would have been `free(head->name)` on line 18 for "Ceres". Its name was freed by the
   first destroy. ASan halted on the read one line earlier.
Plain build: on the course machine, glibc aborted the program (exit 134). Don't count on that.

5. **Rule broken:** each node has exactly one owner. **Fix (this solution):** `list_concat` copies `b`'s nodes
   too, and the comment states that `a` and `b` are borrowed. (Alternative: make `list_concat` *take* `b`, and
   have `main` stop destroying `headliners`. That's also correct, but it's a different contract.)
