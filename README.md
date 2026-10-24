# Learn C Memory by Grinding

A narrow, deep course on dynamic memory in C: allocating responsibly, and finding
and fixing memory bugs. It picks up where `LearnCwithRPG-terminal` chapter 10
("The Heap") stops, and it gives you the repetition that chapter did not have room for.

The full spec is [`PLAN.md`](PLAN.md). Authors follow [`AUTHORING.md`](AUTHORING.md).
Your ledger is [`PROGRESS.md`](PROGRESS.md).

> **Status:** complete. All 15 units and 3 capstones ship with lessons, starters and solutions
> (91 unit exercises plus 3 capstones), and `scripts/verify-all.sh` passes on the full tree.

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

### On macOS (Apple Silicon)

**The course runs natively on a modern Mac.** Plain builds, ASan/UBSan, fault
injection, and leak checking all work on Apple Silicon; the full guide is
[`docs/macos.md`](docs/macos.md). The short version:

```bash
xcode-select --install          # compiler, make, lldb
make -C units/m01-lifetimes/e01-trace-frames check
```

What is different from Linux, in one paragraph: `gcc` on macOS *is* Apple Clang,
so the same Makefiles just work. LeakSanitizer is unsupported, so ASan runs with
`detect_leaks=0` and `make check` runs the macOS `leaks` tool instead of Valgrind.
Fault injection works through compile-time renaming instead of GNU `ld --wrap`
(the mechanism is in `common.mk` and `lib/failalloc.c`). And the debugger is
**lldb, not gdb** — gdb has no native run target for arm64 Darwin and cannot
`run` an arm64 binary; `docs/macos.md` has the gdb→lldb command table. Valgrind,
native gdb, `mtrace`, and `vgdb` still do not exist on macOS, so those parts of
the course (M13 e04, Valgrind reports, `mtrace` logs) use the Linux container
below. `reallocarray` (M09) needs macOS 15+.

#### The Linux container (still available)

For the parts above that macOS cannot do, the container is one command. You need
Docker or a drop-in replacement. If you do not already have one,
[colima](https://github.com/abiosoft/colima) is the lightest option:

```bash
brew install colima docker
```

Start the Linux VM (once per reboot; add `--auto-start` to have it come up on login):

```bash
colima start
```

Build the course image. This installs gcc, clang, gdb, Valgrind, GNU make, bash 5,
coreutils and the section 3 man pages:

```bash
docker build -t lcmg .
```

`scripts/lcmg-shell.sh` runs anything inside the container with the repo mounted at
`/course`. It builds the image for you if it is missing. Open a shell and work normally:

```bash
./scripts/lcmg-shell.sh
```

Or run a single command from macOS without entering the container:

```bash
./scripts/lcmg-shell.sh make -C units/m01-lifetimes/e01-trace-frames check
```

Verify the whole tree (this takes a while; it sweeps every `[FAULT]` injection):

```bash
./scripts/lcmg-shell.sh bash scripts/verify-all.sh
```

Edit files in your usual macOS editor. The mount is live, so a `make` inside the
container always compiles what you just saved.

#### Notes

- `scripts/verify-all.sh` needs bash 4+ and GNU `timeout`. On macOS, install
  them (`brew install bash coreutils`) and run it with the Homebrew bash:
  `/opt/homebrew/bin/bash scripts/verify-all.sh`. It also runs natively this way;
  the container is no longer required for it. A `[BUG]` starter whose bug is
  visible only to Memcheck reports `SKIP (Darwin)` on macOS rather than a false
  pass — verify those in the container (or rely on CI's Linux job).
- The `DEBUGINFOD_URLS` workaround above is Arch-specific. Debian ships an unstripped
  `ld.so`, so the image clears that variable and Valgrind starts without it.
- `[BUG]` starters are meant to abort, so Linux may leave `core` files in their
  directories. They are harmless; delete them or add `core` to your `.gitignore`.
- To compare GCC and Clang diagnostics on an exercise, override the compiler:
  `./scripts/lcmg-shell.sh make -C <exercise> CC=clang check`.
- CI runs the full verification on every push, on macOS arm64 and on Linux
  (`.github/workflows/ci.yml`).

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

Some early units also use `[LOOKUP]`, `[BREAK]` and `[EXPLAIN]`; their READMEs say what each asks.

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

| Unit | Topic | Exercises | State |
|---|---|---|---|
| [M01](units/m01-lifetimes/LESSON.md) | Lifetimes: stack, static, heap | 5 | complete |
| [M02](units/m02-malloc-free/LESSON.md) | `malloc` and `free` fundamentals | 6 | complete |
| [M02b](units/m02b-first-reports/LESSON.md) | Your first ASan and Valgrind report | 3 | complete |
| [M03](units/m03-calloc-size/LESSON.md) | `calloc`, zeroing, size arithmetic | 5 | complete |
| [M04](units/m04-realloc/LESSON.md) | `realloc` done right | 6 | complete |
| [M05](units/m05-ownership/LESSON.md) | Ownership and create/destroy pairs | 6 | complete |
| [M06](units/m06-partial-failure/LESSON.md) | Partial failure and cleanup | 6 | complete |
| [M07](units/m07-heap-strings/LESSON.md) | Heap strings | 6 | complete |
| [M08](units/m08-grids/LESSON.md) | Arrays of pointers and 2D grids | 6 | complete |
| [M09](units/m09-vec/LESSON.md) | The dynamic array | 6 | complete |
| [M10](units/m10-linked/LESSON.md) | Linked structures | 6 | complete |
| [M11](units/m11-tools/LESSON.md) | Reading the tools, in depth | 7 | complete |
| [M12](units/m12-zoo/LESSON.md) | Bug hunt: the heap bug zoo | 14 | complete |
| [M13](units/m13-gdb/LESSON.md) | gdb for memory bugs | 4 | complete |
| [M14](units/m14-arenas/LESSON.md) | Arenas and pools (intuition) | 5 | complete |
| [C1](capstones/c1-wordfreq/README.md) | `wordfreq` | capstone | complete |
| [C2](capstones/c2-gridload/README.md) | `gridload` | capstone | complete |
| [C3](capstones/c3-lru/README.md) | `lru` | capstone | complete |
| | **Total** | **91 + 3 capstones** | |

Work the units in order. M02b comes straight after M02 because you will meet sanitizer
reports from M02 onward.

## Layout

```
LearnCMemoryByGrinding/
├── README.md         this file
├── PLAN.md           the spec
├── AUTHORING.md      rules for authors
├── AUTHORING-INFRA.md  build mechanics for authors
├── PROGRESS.md       your ledger
├── common.mk         shared build
├── Dockerfile        Linux toolchain image (required on macOS)
├── lib/              failalloc, course.h
├── scripts/          verify-all.sh, fault-sweep.sh, lcmg-shell.sh
├── units/mNN-*/      LESSON.md + one folder per exercise
├── capstones/        c1-wordfreq, c2-gridload, c3-lru
└── solutions/        mirrors units/ and capstones/
```
