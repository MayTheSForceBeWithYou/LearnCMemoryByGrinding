# M03 e02: Make it allocate too little `[BREAK]`

## Goal

`points.c` exposes

```c
struct pt { int32_t x, y, z; };          /* 12 bytes */
struct pt *points_load(uint32_t n, uint32_t seed);
```

It computes the byte size, rejects anything above `POINTS_MAX_BYTES` (a format limit in
`points.h`), allocates, and fills all `n` points. `main.c` reads `n` from `argv[1]`. For
normal `n` it works, and `make check` passes for the shipped small `n`. Counts that are too
big are rejected, or so the author believed.

Find a value of `n` that gets **past the size limit with an allocation far too small**, so
that the fill loop overflows the block. Prove it with `make asan ARGS=<n>`.

## Deliverables

- `ANSWER.md`: your `n`, the exact byte count the code requested (show the arithmetic: which
  type it was done in and where it wrapped), why the limit check didn't stop it, and the
  `heap-buffer-overflow` location line you observed (block size, and how many bytes after it).
- A fix in `points.c` so that every `n` either allocates enough or returns NULL. `main` already
  turns NULL into `exit(EXIT_OOM)`.

## Constraints

- Your `n` must make the allocation *succeed*. A count that's rejected or makes `malloc` fail
  isn't a break.
- Keep `POINTS_MAX_BYTES` and its meaning. The defect is in how the limit is checked, not in the limit itself.

## How to verify

- Before the fix: `make asan ARGS=<your n>` reports a heap buffer overflow.
- After the fix: the same command prints `m03-e02-break-count: all checks passed` with no report. `main`
  checks that any `n` whose byte count exceeds 32 bits is refused.
- `make check ARGS=<your n>` is clean.

<details><summary>Hint</summary>

Find the smallest `n` with `n * 12 >= 2^32`, then work out what remains after the wrap.

</details>
