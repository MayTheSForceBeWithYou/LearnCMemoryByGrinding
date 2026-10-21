# M04 e05 notes

- `struct token { const char *start; }` stores an address inside `lx->buf`. The first `lexer_feed` of the large input allocates a
  32-byte buffer (B1, the doubled-from-16 capacity that fits 20 bytes) and records 4 tokens pointing into B1. The second feed needs 44 bytes, so
  `reserve` calls `realloc`, which moves the text to B2 and **frees B1**. Every earlier `start` now dangles. ASan reports
  `heap-use-after-free` in `memcmp` called from `expect`, with B1's free stack in `reserve` (from the second `lexer_feed`).
- **The small input** is a single feed. There's no realloc after the tokens were recorded, so the pointers are still valid. The bug
  needs *a grow after a borrow*. Tests that feed one chunk can't find it.
- **Fix:** store `size_t off` and compute `lx->buf + off` in `token_text`. The returned pointer is still a borrow, now documented as
  "valid until the next `lexer_feed`", because a feed may move the buffer. That's the same contract `std::vector`
  iterators have in C++, and `getline`'s `*lineptr` has in C.
- Pre-sizing the buffer would hide the bug for these inputs, but not fix it.
