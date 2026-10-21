# c2-gridload (capstone, L)

## Goal

Load a variable-size text map from a file into a heap grid, with **all-or-nothing** error
handling: the caller gets a complete, valid grid, or an error code and nothing allocated.
This is the RPG map loader from `LearnCwithRPG-terminal` ch10/ch11 done properly: no size
limits baked in, every failure path tested, and the ownership contract written down.

Expected size: 150-300 lines of your code across `gridload.c` and the header comments.

> **This starter does not build yet, on purpose.** `gridload.h` declares the API, and the harness
> and the CLI in `main.c` call it. `gridload.c` holds only its includes. Until you define the
> functions there, the link step fails with `undefined reference`. There are no placeholder stubs.

Files: `gridload.h` (API, and the contract you write), `gridload.c` (your code), `main.c` (harness
and CLI, provided), and `maps/` (the test maps, provided).

## The map format

- A map is one or more **rows**, one per line. A line ends at `\n`, or at `\r\n` (the `\r` is
  stripped). The last line may lack its line ending. A `\r` anywhere else is an illegal tile,
  including a `\r` that is the last byte of the file (`"#@#\r"` is `GRID_ERR_TILE`).
- A line ending *terminates* a row; it doesn't start one. So `"###\n"` is one row, and
  `"###\n\n"` is that row followed by a **blank row**.
- Every row has the same width, at least 1. A row of a different width, or a blank row anywhere (middle
  or end), is `GRID_ERR_RAGGED`.
- A file with no tiles at all (zero bytes, or only line endings such as a lone `\r\n`) is `GRID_ERR_EMPTY`.
- Legal tiles: `#` wall, `.` floor, `~` water, `+` door, `@` player spawn.
  Anything else, including spaces and tabs, is `GRID_ERR_TILE`.
- Exactly one `@`, or `GRID_ERR_SPAWN`.
- **Error precedence.** `GRID_ERR_EMPTY` comes first: a file with no tiles is EMPTY, never RAGGED,
  even if it holds several line endings (`"\n\n"` is EMPTY). For any other map that breaks more than
  one rule, you may report any of the broken rules' errors; stopping at the first problem you find is
  fine. Every bad map in `maps/` breaks exactly one rule.
- Width and height are each at most `GRID_MAX_DIM` (4096, in the header). Anything larger is an
  error, detected **without** allocating a buffer proportional to the oversized input.
- Row length is otherwise not known in advance. There is no "max line length" in your code.

Sample maps, good and bad, ship in `maps/`. The harness uses them, and you can add your own.

## The API

`gridload.h` declares this. **The ownership contract comments are yours to write.**

```c
typedef struct Grid Grid;            /* opaque */

typedef enum {
    GRID_OK = 0,
    GRID_ERR_IO,        /* could not open or read the file */
    GRID_ERR_EMPTY,     /* no tiles at all */
    GRID_ERR_RAGGED,    /* rows of different width, or a blank row */
    GRID_ERR_TILE,      /* illegal character */
    GRID_ERR_SPAWN,     /* zero or more than one '@' */
    GRID_ERR_TOO_BIG,   /* width or height over GRID_MAX_DIM */
    GRID_ERR_OOM        /* allocation failed */
} GridErr;

GridErr     grid_load(const char *path, Grid **out);
GridErr     grid_load_stream(FILE *f, Grid **out);
void        grid_destroy(Grid *g);
size_t      grid_width(const Grid *g);
size_t      grid_height(const Grid *g);
char        grid_at(const Grid *g, size_t x, size_t y);     /* '\0' if out of range */
void        grid_spawn(const Grid *g, size_t *x, size_t *y);
const char *grid_strerror(GridErr e);                       /* static string, never NULL */
```

`path`, `f` and `out` must not be NULL. The accessors (`grid_width`, `grid_height`, `grid_at`,
`grid_spawn`) require a live grid, not NULL, and `grid_spawn` requires non-NULL `x` and `y`. Only
`grid_destroy` accepts NULL. State all of this in your contract.

For errors other than `GRID_ERR_IO` and `GRID_ERR_OOM`, you may report the 1-based line number via an
optional out-parameter of your design. If you add one, document it in the header. The harness
does not require it.

## Constraints

- **All-or-nothing.** On any error, `*out` is set to NULL and nothing the call allocated is still allocated.
  On success, `*out` owns everything, and `grid_destroy` releases it. `grid_destroy(NULL)` is a no-op.
- `grid_load` closes the file on every path.
- The final grid stores its cells in **one** contiguous block (M08 e04 shape). How you hold rows while
  reading is up to you.
- **Fault-sweep visibility:** read with `fgetc`, `fgets` or `fread` into buffers you allocate yourself.
  **Do not use `getline`, `getdelim` or `asprintf`.** Their allocations happen inside libc, the fault
  wrapper cannot see them, and the sweep would miss those failure points. (`fopen`'s internal buffer
  is also invisible. That's accepted.)
- Grow buffers geometrically with the M04/M09 `tmp` pattern. A failed growth is `GRID_ERR_OOM`,
  never a leak.
- No global or static mutable state.

## The CLI

Run with no arguments, the program is the harness. Given a path (`./build/plain/prog maps/good-wide.txt`),
it loads it and prints the dimensions and spawn, or the error. Exit codes:

| Code | Meaning |
|---|---|
| 0 | loaded |
| 1 | harness check failed (test mode only) |
| 2 | bad input or usage: any `GridErr` except OOM |
| 3 | `EXIT_OOM`: one line to stderr, nothing leaked |

## Verify

```bash
make run        # harness over maps/: all checks pass
make asan
make valgrind
make fault      # FA_COUNT, then FAIL_AT=1..N under ASan, over the whole harness
make check      # all of the above
```

Done means: `c2-gridload: all checks passed`; ASan, LeakSanitizer and Valgrind silent; and every
`FAIL_AT` run either passes or exits 3 with no leak and no report.

<details><summary>Hint</summary>

Decide the error-path shape before you write the happy path: one `goto` target that frees
"whatever is non-NULL so far" and closes the file. Every check then becomes `err = ...; goto fail;`,
and the fault sweep has only one cleanup path to exercise.

</details>
