# M02b e02 notes

`capture.txt` has two records:

| record | kind | size | site | block | why unreachable |
|---|---|---|---|---|---|
| 1 | direct | 64 bytes, 1 object | `roster_new` (`r->players = malloc(...)`) | the players array, 4 x 16-byte `struct player` | its only pointer lived in `*r`, which was freed |
| 2 | indirect | 18 bytes, 3 objects | `roster_add` (`malloc(len + 1)`) | the three name copies (4 + 7 + 7 bytes, NUL included) | pointed to only from the leaked array |

- `free(r)` frees the struct, so the struct itself isn't leaked. But it was the only holder of `r->players`.
- The smallest fix is two additions *before* `free(r)`: free each name, then `free(r->players)`. Adding only `free(r->players)`
  turns the names' record from indirect into **direct** (3 objects): their holder is gone, and nothing leaked points at them
  any more. That's the most instructive prediction in this exercise.
- Order matters for *correctness*, not just for leaks: freeing `r` first and then reading `r->players` would be a
  use-after-free. Free innermost first.
- The solution also uses `roster_free` on the OOM path inside the add loop, which the starter leaked on.
