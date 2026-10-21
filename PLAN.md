# Plan: Learn C Memory by Grinding

**Status:** v0.2, Bruce's review changes applied (see Changelog at the end). Awaiting his sign-off on the diff.
**Location:** `/home/n8/dev/c/LearnCMemoryByGrinding/` (Arch WSL, Linux filesystem)
**Student:** Nathan (n8)

## 1. Why this exists

Chapter 10 of `LearnCwithRPG-terminal` ("The Heap") introduces stack vs heap, `malloc`/`free`,
ownership via `create`/`destroy` pairs, leaks, use-after-free, gdb, partial-failure cleanup,
and sanitizer flags. Then the game moves on. It ships 4 practice drills and 4 chapter exercises.
That is not enough repetition for the skill that sits at the core of C.

`LearnCbyGrinding` explicitly lists heap management as an anti-goal, and `bridge-the-C-asm`
has no focused C-level lesson on it. So this is a **separate, narrow-and-deep course** on
dynamic memory: allocating responsibly, and finding and fixing memory bugs.

## 2. Goals

By the end, the student can, without looking anything up beyond `man`:

- Choose stack vs static vs heap for a given lifetime and explain why.
- Use `malloc`, `calloc`, `realloc`, `free` correctly, including every error path,
  the `sizeof *p` idiom, and `n * size` overflow.
- Write `realloc` growth code that never leaks on failure.
- Design and document ownership: who allocates, who frees, borrowing vs transfer,
  out-parameters (`T **out`), and create/destroy pairs.
- Write multi-allocation constructors with correct partial-failure cleanup
  (reverse-order frees, the `goto cleanup` pattern).
- Manage heap strings, arrays of pointers, 2D grids (jagged and contiguous),
  dynamic arrays, and linked structures without leaks or dangling pointers.
- Diagnose every common heap bug from a tool report: leak (direct/indirect),
  double free, use-after-free, heap buffer overflow, uninitialised read,
  invalid free, size mismatch. Tools: ASan, LeakSanitizer, UBSan,
  Valgrind memcheck, gdb (including watchpoints).
- Inject allocation failures on purpose to test error paths.

### Anti-goals

- **Not a project.** No game and no long-lived codebase. Every exercise stands alone,
  and a few medium-sized ones run 150-300 lines.
- **Not an allocator-implementation course.** Exercises cover arenas and pools for
  intuition, but not writing `malloc` itself.
- **Not C++.** No smart pointers or RAII discussion beyond a one-line contrast.

## 3. Environment (verified on the target machine)

- Arch Linux on WSL2. Work under `~/dev/c`, never `/mnt/c`.
- `gcc`, `clang`, `gdb`, `valgrind`, `make`, and `man` are all on PATH. On Arch, `/usr/sbin` is a symlink to `/usr/bin`.
- `rr` is not used, because it needs hardware performance counters that WSL2 doesn't expose.
- Standard flags: `-std=c17 -Wall -Wextra -Wpedantic -g`
- Sanitizer build: add `-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer`
  to both CFLAGS **and** LDFLAGS. Chapter 10 documents the link-line pitfall. Without
  `-fno-sanitize-recover=all`, UBSan prints a report and keeps going.
- `make asan` runs with `ASAN_OPTIONS=detect_leaks=1:abort_on_error=1:halt_on_error=1:allocator_may_return_null=1`, so any
  report gives a non-zero exit. Huge or failed allocations return NULL instead of aborting, so the
  overflow tests grade the code the student wrote.
- **ASan and Valgrind never run on the same binary.** Valgrind on an ASan build fails or
  reports garbage, so each build mode has its own directory (see 4.4).
- Valgrind on Arch: see `LearnCwithRPG-terminal/appendices/c-gdb-and-sanitizers.md`,
  section "On Arch: it refuses to start". The course must state the fix rather than
  assume it.

## 4. Structure

### 4.1 Units, lessons, exercises

The course has **15 units** (M01-M14 plus M02b) and **91 exercises**, plus 3 medium capstones. Each unit
is a folder with:

```
units/m05-ownership/
├── LESSON.md              # lesson plan: idea, teaching, worked example, man pointers, exercise list
├── e01-who-frees-this/
│   ├── README.md          # instructions: goal, constraints, how to verify. Clear, not spoon-fed.
│   ├── main.c             # starter file: self-checking test harness in main; [WRITE]/[FAULT]/[DESIGN] starters fail to build until the student writes the code (4.2)
│   └── (other .c/.h if multi-file)
├── e02-.../
└── ...
solutions/m05-ownership/
├── e01-who-frees-this/
│   ├── main.c             # complete, compilable reference solution
│   └── NOTES.md           # why this is correct, alternatives, the mistake most people make
└── ...
```

**Solutions live in a separate top-level `solutions/` tree**, never next to the exercise.
Unlike `LearnCbyGrinding`, solution files are **complete, compilable programs**.
Nathan asked for exactly that, as a last resort. The bar is "I REALLY need it,"
so `README.md` never links to the solution.

### 4.2 Exercise files

- **`[WRITE]`, `[FAULT]`, and `[DESIGN]` starters fail to build until the student writes the code.**
  This is a standing preference of Nathan's, decided for `LearnCwithRPG-terminal`. There are no stubs
  that return dummy values and no stub stop lines. The starter declares the prototypes (in `main.c`
  or a provided header), the harness calls them, and the student writes the definitions. Until then,
  the build fails at link time with `undefined reference`.
- **`[BUG]`, `[REPORT]`, `[PREDICT]`, and `[TRACE]` files are complete, buildable programs.**
  `[BUG]` starters build, then fail their own check or a sanitizer or Valgrind run.
- The test harness is the same tiny `check(cond, msg)` pattern the RPG course uses.
  On success it prints `<exercise-id>: all checks passed` and exits 0.
  On failure it exits 1.
- **Graceful allocation failure** exits with code **3** (`EXIT_OOM` in `lib/course.h`) after
  printing a one-line message to stderr. Every `[FAULT]` exercise and capstone must do this,
  and must not leak on that path.
- **Done means clean everywhere.** Checks pass, the ASan/UBSan build shows no reports,
  LeakSanitizer reports zero leaks, and the plain build under
  `valgrind --leak-check=full --errors-for-leak-kinds=definite,indirect,possible --error-exitcode=1` exits 0.
  Exercises marked `[NO-ASAN]` are the exception (see 4.4).
- **Each tool has blind spots, and the course grades with the right one.** ASan does not detect
  uninitialised reads, so those exercises are graded by Valgrind memcheck. MSan is out of scope
  because it is clang-only and needs an instrumented libc.
