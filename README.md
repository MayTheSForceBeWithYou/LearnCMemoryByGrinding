# Learn C Memory by Grinding

A narrow, deep course on dynamic memory in C: allocating responsibly, and finding
and fixing memory bugs. It picks up where `LearnCwithRPG-terminal` chapter 10
("The Heap") stops, and it gives you the repetition that chapter did not have room for.

The full spec is [`PLAN.md`](PLAN.md). Authors follow [`AUTHORING.md`](AUTHORING.md).
Your ledger is [`PROGRESS.md`](PROGRESS.md).

> **Status:** under construction. The shared build (`common.mk`, `lib/`, `scripts/`) is in place.
> M08-M10 and capstone 2 are complete and pass `scripts/verify-all.sh`, pending Bruce's review.
> See the table below.

## What you will be able to do

Without looking anything up beyond `man`:

- pick stack, static or heap storage for a given lifetime, and say why;
- use `malloc`, `calloc`, `realloc` and `free` correctly on every error path,
  including `n * size` overflow and `realloc` growth that never leaks;
- write and honour ownership contracts: who allocates, who frees, borrow vs transfer;
- write multi-allocation constructors that clean up correctly when any step fails;
- manage heap strings, pointer tables, 2D grids, dynamic arrays and linked structures;
- read ASan, LeakSanitizer, UBSan, Valgrind memcheck and gdb output and go straight to the bug;
- force allocation failures on purpose and prove your code survives each one.

What it is not: a project, an allocator-implementation course, or C++.

## Environment

Arch Linux on WSL2. Work under `~/dev/c`, never under `/mnt/c`.
You need `gcc`, `clang`, `gdb`, `valgrind`, `make` and `man` on PATH. `rr` is not used
(WSL2 does not expose the performance counters it needs).

**Valgrind on Arch refuses to start** with "a function redirection which is mandatory
for this platform-tool combination cannot be set up". Arch ships a stripped `ld.so`,
and the packages the message suggests do not exist on Arch. The fix is debuginfod:

```bash
echo "$DEBUGINFOD_URLS"     # empty? open a new login shell, or set it:
export DEBUGINFOD_URLS=https://debuginfod.archlinux.org
```

Details: `LearnCwithRPG-terminal/appendices/c-gdb-and-sanitizers.md`, section
"On Arch: it refuses to start", and appendix A of that course.

## How an exercise works

Each exercise is a folder with a `README.md`, starter source and a 2-3 line `Makefile`
that includes the shared `common.mk`. The README tells you the goal, the constraints and
how to verify. It does not tell you how. At most one hint sits in a collapsed block.

The starter's harness prints `<exercise-id>: all checks passed` and exits 0 on success,
or exits 1 on failure. If an allocation fails and the program handles it gracefully, it prints
one line to stderr and exits **3** (`EXIT_OOM`).

### Starters that do not build (on purpose)

`[WRITE]`, `[FAULT]` and `[DESIGN]` starters **fail to build until you write the code.**
They declare prototypes and the harness calls them. There are no dummy stubs. Until you
write the definitions, the link fails with `undefined reference to ...`. That is your
to-do list, not a broken exercise.

`[BUG]`, `[REPORT]`, `[PREDICT]` and `[TRACE]` files are complete programs that build.
A `[BUG]` starter builds and then fails its own check, a sanitizer or Valgrind.

### Tags

| Tag | You... |
|---|---|
| `[WRITE]` | implement functions until the checks pass |
| `[BUG]` | find and fix the defect, then explain the mechanism in `ANSWER.md` |
| `[REPORT]` | pinpoint the bug from a real ASan/Valgrind report and the source, before running anything |
| `[PREDICT]` | write down what the tool will say (leak counts, direct vs indirect, which line), then run it |
| `[TRACE]` | draw stack and heap state on paper at the marked points |
| `[FAULT]` | survive every forced allocation failure from the fault-injection build |
| `[DESIGN]` | write the ownership contract in the header, then implement it |
| `[NO-ASAN]` | (marker) a demonstration that deliberately runs without sanitizers |

