# m11-e06-track-origins: notes

1. `b->bordered`. Used in `box_area` at line 24 (`if (b->bordered)`), called from `main:36`.
   Origin: the heap block from `malloc` in `box_new` (line 14), called from `main:31`.
2. Memcheck reports only when undefinedness can change behaviour. Allocating at line 14 and
   leaving a field unwritten is not an observable event. The conditional jump at line 24 is.
3. ASan prints no report: it tracks addressability, not definedness. But ASan fills new
   blocks with `0xbe`, so `bordered` is nonzero there, `box_area` returns 6, and the *check*
   fails, with no report. The failure is a fill side effect.
4. A fresh glibc heap block carved from the top chunk is zero because the kernel hands out
   zeroed pages. That stops being true as soon as memory is reused.

**Fix.** Set `b->bordered = 0;` in `box_new` (or use `calloc`; the explicit field is clearer).
