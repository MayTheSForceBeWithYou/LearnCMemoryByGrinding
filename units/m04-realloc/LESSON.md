# M04: `realloc` done right

## Advance organizer

`realloc(p, n)` is the only standard way to resize a heap block, and its contract has
three outcomes that look identical at the call site. Almost every `realloc` bug comes from
handling only one of them:

| Outcome | Return value | The old block `p` | Pointers into the old block |
|---|---|---|---|
| resized in place | same address as `p` | lifetime ended in C17 terms, but the result is at the same address | numerically equal; use only values derived from the result |
| moved | a new address | **freed** by `realloc` (ASan always takes this row) | **all dangling** |
| failed | NULL | **still alive, unchanged**, and still yours to free | still valid |

You can't predict which of the first two happens, and you must handle the third. This unit
covers the `tmp` pattern, `realloc(NULL, n)`, shrinking, why the course never uses
`realloc(p, 0)`, interior pointers, and geometric growth. It's also your first `[FAULT]`
unit: `make fault` makes allocations fail on purpose.

## Foundations

### The `tmp` pattern

```c
/* WRONG */
buf = realloc(buf, newcap);           /* on failure: buf = NULL, old block leaked */

/* RIGHT */
char *tmp = realloc(buf, newcap);
if (tmp == NULL) {
    /* buf is still valid and still holds the old contents and capacity */
    return -1;                         /* caller still owns buf, or frees it */
}
buf = tmp;
cap = newcap;
```

On failure, the old block is still alive, and the only copy of its address was in `buf`.
Assigning NULL to `buf` destroys that copy, so the block can never be freed. That's a leak,
created on the very path where memory is already short. Update `cap` only after success.
Otherwise a failed grow leaves `cap` claiming bytes you don't have, and the next write
overflows.

### `realloc(NULL, n)` is `malloc(n)`

This lets a growable buffer start empty (`buf = NULL, len = 0, cap = 0`) with no
special-case first allocation: the first grow passes NULL. `free(NULL)` being a no-op makes
destruction of a never-grown buffer free too.

### Shrinking

`realloc(p, smaller)` may move the block, or return the same address. Either way, the bytes
beyond the new size are no longer yours. It can even fail, in theory. Plain glibc usually shrinks
heap chunks in place. That doesn't hold under ASan (its `realloc` always moves) or for large
mmapped chunks (which glibc may remap or copy), so code must still use the `tmp` pattern. A failed shrink is harmless: keep the old,
bigger block.

### Never `realloc(p, 0)`

C17 says the result of `realloc(p, 0)` is implementation-defined. It may free `p` and return
NULL, or return a new minimum-size block. glibc frees and returns NULL, other libcs return a
block. With the `tmp` pattern, a NULL here is indistinguishable from failure, so you'd either
leak or double free depending on the platform. C23 makes it outright undefined behaviour.
**Course rule:** to empty a buffer, `free` it and set it to NULL, or keep the block and set
`len = 0`. Never call `realloc` with size 0. Guard computed sizes so they can't reach 0.

### Growth policy: geometric, not additive

Growing by a constant (`cap + 16`) makes `n` appends cost O(n^2) byte copies in the worst
case, because every grow may move the whole block. Growing by a factor (`cap * 2`, or
`cap + cap / 2`) makes the total copying O(n), "amortised O(1) per append". The new
capacity computation itself can overflow, so check it:

```c
size_t newcap = cap ? cap : 16;
while (newcap < need) {
    if (newcap > SIZE_MAX / 2) return -1;   /* doubling would wrap */
    newcap *= 2;
}
```

### The grown tail is uninitialised

Bytes from `[oldsize, newsize)`, in C terms the byte offsets `i` with `i >= oldsize && i < newsize`,
have indeterminate values even if the block came from `calloc`. If you rely on zeroes, `memset`
that range yourself.

## Worked example 1: the interior pointer

```c
struct buf { char *data; size_t len, cap; };

char *word = b.data + 10;       /* points at byte 10 of block B1 */
buf_append(&b, more, 500);      /* grows: realloc may move B1 to B2 */
puts(word);                     /* ??? */
```

In memory: before the append, `b.data` holds B1's address and `word` holds B1 + 10.
If `realloc` moved the data, it allocated B2, copied `len` bytes from B1, **freed B1**,
and returned B2. `buf_append` stored B2 in `b.data`. Nobody updated `word`. It still holds
B1 + 10, which now points into a freed block.

