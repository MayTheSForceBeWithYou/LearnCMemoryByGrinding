# M13: gdb for memory bugs

## Organizer

M11 and M12 used tools that *report*. A report gives you a stack and an address. Sometimes
that is not enough: the corruption happened long before the symptom, no tool reports
anything, or you need to see the heap state at one moment. gdb lets you stop the program
anywhere, inspect memory as typed values, and catch the exact instruction that changes a
location. This unit covers four techniques: breaking on the allocator, watchpoints, stopping
inside an ASan report, and driving Memcheck from gdb with vgdb.

You need M11 (reading reports). Every transcript below is real output from
`lesson-demo/`, captured with `gdb -batch` scripts (`grow.gdb`, `watch.gdb`, `asan.gdb`) by
`make capture`. Addresses will differ on your machine.

## Foundations

### What gdb needs

The course's plain build is `-g` with no optimisation, so every local is in memory and
every line has an address. glibc and libasan on Arch are stripped of debug info. Their
frames show as `?? () from /usr/lib/libc.so.6` or a bare exported name, and you cannot see
their arguments by name. The scripts start with `set debuginfod enabled off` so gdb does
not try to download symbols for them. Interactively you may leave debuginfod on.

On x86-64 Linux (System V ABI), the first integer/pointer arguments arrive in `rdi`, `rsi`,
`rdx`, `rcx`, `r8`, `r9`. At a breakpoint on the *first instruction* of a function without
debug info, `$rdi` and `$rsi` are its first two arguments. That is how you read
`realloc(ptr, size)` when libc has no symbols. The registers are only valid at entry:
the function overwrites them as it runs.

### Breakpoints on the allocator

`break realloc` and `break free` work before `run`: gdb resolves them in libc once it is
loaded. A `commands` block turns a breakpoint into a logger:

```
break realloc
commands
  silent
  printf "realloc(%p, %lu)\n", $rdi, $rsi
  bt 2
  continue
end
```

A *conditional* breakpoint stops only when an expression in the frame is true. From
`capture-grow.txt` (`break buf_add if b->len == b->cap`, then `finish` and `continue`):

```
Breakpoint 1, buf_add (b=0x7fffffffe770, c=119 'w') at main.c:13
13	    if (b->len == b->cap) {
$1 = {data = 0x0, len = 0, cap = 0}
...
Value returned is $3 = 0

Breakpoint 1, buf_add (b=0x7fffffffe770, c=104 'h') at main.c:13
13	    if (b->len == b->cap) {
$4 = {data = 0x555555559010 "watc", len = 4, cap = 4}
$5 = "watc"
$6 = {119 'w', 97 'a', 116 't', 99 'c'}
0x555555559010:	119 'w'	97 'a'	116 't'	99 'c'	0 '\000'	0 '\000'	0 '\000'	0 '\000'
```

`finish` runs to the end of the current function and prints its return value.

### Seeing a heap array: `@`

A pointer to the first element tells gdb nothing about the length. `print *p@n` builds an
artificial array of `n` elements starting at `*p`: `print *b->data@b->len` gave `$5` above.
`print/c b->data[0]@4` formats each element as a character. `x/8cb b->data` is the raw view:
**x**amine 8 units of size **b**yte, formatted as **c**har. Note that `x` happily showed four
bytes past `len`. gdb reads whatever the process can read. It has no idea which bytes are
"yours", and printing past `len` (or past the block) is not an error to it. Those bytes
here happen to be zero because the block is fresh memory from the top of the heap.

### Watchpoints: who wrote this?

A watchpoint stops when a value changes. On x86-64, gdb uses the CPU's debug registers
(four, each covering up to 8 aligned bytes), so a *hardware* watchpoint costs nothing until
it fires. If gdb says `Watchpoint` without `Hardware`, it is single-stepping. That is
thousands of times slower, and usually means the expression was too big or not an lvalue.

The trap fires **after** the store, so gdb shows the *next* statement. From
`capture-watch.txt` (`watch game.round`, then `continue`, `bt`, `print i`, `print v`):

```
Hardware watchpoint 2: game.round

Old value = 1
New value = 40
record (i=4, v=40) at main.c:43
43	}
#0  record (i=4, v=40) at main.c:43
#1  0x0000555555555365 in watch () at main.c:48
#2  0x00005555555554a3 in main (argc=2, argv=0x7fffffffe8f8) at main.c:78
$1 = 4
$2 = 40
round 40
```

`game.scores[4]` is one past the array, and in this layout it is `game.round`. Nothing
crashed, and no tool in M11 would have reported it: it is an overflow *within one object*,
which ASan's redzones (between objects) and Memcheck (addressability only) cannot see.
The watchpoint names the writer: `record`, called with `i=4`.

`watch expr` is tied to the scope of the variables in `expr`. When that frame returns, the
watchpoint is deleted. `watch -l expr` (`-location`) evaluates `expr` once and watches that
*address*, whatever scope you are in. Use `-l` for heap fields reached through a pointer:
`watch -l b->count`. A location watchpoint stays alive after the frame returns, so later
reuse of the same stack slot also triggers it. Delete it when you have your answer.

### Stopping inside an ASan report

ASan instruments each access with a shadow check. On failure it calls a reporting function
such as `__asan_report_store4` (a 4-byte write), which prints the report and, with the
course's `abort_on_error=1`, raises `SIGABRT`. gdb sees the signal before the process dies,
so `bt` under gdb shows the faulting frame. From `capture-asan.txt`:

