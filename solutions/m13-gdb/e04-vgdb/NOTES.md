# m13-e04-vgdb: notes

**Bug.** `flags` is assigned only when `weight > 50`. Items 1, 3 and 5 keep the
uninitialised bytes `malloc` returned.

**Session** (real output, course machine):

```
Program received signal SIGTRAP, Trace/breakpoint trap.
0x000000000400138d in count_heavy (it=0x4aa3040, n=5) at main.c:33
33	        if (it[i].flags & HEAVY)
#0  0x000000000400138d in count_heavy (it=0x4aa3040, n=5) at main.c:33
#1  0x0000000004001480 in main () at main.c:47
$1 = 0
$2 = {id = 1, weight = 10, flags = 0}
00000000 00000000 ffffffff
```

`print` says `flags = 0`: the bytes happen to be zero. `get_vbits` on the 12-byte `Item`
says `id` and `weight` are defined and all 32 bits of `flags` are undefined. Memcheck tracks
validity bit by bit, and the error is raised at `& HEAVY` because the branch depends on bit 0,
which is undefined.

**Why plain and ASan pass.** Plain: fresh heap memory is zero, so `flags & HEAVY` is 0 for
the light items. ASan: the block is filled with `0xbe` bytes, and `0xbebebebe & 1` is also 0.
A malloc fill byte with bit 0 set would have exposed the bug in ASan too
(`ASAN_OPTIONS=malloc_fill_byte=1`). That is a fun check, not a method: only Memcheck
reports uninitialised reads as such.

**Fix.** `it[i].flags = weights[i] > 50 ? HEAVY : 0;`, which assigns on every path.
