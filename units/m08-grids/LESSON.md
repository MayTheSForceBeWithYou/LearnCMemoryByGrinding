# M08: Arrays of pointers and 2D grids

## Organizer

So far every heap object you have built has been a single block, or a struct holding a
few blocks. This unit is about **tables**: many allocations hanging off one pointer.
Split a line into words and you get one, and so does loading a map. The new problems are
**counting**: how many blocks exist, which ones exist *right now* if
construction failed halfway, and in what order they must die.

You should be comfortable with `malloc`/`calloc`/`free`, `sizeof *p`, the `n * size`
overflow check (M03), and `goto cleanup` (M06). Heap strings from M07 show up in e01 and e06.

By the end you can build and free three shapes of 2D data and choose between them:

| Shape | Allocations | Rows can differ in length | `g[y][x]` syntax |
|---|---|---|---|
| Jagged: table of row pointers | 1 + h | yes | yes |
| Flat: one `w*h` block, index math | 1 | no | no, `g[y*w + x]` |
| One block: table and cells together | 1 | no | yes |

## Foundations

### What `char **` actually is

`char **words` is a pointer to the first element of an array whose elements are `char *`.
Each of those elements is a pointer to the first byte of a separate string, somewhere else.

```
words ──► [ ptr0 | ptr1 | ptr2 | NULL ]      one block: 4 * sizeof(char *)
            │      │      └──► "dog\0"       three more blocks
            │      └──► "the\0"
            └──► "see\0"
```

`sizeof *words` is `sizeof(char *)`, the size of a slot in the table, not the size of a string.
The table and the strings are **separate allocations with separate lifetimes**. `free(words)`
releases the four slots and nothing they point to.

### Ownership in a table

The usual contract is: *the table owns its rows.* Then destruction has exactly one correct
order: free every row, then the table. Free the table first and the row pointers you still
need are gone. Reading `words[i]` after `free(words)` is a use-after-free.

Two ways to know how many rows there are:

- **A count** (`size_t n`) stored next to the table or returned through an out-parameter.
- **A sentinel**: one extra slot holding `NULL`, as `argv` does. Costs one slot, makes
  "free everything" a loop with no count. Every allocation must reserve `n + 1` slots.

Pick one per API and document it in the header.

### Partially built tables

Construction can fail after some rows exist. At that moment the table holds `k` valid
pointers and `n - k` slots that are whatever `malloc` left there. Your cleanup must free
exactly the valid ones. Two disciplines work:

- free indices `k-1` down to `0` (you know `k` because it is your loop counter), or
- allocate the table with `calloc` so unfilled slots start as null pointers, and
  `free(NULL)` is a no-op. (All-bits-zero is a null pointer on every platform this course
  targets, though ISO C does not promise it.)

### Flat grids

A `w × h` grid of cells can be one block of `w * h` elements, row-major:
cell `(x, y)` lives at index `y * w + x`. One allocation, one `free`, good cache behaviour,
and no partial-failure problem. This is what the RPG `Map` in `LearnCwithRPG-terminal`
ch10/ch11 does: `char *tiles` sized `width * height`.

The costs: `w * h` can overflow `size_t` (reject `h == 0`, then check `w > SIZE_MAX / h` before multiplying, or
let `calloc`/`reallocarray` do it; they return NULL with `ENOMEM` on plain glibc, but under ASan that holds only with
`allocator_may_return_null=1`, which the course's `ASAN_OPTIONS` set; with ASan's defaults the oversized request
aborts with a report instead), and every access needs the index math, which is
exactly where `x`/`y` and `w`/`h` get swapped. Bounds checks belong in one accessor, not
scattered across callers.

### One block holding the table and the cells

You can have both `g[y][x]` and a single `free`: allocate the row-pointer table and the
cells in one block, cells after the table, then point each row slot into the cell area.

```
block ──► [ row0 | row1 | row2 | pad | c c c c | c c c c | c c c c ]
             │      │      └───────────────────────────────▲
             │      └──────────────────────▲
             └─────────────▲
```

Two details decide whether this is correct:

1. **Size arithmetic.** Total = table + padding + cells, and both products and the sum can
   overflow. Check each step.
2. **Alignment.** The cells start at `table` bytes into a block that `malloc` aligned for
   any type. If `table` is not a multiple of `_Alignof(cell_type)`, the first cell is
   misaligned. That is undefined behaviour, and UBSan reports it. Round the offset up:

   ```c
   size_t off = table_bytes;
   size_t a   = _Alignof(cell_type);
   off = (off + a - 1) / a * a;        /* next multiple of a */
   ```

   The padding is zero only when the table size is already a multiple of the alignment. On x86-64,
   `h` 8-byte pointers are always a multiple of 8, so `double` cells never need padding, and a missing
   round-up hides. `long double` needs 16, so any odd `h` leaves the cells 8 bytes off. e05 uses
   `long double` for that reason. M14 meets the same rule in an arena.

## Worked example: a triangle of rows

A neighbouring problem to the exercises: row `i` holds `i + 1` ints.

