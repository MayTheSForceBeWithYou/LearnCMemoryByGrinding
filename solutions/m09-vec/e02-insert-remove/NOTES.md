# m09-e02-insert-remove: notes

Insert grows first, then moves `len - i` elements one slot right with a single `memmove`, then stores.
Remove reads the element out, then moves `len - i - 1` elements left. Both counts are 0 at the end of the
vector, and `memmove` with a 0 count is fine because `data` is a valid pointer by then (insert grew it, and
remove requires `i < len`, so `len > 0`).

**The usual mistakes.** Using `memcpy` (overlapping ranges: undefined). Using `len - i` as the remove count
(reads one past the last element, which ASan flags only when `len == cap`). Validating `i` *after* growing, so a
rejected insert still reallocates. That's harmless but violates "changes nothing".
