# m13-e03-asan-stop: notes

The harness makes 13 allocations on its success path: the `IntList`, then `realloc` at
capacities 2, 4, 8 and 16 for the valid list, and one `IntList` plus at most one `realloc`
per malformed input that gets that far. The sweep runs `FAIL_AT=1..13`.

**A typical failure, really produced** by deleting the `if (!p) return -1;` after `realloc`
in `push`. `make fault`:

```
fault-sweep: FAIL_AT=2: sanitizer report (exit 1)
    main.c:18:23: runtime error: store to null pointer of type 'int'
...
fault-sweep: FAIL_AT=3: sanitizer report (exit 134)
```

At `FAIL_AT=2` the very first `realloc` fails, `l->vals` becomes NULL with `len == 0`, and
UBSan reports the store through a null pointer. At `FAIL_AT=3`, `len == 2` and the address
is `NULL + 8`. That is no longer "null" to UBSan, so the hardware fault reaches ASan's SEGV
handler. Under gdb, the signal stops first:

```
Program received signal SIGSEGV, Segmentation fault.
0x00005555555589f5 in push (l=0x7c2ff5ae0040, x=17) at main.c:18
18	    l->vals[l->len++] = x;
#0  0x00005555555589f5 in push (l=0x7c2ff5ae0040, x=17) at main.c:18
#1  0x0000555555558f19 in parse_ints (csv=0x55555555a1c0 "4,-2,17,0,99,1000000,-7,3,8", out=0x7bfff49d0030) at main.c:40
#2  0x0000555555559267 in main () at main.c:67
```

`x=17` is the third number, and `print *l` would show `vals = 0x0, len = 2`. The bug also
loses the old array: assigning `realloc`'s NULL over `l->vals` leaks it, which is why the
`p` temporary exists.

**Design points of the reference.** `push` leaves the list unchanged on failure, so the
single `fail:` path in `parse_ints` can free everything with `intlist_free`. The range check
is `errno == ERANGE || v < INT_MIN || v > INT_MAX`, because `long` is 64-bit here and
`99999999999` fits in it.
