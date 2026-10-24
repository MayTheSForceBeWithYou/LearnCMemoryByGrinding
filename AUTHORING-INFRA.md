# Authoring infrastructure

How exercises plug into `common.mk`, `lib/` and `scripts/`. Rules for content are in PLAN.md section 7.

## Exercise Makefile template

Every exercise dir and every solution dir has the same Makefile, with only the values changed:

```make
TAG = FAULT
SRCS = main.c
EXPECT_SYMBOLS = record_create record_destroy
include ../../../common.mk
```

- The include is relative: `../../../` from `units/mNN/eNN/` and `solutions/mNN/eNN/`,
  `../../` from `capstones/cN/`, and `../../../` from `solutions/capstones/cN/`.
  `common.mk` finds the course root from its own path, so `lib/` is found at any depth.
- `SRCS` defaults to `main.c`. List every `.c` file in a multi-file exercise. Local `.h` files are picked up automatically.
- Optional: `ARGS = input.txt` (program arguments), `EXTRA_CFLAGS = ...`, `LDLIBS = -lm`.
- Flags are strict `-std=c17`, so `strdup`, `strndup`, `getline` and `reallocarray` are **not declared**
  unless the file starts with `#define _POSIX_C_SOURCE 200809L` (strdup/strndup/getline) or
  `#define _DEFAULT_SOURCE` (adds reallocarray), placed before any `#include`.
  Keep those defines in the sources: they are the Linux path. On macOS,
  `common.mk` adds `-D_DARWIN_C_SOURCE` for the same effect, so the same sources
  compile on both. `reallocarray` needs macOS 15+ (see `docs/macos.md`).

| Command | What it does |
|---|---|
| `make` / `make run` | build `build/plain/prog` and run it |
| `make asan` | `build/asan/` (ASan+UBSan), run with `ASAN_OPTIONS=detect_leaks=1:abort_on_error=1:halt_on_error=1:allocator_may_return_null=1` (+ `ASAN_EXTRA`) |
| `make valgrind` | memcheck on the **plain** build (never on an ASan build). Linux only; on macOS it explains itself and exits 1 |
| `make leaks` | macOS only: the `leaks` tool on the plain build; the leak-check step of `make check` on macOS |
| `make fault` | `build/fault/` (ASan + `lib/failalloc.c` + `--wrap` on Linux, compile-time renaming on macOS), run the FAIL_AT sweep |
| `make check` | build all modes, then checks, asan, valgrind (Linux) or `leaks` (macOS), and the sweep if TAG has FAULT |
| `make clean` | remove `build/` |
| `make capture TOOL=asan` | REPORT only: write `capture.txt` (see below) |

ASan reports abort the program (SIGABRT, `make: *** ... Error 134`). That is a failure, as intended.

The same `ASAN_OPTIONS` are used by `make asan`, the asan stage of `make check`, `make fault` and the
FAULT sweep. Two parts of them matter to authors:

- `allocator_may_return_null=1`: an impossible request (e.g. `malloc(SIZE_MAX)`, or a size that
  overflowed) returns NULL, like glibc, instead of ASan aborting with `requested allocation
  size ... exceeds maximum supported size`. ASan still prints one line,
  `WARNING: AddressSanitizer failed to allocate 0x... bytes`, on stderr, and the run continues. Overflow-check exercises (M03)
  rely on this. Real out-of-bounds, use-after-free and leak reports still abort.
- `ASAN_EXTRA = opt=val[:opt=val]` in an exercise Makefile appends options for that exercise only,
  e.g. `ASAN_EXTRA = detect_stack_use_after_return=1`.

Valgrind is unaffected: its allocator already returns NULL for impossible sizes (it prints
`Argument 'size' of function malloc has a fishy (possibly negative) value: -1` plus a stack).

## TAG and EXPECT_SYMBOLS

`TAG` is one of `WRITE BUG REPORT PREDICT TRACE FAULT DESIGN`. Add `NO-ASAN` (e.g. `TAG = BUG NO-ASAN`)
for the deliberate no-sanitizer demos. It skips the ASan and fault steps in `make check`.

