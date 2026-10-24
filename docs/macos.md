# macOS (Apple Silicon) guide

The course runs natively on a modern Mac. Almost everything works: plain builds,
ASan/UBSan, fault injection (`make fault`), and leak checking. The exceptions are
Valgrind, native gdb, and the two glibc-only tools (`mtrace`, `vgdb`) — those still
need the Linux container. This page is the whole story; the container is the fallback,
not the default.

## What works natively, and what does not

| Tool | On Apple Silicon | Notes |
|---|---|---|
| `make run`, `make build` | yes | Apple Clang, `-std=c17` |
| `make asan` (ASan + UBSan) | yes | LeakSanitizer off (see below) |
| `make fault` (fault sweep) | yes | compile-time renaming, see "Fault injection" |
| `make leaks` | yes | macOS built-in leak checker, runs in `make check` |
| `make check` | yes | runs plain, ASan, `leaks`, fault sweep |
| `lldb` | yes | the native debugger; use it instead of gdb |
| `make capture TOOL=asan\|run` | yes | |
| Valgrind / `make valgrind` | no | no Valgrind port for Apple Silicon |
| `gdb` (native) | no | see "gdb" below |
| LeakSanitizer (`detect_leaks=1`) | no | ASan aborts at startup; `make leaks` replaces it |
| `mtrace` (M11 e07) | no-op | glibc-only; the exercise compiles, the call does nothing |
| `vgdb` (M13 e04) | no | needs Valgrind + gdb; container-only |
| `reallocarray` (M09) | yes | absent from the macOS SDK entirely; the course provides it in `lib/lcmg_reallocarray.c`, linked automatically on macOS |

## Setup

1. Command Line Tools (the compiler, `make`, `lldb`):

   ```bash
   xcode-select --install
   ```

   Verify: `uname -m` prints `arm64`, and `cc --version` prints an Apple clang
   version.

2. Homebrew bash and coreutils — only needed for `scripts/verify-all.sh`
   (the system `/bin/bash` is 3.2, and macOS has no GNU `timeout`):

   ```bash
   brew install bash coreutils
   ```

   Then run verification with the Homebrew bash:

   ```bash
   /opt/homebrew/bin/bash scripts/verify-all.sh
   ```

   Ordinary exercise work (`make`, `make check`) needs nothing from Homebrew.

3. Clone and go:

   ```bash
   git clone https://github.com/MayTheSForceBeWithYou/LearnCMemoryByGrinding.git
   cd LearnCMemoryByGrinding
   make -C units/m01-lifetimes/e01-trace-frames check
   ```

## The compiler: `gcc` is Apple Clang

On macOS, `gcc`, `cc`, and `clang` are all the same compiler — Apple Clang:

```bash
$ gcc --version
Apple clang version 17.0.0 (clang-1700.0.13.3)
Target: arm64-apple-darwin25.6.0
```

The course's `common.mk` defaults to `gcc` and the flags (`-std=c17 -Wall -Wextra
-Wpedantic -g`) are the same on both platforms, so every exercise Makefile works
unchanged. `make CC=clang` is the same compiler under a different name. A real GCC
(`brew install gcc`, invoked as `gcc-14` or similar) also works, but nothing in the
course needs it — Apple Clang is the supported compiler here.

One macOS-only flag is set for you in `common.mk`: `-D_DARWIN_C_SOURCE`. Strict
`-std=c17` hides the POSIX declarations (`strdup`, `strndup`, `getline`) from the
macOS SDK headers; that flag restores them. It is the macOS equivalent of glibc's
`_DEFAULT_SOURCE`. You do not need to add it yourself.

## Sanitizers and leak checking

`make asan` works exactly as on Linux: ASan + UBSan, halting on the first report.
The one difference is leak detection. LeakSanitizer is unsupported on macOS —
with `detect_leaks=1`, ASan aborts at startup before `main` runs. `common.mk`
therefore sets `detect_leaks=0` on macOS automatically, and leak checking moves
to the macOS built-in tool:

```bash
make leaks          # leaks --atExit -- ./build/plain/prog
```

`make check` runs this step in place of Valgrind. It exits non-zero when leaks
are found, so a passing `make check` on macOS still means "no leaks".

## Fault injection

`make fault` works natively. On Linux it uses GNU `ld --wrap` to redirect the
student's allocation calls into `lib/failalloc.c`; Apple's linker rejects
`--wrap`, so on macOS `common.mk` does the same renaming at compile time
(`-Dmalloc=__wrap_malloc`, …) for the fault objects only, and `failalloc.c`
calls the real allocator directly. Same sweep (`FAIL_AT=1..N`), same output,
same `failalloc: N allocations` count. Nothing about the exercises changes.

## Debugging: use lldb, not gdb

### Why gdb does not work here

gdb has no native run target for arm64 Darwin. A Homebrew gdb announces it in
its own banner:

```
This GDB was configured as "--host=aarch64-apple-darwin25.6.0 --target=x86_64-apple-darwin20"
```

It can read symbols (with `-gdwarf-4`, see below) and set breakpoints, but `run`
fails with `Don't know how to run.` There is no flag that fixes this.

Two workarounds exist and both are worse than lldb:

- **Compile an Intel binary** (`-arch x86_64`) and run it under Rosetta 2, which
  matches the target this gdb build does have. It sometimes works, but the next
  wall is macOS codesigning: gdb needs a `com.apple.security.cs.debugger`
  entitlement (or `taskgated` access) to attach, and a stock Homebrew gdb does
  not have it. Do not fight this; it is not the course.
- **Use the Linux container** (`./scripts/lcmg-shell.sh`), where real gdb works.
  This is the right choice for the gdb-specific parts of M13 (see below).

