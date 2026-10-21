# M07: Heap strings

## Advance organizer

A C string is not a type. It's a convention: a `char` array whose meaningful content
ends at the first `'\0'`. Every heap-string bug is a disagreement between the bytes you
allocated and the bytes the convention needs:

- **length** `strlen(s)` counts the characters *before* the NUL;
- **size** is what you allocate and what a buffer holds: **length + 1**.

This unit covers `strdup` and `strndup`, exact sizing for concatenation and joining, measuring
formatted output with `snprintf(NULL, 0, ...)`, and documenting who owns a returned string.
It brings together M02 (sizing), M03 (overflow in size arithmetic), M05 (ownership), and M06
(failure paths) on the data type you'll allocate most often.

## Foundations

### Length vs size, in memory

```c
char *s = malloc(6);
memcpy(s, "hello", 6);    /* h e l l o \0 : 6 bytes, length 5 */
```

The block is 6 bytes, and valid indices are `i >= 0 && i < 6`. `s[5]` is the NUL. `strlen(s)` is 5.
`malloc(strlen(src))` followed by `strcpy` writes one byte past the end. It's the single most
common heap overflow in C, and it's sneaky, because glibc's 16-byte chunk granularity means the
stray byte usually lands in padding and nothing visible happens. ASan's redzones are exact to the
byte, so it reports `heap-buffer-overflow`, "0 bytes after N-byte region", on the NUL write.

### `strdup` and `strndup` need a feature-test macro

`strdup(s)` returns a new heap copy of `s` (`strlen(s) + 1` bytes). `strndup(s, n)` copies at
most `n` characters and **always** NUL-terminates, so the block is at most `n + 1` bytes. Both
return NULL on allocation failure, and both are caller-frees.

Neither is in C17 (C23 adds them). They come from POSIX.1-2008. Under the course's
`-std=c17`, glibc's headers declare only ISO C names, so you must ask for POSIX explicitly,
**before the first `#include`** in the file:

```c
#define _POSIX_C_SOURCE 200809L
#include <string.h>
```

The headers decide what to declare when they're first included, so a `#define` after `<string.h>`
does nothing. If you forget it, gcc 14 and later stop with "implicit declaration of function 'strdup'"
(C99 removed implicit declarations, and gcc 14 made that an error by default). Older compilers assumed a function returning
`int`, which silently truncated the returned pointer to 32 bits. M05 introduced this. `getline` needs the same macro.

### Exact sizing for concatenation

```c
char *concat(const char *a, const char *b)
{
    size_t la = strlen(a), lb = strlen(b);
    if (lb > SIZE_MAX - la - 1)        /* la + lb + 1 would wrap */
        return NULL;
    char *s = malloc(la + lb + 1);
    if (s == NULL)
        return NULL;
    memcpy(s, a, la);
    memcpy(s + la, b, lb + 1);        /* copies b's NUL too */
    return s;
}
```

Measure once, allocate once, and copy with `memcpy` at known offsets. `strcat` rescans the destination
from the start on every call (quadratic for joins), and `strcpy` has no bound, but here the size
is already right, so they aren't needed. The overflow check is pedantic for real strings, but
it costs one comparison, and it's the M03 habit.

### Measuring with `snprintf(NULL, 0, ...)`

`snprintf` returns the number of characters the full output *would* have, excluding the NUL,
whatever buffer size you gave it. With a NULL buffer and size 0, it writes nothing and just
measures:

```c
int n = snprintf(NULL, 0, "%s: %d/%d", name, hp, max);
if (n < 0) return NULL;                 /* encoding error */
char *s = malloc((size_t)n + 1);
if (s == NULL) return NULL;
snprintf(s, (size_t)n + 1, "%s: %d/%d", name, hp, max);
```

For your own variadic `fmt_alloc(const char *fmt, ...)`, use `vsnprintf` twice, with a
**`va_copy`** of the argument list for the first pass. A `va_list` can be traversed once, and
reusing it is undefined. (`asprintf` does all this, but it's a GNU extension and allocates inside libc,
so the course's fault injector can't fail it.)

### Returning heap strings: say who frees

A function returning `char *` should say "caller frees". One returning `const char *` usually
means "borrowed" and should say valid until what. Returning a pointer *into an argument* (like
a basename into the path) is a borrow of the *caller's* storage. Document that too, because it dies
when the caller's string does.

## Worked example 1: the missing +1

```c
char *shout(const char *s)
{
    size_t n = strlen(s);
    char *r = malloc(n + 1);           /* "!" needs one more byte */
    if (r == NULL) return NULL;
    memcpy(r, s, n);
    r[n] = '!';
    r[n + 1] = '\0';                   /* writes byte n + 1 of an (n + 1)-byte block */
    return r;
}
```

