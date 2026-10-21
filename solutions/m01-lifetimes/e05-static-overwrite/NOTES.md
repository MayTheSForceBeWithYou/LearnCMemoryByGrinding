# M01 e05 notes

1. **Swap line.** Both arguments are calls to `item_name`, and both run before `printf` does. Each
   returns the address of the same `buf`, so both `%s` print whatever the call that ran *second* wrote.
   The order in which function arguments are evaluated is unspecified. On this toolchain it's right to left, so
   `item_name(1)` runs last and the line reads `#1 shield <-> #1 shield`. Another compiler could
   print `#2 potion` twice. Neither is the intended output.
2. **Bag loop.** There is one `char[32]` object (static storage). All four `bag` elements hold its address,
   so there are 4 pointers to 1 object, and every line prints the last text written: `#3 lantern`.
3. **No sanitizer finds it.** `buf` is alive for the whole run and every access is in bounds. There's no UB, just a
   logic error in the interface.
4. **Smallest interface change:** let the caller provide the storage, as in
   `const char *item_name(char *dst, size_t cap, int id)`. Each call site then
   passes its own buffer (for example `char bagbuf[4][32]`). Returning a heap string works too, but adds frees and an OOM path.