```
Breakpoint 2 at 0x7ffff793946c
No breakpoints made.
Breakpoint 3 at 0x7ffff793ada0
...
2       breakpoint     keep y   0x00007ffff793946c <__asan_report_error+12>
3       breakpoint     keep y   0x00007ffff793ada0 <__asan_report_store4>

Breakpoint 3, 0x00007ffff793ada0 in __asan_report_store4 () from /usr/lib/libasan.so.8
#0  0x00007ffff793ada0 in __asan_report_store4 () from /usr/lib/libasan.so.8
#1  0x0000555555557d4a in asan () at main.c:57
#2  0x0000555555558022 in main (argc=2, argv=0x7fffffffe8b8) at main.c:80
```

Three things to take from this:

1. `break __asan_report_error` is the advice you will find online, and gdb accepts it
   (breakpoint 2). **It never fired.** With GCC 16's libasan, the path from
   `__asan_report_store4` to the printed report does not go through that function's entry.
   `rbreak ^__asan_report_store[0-9]+$` made no breakpoints at all with this stripped
   library. What works here is to break on the specific `__asan_report_{load,store}N` named by
   the report (`WRITE of size 4` means `store4`), or simply to let `abort_on_error=1` raise
   `SIGABRT` and read `bt`. The second run in the capture does that: frame `#7` is
   `__asan_report_store4` and `#8` is `asan () at main.c:57`.
2. Breaking *before* the report gives you the live process at the faulting access: `frame 1`,
   then `print` any local, `info locals`, `x` the shadow region.
3. LeakSanitizer does not run under ptrace. Leak reports need the program to run outside
   gdb. Use gdb for access errors and LSan's own stacks for leaks.

### Signals from the fault build

When `FAIL_AT=N` makes an allocation return NULL and the code writes through it anyway, the
result is `SIGSEGV`. Under gdb, the signal stops the program *before* ASan's SEGV handler
runs: `export FAIL_AT=3` (or `set environment FAIL_AT 3` inside gdb), `run`, `bt`. This is
how M13 e03 is debugged.

### vgdb: gdb driving Memcheck

`valgrind --vgdb=yes --vgdb-error=0 prog` starts the program under Memcheck and waits for a
debugger before the first instruction. In another terminal, `gdb prog` and then
`target remote | vgdb`. On each error, Memcheck stops the program as if at a breakpoint
(`SIGTRAP`). From `capture-vgdb.txt` (the `uninit` mode, then `bt`, `print v[1]`, and
`monitor get_vbits` on the 8 bytes of `v`):

```
Program received signal SIGTRAP, Trace/breakpoint trap.
0x0000000004001415 in uninit () at main.c:67
67	    if (v[1] > v[0])
...
$1 = 0
00000000 ffffffff
```

`print v[1]` says `0`. gdb shows the bits in memory, and it cannot know they were never
written. `monitor` passes a command to Valgrind's gdbserver. `get_vbits addr len` (numbers, not expressions: the text goes to Valgrind unparsed, hence the `eval "monitor get_vbits %p 8", v` in `demo-vgdb`) prints
Memcheck's *validity bits*, 2 hex digits per byte, where a set bit means "undefined". Here
`v[0]` is fully defined and `v[1]` fully undefined. Other useful monitor commands:
`monitor leak_check summary` (a leak check *now*, mid-run) and
`monitor who_points_at addr` (which pointers keep a block reachable).

### Core files

On this machine `core_pattern` pipes cores to systemd-coredump, so "core dumped" leaves no
file in the exercise dir. On the course's WSL setup, `coredumpctl list` still said
`No coredumps found` after several aborts, so do not count on post-mortem cores here.
Reproduce under gdb instead: everything in this unit needs a live process anyway.

## Check yourself

1. A heap field changes to garbage somewhere in a 5000-line program. Which gdb command,
   set where, finds the writer?
2. Why does `watch b->count`, set in `bag_init`, not catch a write made later in `main`?
3. At a breakpoint on `free` in a stripped libc, how do you print the pointer being freed?
4. vgdb stopped on `Conditional jump ... uninitialised`. `print x` says `7`. Is `x` defined?

<details><summary>Answers</summary>

1. Break after the field is set correctly, then `watch -l p->field` and `continue`. The stop
   is in the statement after the write, and `bt` shows who wrote it.
2. `b` is `bag_init`'s parameter. The watchpoint is scoped to that frame and is deleted
   when `bag_init` returns. `watch -l` watches the address instead.
3. `print $rdi` (or `printf "%p\n", $rdi`) at the function's first instruction.
4. Not necessarily. `print` shows bits, not validity. Ask Memcheck:
   `eval "monitor get_vbits %p %d", &x, (int)sizeof(x)`. `monitor` text goes to Valgrind
   unparsed, so gdb must compute the address and size first.
</details>

## Exercises

| Exercise | Tag | Size | Technique |
|---|---|---|---|
| e01-break-on-alloc | `[TRACE]` | S | `break realloc`/`free`, `$rdi`/`$rsi`, `print *v.data@v.len` |
| e02-far-corruption | `[BUG]` | S | `watch -l` on a field that is corrupted far from the symptom |
| e03-asan-stop | `[FAULT]` | M | write a parser; debug sweep failures under gdb |
| e04-vgdb | `[BUG]` | S | vgdb, `monitor get_vbits` (Valgrind-graded) |