```c
#define _DEFAULT_SOURCE            /* reallocarray() under -std=c17; before any #include */
#include <stdlib.h>

/* Returns a table of n rows; row i has i+1 zeroed ints.
   Caller owns the result and frees it with free_triangle(rows, n).
   Returns NULL on allocation failure or n == 0. */
int **make_triangle(size_t n)
{
    if (n == 0)
        return NULL;
    int **rows = reallocarray(NULL, n, sizeof *rows);  /* malloc with the M03 overflow check; not zeroed */
    if (!rows)
        return NULL;
    for (size_t i = 0; i < n; i++) {
        rows[i] = calloc(i + 1, sizeof *rows[i]);
        if (!rows[i]) {
            while (i-- > 0)          /* rows[0..i-1] exist; rows[i] is NULL */
                free(rows[i]);
            free(rows);
            return NULL;
        }
    }
    return rows;
}

void free_triangle(int **rows, size_t n)
{
    if (!rows)
        return;
    for (size_t i = 0; i < n; i++)
        free(rows[i]);
    free(rows);
}
```

**The plausible wrong reading.** "The failure path is just `free_triangle(rows, n)`. It
frees every row and then the table, which is the right order, and `free(NULL)` is safe."
The order is right, but the slots are not all NULL. `rows` came from `reallocarray`, which, like `malloc`,
does not zero, so slots `i+1 .. n-1` hold garbage, and `free(garbage)` is undefined behaviour. What you
see depends on the garbage. A fresh block is often zeroed by accident, so the plain build may run clean,
because every bad `free` was `free(NULL)`. ASan fills new blocks with a non-zero pattern, so under ASan the same
`free` gets a wild pointer and fails loudly. Either way, no tool says "you freed a slot you never filled".
You get there from the stack, which points at the cleanup loop. The reading becomes true if the table
comes from `calloc`. Then reusing `free_triangle` on the failure path is a legitimate design,
and arguably a cleaner one, because there is only one teardown routine to get right.

A second wrong reading: "`n == 0` returning NULL is fine." It is fine only if the header says so,
because the caller can no longer tell "empty" from "out of memory". Decide, then document.

## `man` pointers

- `man 3 malloc`: `calloc`, `reallocarray` (overflow-checked `n * size`), RETURN VALUE.
- `man 3 free`: what `free(NULL)` does.
- `man 3 strndup`: copying a word out of the middle of a line (e01).
- `man 3 isspace`: what counts as whitespace, and why the argument must be cast to `unsigned char`.
- `gcc` manual, `-fsanitize=alignment` (part of `undefined`): what UBSan checks for e05.
- `gcc` manual, `-Wuse-after-free`: gcc 12+ catches some freed-pointer reads at compile time (e02).

## Check yourself

1. In `char **t` pointing to a 5-slot table, what is `sizeof *t`, and what does it have to do with string lengths?
2. A jagged table of 10 rows is lost (its only pointer goes out of scope). How many blocks
   leak, and which one does LeakSanitizer call *direct*?
3. Why does the flat grid have no partial-failure path?
4. On x86-64, a one-block grid has 3 row pointers. Is padding needed before `double` cells
   (alignment 8)? Before `long double` cells (alignment 16)?

<details><summary>Answers</summary>

1. `sizeof(char *)`, the slot size. It says nothing about the strings.
2. 11 blocks. The table is directly lost (nothing points to it). The 10 rows are only reachable
   through the lost table, so they are *indirect*. Fix the direct one and the indirect ones go with it.
3. There is only one allocation. It either happened or it didn't.
4. The table is 24 bytes. That's a multiple of 8, so `double` needs no padding. It isn't a multiple
   of 16, so `long double` cells start at 32, after 8 bytes of padding. Get alignments from `_Alignof`,
   not from memory.

</details>

## Takeaways

- A table of pointers is 1 + n allocations, and the table owns the rows unless the header says otherwise.
- Destroy rows first, then the table.
- A half-built table has valid and garbage slots. Free only what you allocated: count down, or `calloc` the table.
- Flat grids trade `[y][x]` syntax for one allocation and no partial failure. Guard `w * h`.
- One-block grids need correct size arithmetic **and** alignment of the cell area.

## Exercises

| # | Folder | Tag | Size | What |
|---|---|---|---|---|
| 1 | `e01-split-words` | `[WRITE]` | S | Split a line into an owned, NULL-terminated `char **` |
| 2 | `e02-free-order` | `[BUG]` | | A grid teardown that compiles, runs, and is wrong |
| 3 | `e03-lost-table` | `[PREDICT]` | | Predict LeakSanitizer's direct/indirect split for lost tables |
| 4 | `e04-flat-grid` | `[WRITE]` | M | A `w*h` grid with overflow-checked creation and one bounds-checked accessor |
| 5 | `e05-one-block` | `[WRITE]` | M | `long double` table and cells in one allocation, `g[y][x]`, correct alignment |
| 6 | `e06-copy-rows` | `[FAULT]` | S | Deep-copy a string table, all-or-nothing, under every `FAIL_AT` |