**Rejected reading:** "`word` is fine because the data was copied to B2." The *bytes* were
copied. The *pointer* wasn't, because `realloc` can't know `word` exists. Store **offsets**
(`size_t word_off = 10;`) across any operation that may grow, and recompute
`b.data + word_off` after it. This also bites anything that keeps `&v[i]` across a push
(M09) and any struct with a pointer into its own buffer.

## Worked example 2: the failure path, under the fault injector

```c
int buf_reserve(struct buf *b, size_t need) {
    if (need <= b->cap) return 0;
    size_t newcap = /* geometric, overflow-checked */;
    char *tmp = realloc(b->data, newcap);
    if (tmp == NULL) return -1;
    b->data = tmp;
    b->cap = newcap;
    return 0;
}
```

`make fault` first runs your program with `FA_COUNT=1`, which reports how many allocation
calls it made (say N). Then it runs again N times, with `FAIL_AT=1`, then `FAIL_AT=2`, through
`FAIL_AT=N`. In run `k`, the `k`th allocation call returns NULL. Each run must either pass
its checks or exit with `EXIT_OOM` (3), with no ASan report and no leak. When the failing call
is a `realloc`, the injector **leaves the old block alive**, exactly as the real `realloc` does.

**Rejected reading:** "`buf_reserve` returned -1, so `main` can just `exit(EXIT_OOM)` and
the OS reclaims everything." LeakSanitizer runs at exit and reports `b->data` as leaked
unless it is still reachable. More importantly, in a long-lived program the caller would carry
on. The failure path must leave `*b` consistent (old data, old `cap`) so the caller can
`buf_free(b)` and exit cleanly. That's what the sweep tests.

## man pointers

- `man 3 realloc`: RETURN VALUE (what happens to the original block on failure), and the
  NOTES or BUGS remarks on size 0.
- `man 3 reallocarray`: the checked variant from M03. Same three-outcome contract.

## Check yourself

1. After `q = realloc(p, n)` succeeds with `q != p`, what's the state of the block `p`
   pointed to, and of every pointer into it?
2. Why does `p = realloc(p, n)` leak, and on which path only?
3. A grow fails after `cap` was already updated. What goes wrong on the next append?
4. Why does this course forbid `realloc(p, 0)` even though glibc's behaviour is documented?
5. Write the C condition for "byte offset `i` lies in the newly grown, uninitialised tail".
6. Why does doubling give amortised O(1) appends while `+16` does not?

<details><summary>Answers</summary>

1. The old block's lifetime has ended. Every pointer into it, including `p`, is dangling
   and must not be used.
2. Only on the failure path: `realloc` returns NULL, the assignment overwrites `p`, and the
   still-valid old block is no longer reachable.
3. `cap` claims more room than the block has, so the next append skips growing and writes
   past the end of the old, smaller block.
4. Its meaning has varied between standards and implementations (C23 makes it undefined),
   and code that relies on it is ambiguous to the reader. Use `free`.
5. `i >= old_size && i < new_size`.
6. Doubling makes the total bytes copied over `n` appends at most about `2n`, a constant
   per append. With `+16`, every 16 appends copy the whole buffer, so the total is `O(n^2)`.

</details>

## Takeaways

- Three outcomes: in place, moved (old block freed, interior pointers dangling), failed
  (old block intact and still yours).
- Always assign to `tmp`, check, then commit both pointer and capacity.
- `realloc(NULL, n)` is `malloc`. Never pass size 0.
- Keep offsets, not pointers, across anything that may reallocate.
- Grow geometrically, and check the capacity computation for overflow.
- `make fault` fails every allocation in turn. Each failure must leave state you can clean up.

## Exercises

| # | Folder | Tag | Size | Topic |
|---|---|---|---|---|
| e01 | `e01-append-grow/` | `[WRITE]` | S | `ints_push` on a growable `int` array using the `tmp` pattern, starting from NULL |
| e02 | `e02-lost-on-failure/` | `[BUG]` | S | `p = realloc(p, n)` and an early `cap` update; found by the fault sweep |
| e03 | `e03-trace-move/` | `[TRACE]` | S | Draw blocks and pointers before and after a growing `realloc`, both in-place and moved |
| e04 | `e04-predict-realloc/` | `[PREDICT]` | S | Predict the outcome of `realloc(NULL, n)`, shrink, grow, and the size-0 call on glibc |
| e05 | `e05-dangling-interior/` | `[BUG]` | S | A tokenizer keeps `char *` into a buffer that grows; switch to offsets |
| e06 | `e06-fault-growbuf/` | `[FAULT]` | M | A byte buffer with reserve/append/shrink-to-fit that survives every `FAIL_AT` |
