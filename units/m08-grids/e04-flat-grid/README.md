# m08-e04-flat-grid `[WRITE]` (M)

## Goal

Implement a flat, row-major grid of `char` cells: one block of `w * h` bytes, accessed by index math.
`grid.h` declares the API and its ownership contract. Read it first.

```c
Grid *grid_create(size_t w, size_t h, char fill);
void  grid_destroy(Grid *g);
bool  grid_get(const Grid *g, size_t x, size_t y, char *out);
bool  grid_set(Grid *g, size_t x, size_t y, char c);
```

> **This starter does not build yet, on purpose.** It declares the functions you must write,
> and the harness calls them. Until you define them, the link step fails with
> `undefined reference`. There are no placeholder stubs.
> Write your definitions in `main.c`, below the marked line.

## Constraints

- Exactly two allocations per grid: the `Grid` struct and the cell block. No row-pointer table.
- `grid_create` returns NULL if `w` or `h` is 0, if `w * h` would overflow `size_t`, or on
  allocation failure. It leaks nothing in any of those cases.
- `grid_get`/`grid_set` return false for out-of-range coordinates and never touch memory outside the cells.
  `grid_get` leaves `*out` untouched on false.
- `grid_destroy(NULL)` is a no-op.
- The harness includes a size that overflows. A correct implementation rejects it without
  calling `malloc` with a wrapped-around size.

## Verify

```bash
make check
```

The harness prints `m08-e04-flat-grid: all checks passed`; ASan and Valgrind are silent.

<details><summary>Hint</summary>

Put the index computation in one place. If both accessors compute `y * w + x` independently,
one of them will eventually say `x * h + y`.

</details>
