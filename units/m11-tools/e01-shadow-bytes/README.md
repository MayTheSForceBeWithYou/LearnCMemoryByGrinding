# m11-e01-shadow-bytes `[REPORT]`

## Goal

`capture.txt` is a real ASan report from this exact `main.c`. Using **only the report**
(and the source for line numbers), write `ANSWER.md` with:

1. The size of the block and how you can read it from the **shadow bytes alone**, without
   the "located 0 bytes after" line. Name the shadow byte you used and what its value means.
2. The offset of the faulting byte inside the block, and the expression in the source that wrote it.
3. The one-token change that fixes the allocation, and why the loop itself is correct.

Then fix `main.c` and confirm.

## Constraints

- Answer 1-2 before you run anything. Addresses in your own run will differ; the shadow pattern will not.
- The fix must not change the loop or the encoding. The `SIZE_MAX` guard must stay correct after your change.

## Verify

`make check` passes: checks, `make asan` with no report, and `make valgrind` clean.

<details><summary>Hint</summary>

The row marked `=>` starts at an address ending in `000`. Count granules from there: each
shadow byte is 8 application bytes, and the block starts after the `fa` redzone.
</details>
