# m13-e02-far-corruption: notes

**Session** (real output, plain build):

```
Breakpoint 1, bag_label (b=0x7fffffffe820, label=0x55555555605b "inventory") at main.c:28
28	    strcpy(b->name, label);
Hardware watchpoint 2: -location b->count

Old value = 3
New value = 121
0x00007ffff7d78f3f in ?? () from /usr/lib/libc.so.6
#0  0x00007ffff7d78f3f in ?? () from /usr/lib/libc.so.6
#1  0x0000555555555315 in bag_label (b=0x7fffffffe820, label=0x55555555605b "inventory") at main.c:28
#2  0x00005555555553d2 in main () at main.c:46
```

**Mechanism.** `"inventory"` is 9 characters plus the NUL = 10 bytes, into `name[8]`.
Bytes 8 and 9 (`'y'` = 0x79 and `'\0'`) land on the two low bytes of `count` (offset 8,
little-endian), so 3 becomes 0x79 = 121. Then `bag_sum` reads `items[3]` of a 12-byte
block, and *that* is the first access ASan can see. The `strcpy` itself wrote inside `b`,
which is addressable: ASan's redzones sit between objects, not between members. Memcheck
cannot see it either.

**Fix.** `snprintf(b->name, sizeof b->name, "%s", label);` truncates to 7 characters plus
NUL. (`strncpy` would not terminate on truncation. `strncpy` + manual NUL also works.)

If you continue after the first stop, the location watchpoint fires again, inside libc
after the checks have printed: something else is now using that stack address. A location
watchpoint outlives the meaning of its address, which is why you delete it when you are done.
