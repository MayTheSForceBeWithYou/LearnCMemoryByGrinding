# c2-gridload: notes

**Shape.** One pass with `fgetc`. Tiles go straight into a single growing buffer (×2 from 256, via
`realloc` into `tmp`), which becomes the grid's cell block, so there is no second copy. Rows are never stored
separately: the first row fixes `w`, and every later row is checked against it as it's read. A row
longer than `w` is rejected as soon as it passes `w` characters. Either dimension is rejected as soon as it
passes `GRID_MAX_DIM`, so the buffer never grows past `GRID_MAX_DIM * GRID_MAX_DIM` plus one row, however
large the input.

**All-or-nothing.** Every error goes to one `fail:` label that frees the buffer. `*out` is set to NULL first
and assigned only at the very end, after the `Grid` struct allocation succeeded. `grid_load` closes the file
on every path, and treats a failed `fclose` after a successful load as `GRID_ERR_IO` (destroying the grid).

**Rule order.** Errors are reported at the first point they're certain: TILE/SPAWN(second `@`)/TOO_BIG/RAGGED
while reading (a `\r` not followed by `\n`, including one right before EOF, is TILE), and EMPTY, blank-row
RAGGED and missing-`@` SPAWN at end of file, in that order, so a file with no tiles is always EMPTY. The README
allows any broken rule for other multi-rule maps.

**Sweep.** `verify-all`: `sweep N=47 pass=0 oom=47 fail=0`. Every allocation in the harness, across all the
good and bad maps, fails once and exits 3 cleanly. `fopen`'s own buffer isn't counted (the `--wrap`
limitation); that's accepted.

**The usual mistakes.**
- Using `getline`: it works, but the sweep can't see it.
- Building rows as separate allocations and then forgetting the half-built rows on a late error.
- Treating `\n\n` at end of file as harmless: the spec makes it a blank row.
- Returning `GRID_ERR_OOM` for an oversized map because the buffer grew first: check the dimension *before* growing.