`EXPECT_SYMBOLS` is required for `WRITE`, `FAULT` and `DESIGN`. It lists the functions the student must
define. The starter declares them and the harness calls them, but they are not defined, so `make` must fail with
`undefined reference to 'one_of_them'`. A compile error in a starter (bad include, syntax) is a bug in the
exercise, and verify-all reports it as one.

The harness is `lib/course.h`: `check(cond, "msg")` prints `FAIL file:line: msg` and counts the failure.
`return checks_done("m06-e01");` prints `m06-e01: all checks passed` and returns 0, or returns 1 on failure.
On allocation failure, free everything you own, print one line to stderr, and `return EXIT_OOM;` (3).

## FAULT exercises and failalloc

There is no special API. Students call plain `malloc`, `calloc`, `realloc`, `reallocarray`, `strdup`,
`strndup` and `free`. In the fault build these go through `lib/failalloc.c`:

- `FAIL_AT=N ./build/fault/prog` makes only the Nth allocation call return NULL (errno ENOMEM).
  A failed realloc leaves the old block alive.
- `FA_COUNT=1 ./build/fault/prog` prints `failalloc: N allocations` to stderr at exit.
- `make fault` runs FAIL_AT=1..N. Every run must exit 0 or 3 with no sanitizer or leak report.
  The summary reads `fault-sweep: N=3 pass=0 oom=3 fail=0`.
- **Limitation:** only calls in the exercise's own `.c` files are seen. libc-internal allocations
  (`getline`, `asprintf`, stdio buffers) are not counted or failed. Keep exercises that depend on them out of FAULT.

## REPORT exercises: the capture convention

The pasted tool output lives in `capture.txt` in the exercise dir and must come from the exact shipped source:

```
make capture TOOL=asan        # or TOOL=valgrind, TOOL=run
```

The first line of the file is `# source-sha256: <hash>`, a hash of `SRCS` and local headers. verify-all recomputes it
and flags the exercise **STALE** if the source changed after the capture. Re-run `make capture` and update
any README excerpt taken from the file. Leave the header lines in place. They also tell the student that addresses will differ.

## verify-all

```
scripts/verify-all.sh                # whole course
scripts/verify-all.sh /path/to/tree  # or LCMG_ROOT=/path/to/tree scripts/verify-all.sh
scripts/verify-all.sh --only units/m12-zoo --only capstones/c3-lru   # just these (repeatable)
```

`--only PATH` is a prefix match on the path relative to the course root. The mirror in `solutions/`
is included automatically.

It walks `units/*/e*/` and `capstones/*/` plus their mirrors in `solutions/`:

- solutions: `make check` passes (sweep counts are shown);
- WRITE/FAULT/DESIGN starters: the build fails with an undefined reference to an EXPECT_SYMBOLS name;
- BUG starters: they build, then `make check` fails;
- REPORT starters: they build, and capture.txt is not stale;
- PREDICT/TRACE/others: they build;
- `units/*/lesson-demo/` (optional, programs quoted by LESSON.md): it builds, and every
  `capture*.txt` in it matches the current source hash.

Any `capture*.txt` in a **solution** dir (e.g. a PREDICT answer or a gdb TRACE transcript) must match
the solution's own source hash. If that solution dir has no Makefile, it must match the starter's.
Custom capture targets are fine: `make capture TOOL=<your-target>`, then rename the file if you keep
several (`capture-asan.txt`). The hash covers `SRCS` and `*.h` only, **not** `.gdb` scripts or other
inputs, so re-capture by hand when you change those.

It prints one table row per item, cleans every `build/`, and exits non-zero on any failure. Run it before you mark a unit done.

## Valgrind on Arch

If Valgrind dies with `Fatal error at startup: a function redirection ... cannot be set up`, it needs
debuginfod for the stripped `ld.so`: `DEBUGINFOD_URLS=https://debuginfod.archlinux.org`. `common.mk`
exports that value when it is not already set.