The result has length `n + 1` (the text plus `!`), so it needs size `n + 2`. The block B has
indices `i >= 0 && i < n + 1`, and `r[n + 1]` is the first byte after B.

**Rejected reading:** "I added +1, so the NUL is covered." The +1 covered the NUL of the *input's*
length. Every character you add to the output needs its own byte. Derive the size from the
output's length, not the input's: `size = output length + 1`. Under the plain build, this
usually "works", because B is rounded up to 16+ bytes. Under ASan it fails at once. That's why
`make check` runs both.

## Worked example 2: `strndup` and the truncated borrow

```c
const char *line = "key=value";
const char *eq = strchr(line, '=');
char *key = strndup(line, (size_t)(eq - line));   /* "key", 4-byte block */
```

`eq - line` is 3, a `ptrdiff_t`, which is non-negative here. `strndup` copies 3 characters and appends the NUL.
`line` is untouched (it's a string literal and must not be written). `key` is a new block that the caller owns.

**Rejected reading:** "Just write `'\0'` over the `=` and use `line` as the key." That writes
into a string literal (UB, and a segfault on Linux, because literals live in read-only memory). Even
with a writable buffer, it modifies the caller's data and makes the key a borrow of the caller's
buffer. `strndup` gives you an independent owned copy, which is what a parser should hand out.

## man pointers

- `man 3 strdup`: both functions, plus the feature-test macro requirements in SYNOPSIS.
- `man 3 printf`: RETURN VALUE for `snprintf`, "the number of characters (excluding the
  terminating null byte) which would have been written".
- `man 3 stdarg`: `va_copy`, and why a `va_list` can't be reused.
- `man 7 feature_test_macros`: `_POSIX_C_SOURCE` and `_DEFAULT_SOURCE`.

## Check yourself

1. `char buf[] = "abc";`: what are `sizeof buf` and `strlen(buf)`, and why do they differ?
2. You concatenate `a`, `" - "`, and `b`. Write the allocation size in terms of `strlen(a)` and `strlen(b)`.
3. What does `snprintf(NULL, 0, "%d", -42)` return?
4. Why must the `#define _POSIX_C_SOURCE` line come before `#include <string.h>`?
5. `strndup("hello", 10)`: how many bytes does the result's block hold at least, and what's its length?
6. Why can't `fmt_alloc` call `vsnprintf` twice on the same `va_list`?

<details><summary>Answers</summary>

1. 4 and 3. `sizeof` counts the array, including its NUL. `strlen` counts characters before the NUL.
2. `strlen(a) + 3 + strlen(b) + 1`.
3. 3: two digits and the minus sign.
4. Feature-test macros are read by the headers when they're first included. Defined later,
   it's too late, and `strdup`'s declaration was never made visible.
5. At least 6 bytes; length 5. `strndup` stops at the source's NUL.
6. `vsnprintf` consumes the `va_list`. Using it again is undefined, so you make a `va_copy`
   for the second pass.

</details>

## Takeaways

- size = length + 1. Derive the size from the *output* length.
- Measure, check for overflow, allocate once, and `memcpy` at known offsets.
- `snprintf(NULL, 0, ...)` measures formatted output exactly. Use `va_copy` for two passes.
- `strdup`/`strndup` need `_POSIX_C_SOURCE` under `-std=c17`, defined before any include.
- Every returned string says "caller frees" or "borrowed until X".

## Exercises

| # | Folder | Tag | Size | Topic |
|---|---|---|---|---|
| e01 | `e01-my-strdup/` | `[WRITE]` | S | Write `my_strdup` and `my_strndup` from `malloc` and `memcpy` |
| e02 | `e02-off-by-one-nul/` | `[BUG]` | S | A label builder forgets one byte; find it with ASan and explain why the plain build hides it |
| e03 | `e03-join/` | `[WRITE]` | S | `str_join(parts, n, sep)`: one exactly sized allocation |
| e04 | `e04-fault-format/` | `[FAULT]` | M | `fmt_alloc(fmt, ...)` with `vsnprintf` and `va_copy`, used to build a report that survives every `FAIL_AT` |
| e05 | `e05-predict-lengths/` | `[PREDICT]` | S | Predict `sizeof`, `strlen`, `strndup`, and `snprintf` results for twelve expressions |
| e06 | `e06-design-paths/` | `[DESIGN]` | M | Path helpers returning owned strings and borrows into the argument, with contracts |
