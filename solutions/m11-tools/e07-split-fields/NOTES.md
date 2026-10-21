# m11-e07-split-fields: notes

Count separators first, allocate `n + 1` slots, then `strndup` each field. On failure at
field `i`, free fields `i-1 .. 0` and the array. The sweep has N = 7 for the harness
(5 + 2 allocations), and every run exits 3.

`strchr(start, sep)` gives the end of a field; when it returns NULL the field runs to the
end of the line. The empty line gives one empty field because the separator count is 0.

**The usual mistake** is freeing only the array on failure (leaking earlier fields), or
freeing `fields[i]` for the slot that just failed. That one is NULL, so it is harmless,
but it shows confusion about which slots are live. A loop `while (i > 0) free(fields[--i]);`
frees exactly the filled slots.

**Alternative:** a single allocation holding the pointer table and all strings (M08's
"one block"). It has exactly one failure point, and `free_fields` becomes `free`.
