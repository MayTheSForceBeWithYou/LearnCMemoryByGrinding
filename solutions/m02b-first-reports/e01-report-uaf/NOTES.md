# M02b e01 notes

Reading `capture.txt` in the lesson's order:
1. **Class:** `heap-use-after-free`, a `READ of size 4`. Size 4 means an `int`.
2. **Access:** the first course frame is `main.c:25`, the `check(heaviest->weight <= total, ...)` line.
3. **Location line:** "16 bytes inside of 20-byte region". `struct item` is `char name[16]` + `int weight` = 20 bytes, and
   offset 16 is `weight`. The allocation stack is `inventory_add` (`malloc(sizeof *it)`) called from `main` for the *first*
   item, the sword.
4. **Freed by:** `inventory.c:20` in `inventory_drop` (`free(inv->slots[idx])`), called from `main.c:19`. Allocated at `inventory.c:9` from `main.c:8`.
5. `heaviest` was copied from `sword` before the drop. `inventory_drop` freed that block and removed its slot, so
   `heaviest` (and `sword`) now dangle. The fix re-derives `heaviest` from live items after the drop. In a larger program
   you'd store an index or a name, not a borrowed pointer, across an operation that may free.

The header comment on `inventory_add` already said "valid until dropped". The contract was there, and `main` broke it.