### lldb quickstart

lldb ships with the Command Line Tools and debugs arm64 natively. The plain
build is `-g` with no optimisation, so every local is in memory and every line
has an address — same as for gdb.

```bash
lldb ./build/plain/prog
(lldb) breakpoint set --file main.c --line 6     # or: b main.c:6
(lldb) run
(lldb) frame variable                            # locals, like `info locals`
(lldb) print v.data[0]                           # like gdb `print`
(lldb) next                                       # step over; `s` steps in
(lldb) thread backtrace                           # `bt`
(lldb) memory read --format x --size 8 --count 4 v.data
```

### gdb → lldb translation

| gdb | lldb |
|---|---|
| `gdb ./prog` | `lldb ./prog` |
| `b 6` / `break main.c:6` | `b main.c:6` / `breakpoint set --file main.c --line 6` |
| `break realloc` | `breakpoint set --name realloc` |
| `break realloc` + `commands` logger | `breakpoint set --name realloc --command "bt 2" --command "continue"` (or `--auto-continue true`) |
| `break buf_add if b->len == b->cap` | `breakpoint set --file main.c --line 13 --condition 'b->len == b->cap'` |
| `run` / `r` | `run` / `r` |
| `next` / `step` | `n` / `s` |
| `print x` / `p x` | `print x` / `p x` |
| `print *v.data@v.len` | `parray 4 v.data` (or `print v.data[0]`, `...[1]`, …) |
| `info locals` | `frame variable` |
| `bt` | `bt` / `thread backtrace` |
| `watch -l b->count` | `watchpoint set expression -- &b->count` |
| `watch game.round` | `watchpoint set variable game.round` |
| `finish` | `thread step-out` |
| `continue` | `c` |
| `x/4gx ptr` | `memory read --format x --size 8 --count 4 ptr` |
| `info registers` | `register read` |
| `quit` | `quit` |

Two lldb notes for this course:

- **Breaking on the allocator.** `breakpoint set --name realloc` works before
  `run`, just like gdb's `break realloc`. At the breakpoint, the first two
  arguments are in `$x0` and `$x1` (AArch64 calling convention), not `$rdi`/`$rsi`
  as on x86-64 Linux. The M13 lesson text quotes x86-64 registers; on your Mac
  read `$x0`/`$x1` instead.
- **ASan stops.** When an ASan build reports, lldb stops at the report
  automatically — the same workflow as `demo-asan` in M13, with lldb commands.

### The `-gdwarf-4` gotcha

If you compile with plain `-g` and point a Homebrew gdb at the result, gdb
complains `DW_FORM_GNU_str_index or DW_FORM_strx used without .debug_str
section` and breakpoints misbehave. That is DWARF5 (the compiler default)
meeting a gdb that cannot read it from a macOS `.dSYM` bundle. The fix is
`-gdwarf-4` on the compile line. You do **not** need it for lldb — lldb reads
DWARF5 fine — so `common.mk` does not set it. It only matters if you are
deliberately feeding a binary to Homebrew gdb, which, per above, cannot run it
anyway.

## Per-unit exceptions

Most units are identical on macOS. The exceptions:

- **M02b e03 (predict Valgrind).** The exercise is reading a Valgrind report.
  Read the captured report in the lesson and do the prediction; to *produce* a
  Valgrind report yourself, use the container.
- **M09 (vectors).** Uses `reallocarray`, which the macOS SDK does not ship.
  The course provides it in `lib/lcmg_reallocarray.c` (renamed by
  `common.mk` and linked into every macOS binary), with identical
  overflow-checked semantics, so M09 builds and runs natively. In the fault
  build the call is still counted by the sweep, exactly as on Linux.
- **M11 e07 and the M11 lesson demo.** They call glibc's `mtrace()`. On macOS
  the call compiles to a no-op (guarded by `#if defined(__GLIBC__)`), so the
  exercises build and pass — you just cannot produce an `mtrace` log there.
  The log is a Linux-container artifact; the lesson explains it.
- **M12 (the zoo).** The exercises themselves build and run natively. The
  Valgrind captures (`capture-*-vg.txt`) are pre-recorded Linux output for
  reading; reproduce them in the container if you want your own.
- **M13 (gdb).** Do the four techniques with lldb using the translation table
  above. The lesson's transcripts are real gdb output from Linux, so register
  names and addresses differ. **e04 (vgdb) is container-only**: it needs
  Valgrind's gdbserver, which does not exist on macOS.
- **`make capture TOOL=valgrind`.** Container-only. `TOOL=asan` and `TOOL=run`
  work natively.

## Verification

```bash
/opt/homebrew/bin/bash scripts/verify-all.sh
```

This needs the Homebrew bash (system bash is 3.2) and coreutils (`gtimeout`).
One macOS caveat: a `[BUG]` starter whose bug is visible *only* to Memcheck
passes `make check` on a Mac (no Valgrind in the loop), so `verify-all` reports
those as `SKIP (Darwin)` rather than FAIL. They are verified in CI's Linux job
and in the container. Everything else is a real pass/fail.

## When to use the container anyway

`./scripts/lcmg-shell.sh` still works exactly as before, and it is the right
tool for: any Valgrind work, gdb itself, vgdb (M13 e04), `mtrace` logs
(M11 e07), and M09 on a pre-Sequoia Mac. The container and the native build
agree on everything else — CI runs both on every push.

## CI

GitHub Actions runs the full verification on every push: a macOS arm64 job
(`macos-latest`) and a Linux job (`ubuntu-latest`), each running
`scripts/verify-all.sh` over the whole tree, plus an lldb smoke test on macOS.
See `.github/workflows/ci.yml`. If CI is green on macOS, a native checkout
builds and passes there too.