- READMEs state the *what* and the *constraints* (e.g. "must not leak if the 3rd
  allocation fails"), and leave the *how* to the student. Each one gets at most one hint,
  placed in a collapsed `<details>` block.

### 4.3 Exercise types (adapted from LearnCbyGrinding)

| Tag | Student... |
|---|---|
| `[WRITE]` | implements functions to make checks pass |
| `[BUG]` | gets defective code, finds and fixes the defect, and writes the mechanism in `ANSWER.md` |
| `[REPORT]` | gets a real ASan or Valgrind report plus source, and pinpoints the bug from the report alone before running anything |
| `[PREDICT]` | predicts the tool output (leak count, direct vs indirect, which line), then runs it |
| `[TRACE]` | draws stack and heap state on paper at marked points (template in README) |
| `[FAULT]` | uses the course's fault-injection `malloc` wrapper to force failures, and makes the code survive every one |
| `[DESIGN]` | writes the ownership contract (header comments) for an API, then implements it |
| `[EXPLAIN]` | writes 2-4 sentences of prose, no code |
| `[LOOKUP]` | answers only from `man`/`info`/`--help`, quoting the page |
| `[BREAK]` | finds an input that defeats working code |

`[EXPLAIN]`, `[LOOKUP]` and `[BREAK]` appear in lesson lists and README titles only. Their Makefiles
build as `TAG = PREDICT` (verify-all builds the starter, and the solution must pass `make check`),
because common.mk accepts only the seven tags above.

**Composition per unit:** at most 3 `[WRITE]`, at least 1 `[BUG]` or `[REPORT]`,
at least 1 `[PREDICT]` or `[TRACE]`. From unit M06 on, every unit also has at least 1 `[FAULT]`.

### 4.4 Shared infrastructure (Bruce builds this first)

- **`common.mk`** with three separate build directories:
  - `build/plain/`: standard flags. Used by `make run` and `make valgrind`.
  - `build/asan/`: ASan + UBSan flags as in section 3. Used by `make asan`.
  - `build/fault/`: ASan build plus the `failalloc` wrappers. Used by `make fault`.
  - `make check` builds all three and runs checks, ASan, Valgrind, and (where the exercise is
    tagged `[FAULT]`) the fault sweep. `make clean` removes `build/`.
  - Each exercise Makefile is 2-3 lines that include it.
- **`lib/failalloc.c`: link-time fault injection.** There is no `fa_*` API, so students write
  plain `malloc` everywhere.
  - Fault binaries link with
    `-Wl,--wrap=malloc,--wrap=calloc,--wrap=realloc,--wrap=reallocarray,--wrap=free,--wrap=strdup,--wrap=strndup`.
    Each `__wrap_X` counts the call and either fails it or forwards to `__real_X`. This works
    with ASan linked.
  - `FAIL_AT=N` fails the Nth allocation. A failed `realloc` returns NULL **and leaves the
    original block alive**, because testing exactly that is the point.
  - `FA_COUNT=1` prints the total allocation count to stderr on exit.
  - **The sweep** runs once with `FA_COUNT=1` to get N, then runs `FAIL_AT=1..N` under ASan.
    Every run must either pass its checks or exit with `EXIT_OOM` (3), with zero leaks and no
    sanitizer report.
  - **Documented limitation:** `--wrap` only catches calls made from the student's own object
    files. Allocations inside libc (`getline`, `asprintf`, `fopen` buffers) are invisible to it.
    Either wrap `getline` explicitly as well, or keep exercises that use it out of `[FAULT]`.
- **`lib/course.h`**: `check()`, `EXIT_OOM`, and shared helpers.
- **`scripts/verify-all.sh`**:
  - builds every solution and runs `make check` on it;
  - confirms every `[WRITE]`, `[FAULT]`, and `[DESIGN]` starter **fails to build**;
  - confirms every `[BUG]` starter **builds but fails** its check, a sanitizer, or Valgrind;
  - confirms every `[REPORT]` file's pasted output still matches a fresh capture from its
    shipped source, ignoring addresses.

  **Authors must run this before declaring a unit done.**
- **`[NO-ASAN]`**: a few demonstration exercises (e.g. "watch a UAF silently succeed without
  sanitizers") deliberately run without ASan. Flag them clearly.

## 5. Unit outline

Each unit lists its target exercises. Sizes are S (<40 lines of student code),
M (40-150), or L (150-300, capstones only).

**M01: Lifetimes: stack, static, heap** (5)
(M11 revisits this unit with ASan's `detect_stack_use_after_return=1`.)
Returning a pointer to a local, static buffers that get overwritten across calls, and
when the heap is actually needed. `[TRACE]` stack frames, `[BUG]` returning `&local`, `[PREDICT]`
what `-Wreturn-local-addr` says, `[WRITE]` the same function three ways with
lifetime trade-offs, `[EXPLAIN]`-style README answer.

**M02: `malloc` and `free` fundamentals** (6)
The `sizeof *p` idiom, NULL checks, `free(NULL)`, never using freed memory, and
`p = NULL` after free. Includes `[LOOKUP]` from `man 3 malloc` (RETURN VALUE, NOTES).

**M02b: Your first ASan and Valgrind report** (3)
Comes right after M02, because students hit sanitizer reports from M02 on. Read an ASan
heap-use-after-free report top to bottom (error line, accessed-at, freed-by, allocated-by stacks),
read a LeakSanitizer summary, and read a Valgrind memcheck leak summary. `[REPORT]` and `[PREDICT]` only.
The deeper tool work stays in M11.

**M03: `calloc`, zeroing, and size arithmetic** (5)
`calloc` vs `malloc`+`memset`, `n * size` overflow, `reallocarray`, `size_t` vs `int`,
and checking `SIZE_MAX / n`. `[BREAK]`: find a count that makes naive code allocate too little.

**M04: `realloc` done right** (6)
The `tmp` pointer pattern, `realloc(NULL, n)`, shrinking, why `realloc(p, 0)` is never
used (implementation-defined, and undefined behavior in C23), the failed-realloc leak,
pointers into a block that `realloc` moved (dangling interior pointers), and geometric
growth. `[FAULT]` required.

**M05: Ownership and create/destroy pairs** (6)
Writing ownership contracts in headers, borrowing vs transfer, `T **out` parameters,
`destroy` functions that accept NULL, and opaque types. `[DESIGN]` heavy.

**M06: Partial failure and cleanup** (6)
Multi-step constructors, reverse-order cleanup, single-exit `goto cleanup`, and
cleanup that is idempotent. Every exercise `[FAULT]`-tested at every allocation point.

**M07: Heap strings** (6)
`strdup`/`strndup`, the `+1` for NUL, string concatenation and joining with exact sizing,
`snprintf(NULL, 0, ...)` to measure, and returning heap strings with documented ownership.

**M08: Arrays of pointers and 2D grids** (6)
Jagged `char **` (e.g. split a string into words), contiguous `w*h` grids with index math,
"one allocation for the pointer table plus the data", and freeing in the right order.
Ties directly back to the RPG `Map`.

**M09: The dynamic array** (6)
Building a `Vec` with `len`/`cap`, push/pop/insert/remove, amortized growth, shrink-to-fit,
element ownership (a vector of `char *` that owns its strings), and the dangling pointer
you get from holding `&v.data[i]` across a push.

**M10: Linked structures** (6)
Singly linked list create/insert/remove/free, the UAF in a naive free loop, freeing a tree
recursively, and the double-free from shared nodes.

**M11: Reading the tools, in depth** (7)
Mostly `[REPORT]` and `[PREDICT]`. ASan shadow bytes and redzones,
`detect_stack_use_after_return=1` (back to M01's returned locals), LeakSanitizer direct vs
indirect leaks, Valgrind's four leak categories (especially **still reachable vs definitely
lost**, and why cleanup at exit still matters), "conditional jump depends on uninitialised
value" with `--track-origins=yes`, and `ASAN_OPTIONS`. **States explicitly which tool catches
what.** ASan misses uninitialised reads, and Valgrind can't run on ASan builds.
Also covers `mtrace()` with the `mtrace` script as a leak finder that needs no extra tools.

**M12: Bug hunt: the heap bug zoo** (10)
One `[BUG]` per class, followed by mixed-bag files with 2-3 bugs each and no hint about
which kinds:
- leak
- double free
- use-after-free
- heap buffer overflow
- heap underflow
- off-by-one NUL write (distinct from a generic overflow)
- uninitialised read (**Valgrind-graded**)
- invalid free (freeing stack memory or an interior pointer)
- under-allocation from `sizeof p` vs `sizeof *p`, which shows up as a heap-buffer-overflow
  that the student traces back to its root cause
- **glibc's own aborts with no sanitizer present**: `free(): double free detected in tcache 2`
  and `malloc(): corrupted top size`. The student learns to read them.

**M13: gdb for memory bugs** (4)
Breakpoints on `malloc`/`free`, and inspecting structs and arrays through pointers
(`p *v.data@v.len`). Heap corruption that crashes far from the bug, found with a hardware
watchpoint on the corrupted field. Stopping at the moment of an ASan report with
`break __asan_report_error`, or with `ASAN_OPTIONS=abort_on_error=1` plus a backtrace or core.
Valgrind and gdb together via vgdb (`valgrind --vgdb-error=0`, then `target remote | vgdb`).

**M14: Arenas and pools (intuition)** (5)
A bump arena with one free for everything. **Allocations must round up to
`_Alignof(max_align_t)`.** Students will hit that misalignment bug, and one exercise makes it happen on purpose.
Also covers a fixed-size object pool with a free list,
and when each beats `malloc` per object. These are medium-sized `[WRITE]` exercises.

**Capstones (L, 3):**
1. `wordfreq`: read a file and count words with a growable hash table of owned strings.
   It must be clean under ASan, Valgrind, and every `FAIL_AT`.
2. `gridload`: load a variable-size text map file into a heap grid, with
   all-or-nothing error handling. A `\r` not followed by `\n`, including a `\r` as the last byte of the
   file, is `GRID_ERR_TILE`. This is the ch10/ch11 RPG map done properly.
3. `lru`: an LRU cache (hash map + doubly linked list) with eviction that frees
   correctly. It ships with a provided buggy version to fix first, then gets
   extended.

**Total:** 91 exercises + 3 capstones (counted from `units/*/e*/`).

## 6. Repository layout

```
LearnCMemoryByGrinding/
├── README.md              # how to use, workflow, progress table
├── PLAN.md                # this file
├── AUTHORING.md           # rules for authors (from this plan's sections 4 and 7)
├── PROGRESS.md            # Nathan's ledger
├── common.mk
├── lib/failalloc.{h,c}
├── scripts/verify-all.sh
├── units/m01-lifetimes/ ... units/m14-arenas/
├── capstones/c1-wordfreq/ c2-gridload/ c3-lru/
└── solutions/ (mirrors units/ and capstones/)
```

## 7. Authoring rules

1. Write each unit's LESSON.md first (120-250 lines): the idea, the teaching with 1-2
   worked examples, `man` pointers, and the exercise list with tags. Exercises follow
   the lesson.
2. Instructions are clear but not spoon-fed. State the goal, the constraints, and how to
   verify. Never give step lists that dictate the implementation.
3. Every exercise is a real file set that compiles with `make`. No markdown-embedded
   snippets standing in for exercises.
4. Every solution is verified with `scripts/verify-all.sh` (checks + ASan + Valgrind +
   FAIL_AT sweep where relevant) before the unit is marked done.
5. Bug exercises must contain exactly the advertised bugs and nothing else accidental.
   A second author confirms this.
6. Don't invent tool output. Paste real ASan, Valgrind, and gdb output captured on
   this machine **from the exact shipped source**, and re-capture whenever that source changes.
   `[REPORT]` files note that addresses will differ on the student's run.
7. Cross-reference `LearnCwithRPG-terminal` ch10 and appendix C where it helps,
   but the course must stand alone.

## 8. Team and work split (pending Bruce's review)

| Author | Owns | Also |
|---|---|---|
| **Bruce** (systems educator) | Infrastructure (`common.mk`, `failalloc`, `verify-all.sh`), M11, M12, M13, M14, capstone 3 | Technical reviewer for every unit; final sign-off on all solutions |
| **Monk** (technical education writer) | M01-M07, capstone 1 | Peer review of Jack's lesson prose |
| **Jack Gable** (technical education writer) | M08-M10, capstone 2, README/AUTHORING/PROGRESS | Peer review of Monk's lesson prose |

**Order:** Bruce builds the infrastructure first, and nobody writes exercises until
`common.mk` and `failalloc` exist. Monk and Jack then author in parallel while Bruce
writes his units. Bruce reviews each unit as it lands, and the course is done when
`verify-all.sh` passes on the full tree. Bruce is also running another project, so his units
may land after Monk's and Jack's first ones. The infrastructure still comes first.

## 9. Review decisions (from Bruce, v0.2)

1. **Scope:** M13 stays its own unit but drops to 4 exercises. M11 is split, with a short M02b early and the deep M11 later. About 85 exercises total.
   (Shipped count: 91 exercises; see §5.)
2. **Solutions:** complete, compilable programs, as Nathan asked.
3. **failalloc:** link-time `--wrap`, not macros. See 4.4.
4. **Added coverage:** `realloc(p, 0)`, off-by-one NUL, glibc tcache and top-size aborts, far-away corruption plus watchpoints, stack-use-after-return, still reachable vs definitely lost, vgdb, `__asan_report_error`, and `mtrace`. `rr` is excluded on WSL2.

## Changelog

- **v0.2:** Applied Bruce's review. Starters now fail to build until the student writes the code. Build directories are separate for plain/Valgrind, ASan, and fault builds. UBSan no longer recovers, and ASan halts on its first report. ASan's blind spot for uninitialised reads is called out, and those exercises are Valgrind-graded. The size-mismatch class is renamed to under-allocation. failalloc uses `--wrap` with an `FA_COUNT` sweep and `EXIT_OOM`=3. M02b and the added bug classes and skills are in, M13 is cut to 4, and M14 states the alignment rule. Tool captures must come from the exact shipped source.
- **v0.1:** First draft.