Monk's early units also use `[LOOKUP]`, `[BREAK]` and `[EXPLAIN]`; their READMEs say what each asks.

### Make targets

These come from `common.mk`. Authors: the full mechanics are in `AUTHORING-INFRA.md`.

| Target | Does |
|---|---|
| `make run` | plain build in `build/plain/`, runs the harness |
| `make asan` | ASan + UBSan build in `build/asan/`, halts on the first report, leak detection on |
| `make valgrind` | runs the plain build under memcheck (`--leak-check=full`, definite/indirect leaks are errors) |
| `make fault` | ASan build with the `failalloc` wrappers in `build/fault/`, sweeps `FAIL_AT=1..N` |
| `make check` | all of the above that apply to this exercise |
| `make clean` | removes `build/` |

To see how many allocations a program makes: `make fault`, then `FA_COUNT=1 ./build/fault/prog`.

ASan and Valgrind never run on the same binary. That is why the build directories are separate.

### Done means clean everywhere

An exercise is done when the checks pass, the ASan/UBSan build is silent, LeakSanitizer
reports nothing, the plain build exits 0 under Valgrind, and for `[FAULT]` work, every `FAIL_AT`
run either passes or exits 3 with no leak and no report. `[NO-ASAN]` exercises say what "done" means for them.

### Solutions

Complete reference solutions live in `solutions/`, mirroring `units/` and `capstones/`.
They exist for when you **really** need one. No README links to them.

## Course map

| Unit | Topic | Exercises | Author | State |
|---|---|---|---|---|
| M01 | Lifetimes: stack, static, heap | 5 | Monk | planned |
| M02 | `malloc` and `free` fundamentals | 6 | Monk | planned |
| M02b | Your first ASan and Valgrind report | 3 | Monk | planned |
| M03 | `calloc`, zeroing, size arithmetic | 5 | Monk | planned |
| M04 | `realloc` done right | 6 | Monk | planned |
| M05 | Ownership and create/destroy pairs | 6 | Monk | planned |
| M06 | Partial failure and cleanup | 6 | Monk | planned |
| M07 | Heap strings | 6 | Monk | planned |
| [M08](units/m08-grids/LESSON.md) | Arrays of pointers and 2D grids | 6 | Jack | complete, verify-all green, awaiting review |
| [M09](units/m09-vec/LESSON.md) | The dynamic array | 6 | Jack | complete, verify-all green, awaiting review |
| [M10](units/m10-linked/LESSON.md) | Linked structures | 6 | Jack | complete, verify-all green, awaiting review |
| M11 | Reading the tools, in depth | 7 | Bruce | planned |
| M12 | Bug hunt: the heap bug zoo | 10 | Bruce | planned |
| M13 | gdb for memory bugs | 4 | Bruce | planned |
| M14 | Arenas and pools (intuition) | 5 | Bruce | planned |
| C1 | `wordfreq` | capstone | Monk | planned |
| [C2](capstones/c2-gridload/README.md) | `gridload` | capstone | Jack | complete, verify-all green, awaiting review |
| C3 | `lru` | capstone | Bruce | planned |

Work the units in order. M02b comes straight after M02 because you will meet sanitizer
reports from M02 onward.

## Layout

```
LearnCMemoryByGrinding/
├── README.md         this file
├── PLAN.md           the spec
├── AUTHORING.md      rules for authors
├── AUTHORING-INFRA.md  build mechanics for authors (Bruce)
├── PROGRESS.md       your ledger
├── common.mk         shared build
├── lib/              failalloc, course.h
├── scripts/          verify-all.sh, fault-sweep.sh
├── units/mNN-*/      LESSON.md + one folder per exercise
├── capstones/        c1-wordfreq, c2-gridload, c3-lru
└── solutions/        mirrors units/ and capstones/
```
