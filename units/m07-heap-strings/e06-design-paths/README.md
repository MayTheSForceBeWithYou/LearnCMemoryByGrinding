# M07 e06: Path helpers with honest contracts `[DESIGN]`

## Goal

`path.h` declares four functions with no comments:

```c
char       *path_join(const char *dir, const char *file);
const char *path_basename(const char *path);
char       *path_dirname(const char *path);
char       *path_with_ext(const char *path, const char *ext);
```

- `path_join("a/b", "c.txt")` gives `"a/b/c.txt"`. Exactly one `/` separates the parts, even if `dir`
  ends with one. An empty `dir` gives just `file`.
- `path_basename("a/b/c.txt")` gives `"c.txt"`, **as a pointer into the argument**, not a copy.
- `path_dirname("a/b/c.txt")` gives `"a/b"`, a new string. It's `"."` when there's no `/`, and `"/"` for `"/x"`.
- `path_with_ext("a/b.tar", ".gz")` gives `"a/b.gz"`: the last extension of the basename is replaced (or
  appended, if there's none). Dots in directory names don't count.

Write the ownership contract above each declaration (who frees, what's borrowed, and how long a
borrowed result stays valid), then implement them in `path.c`.

**The starter will not build until you write the definitions.** The link error is intentional.

## Constraints

- Every owned result is sized exactly.
- `path_basename` allocates nothing. Its contract must say what happens to the result when the
  caller frees or modifies `path`.
- NULL on allocation failure, with nothing leaked.

## How to verify

- `make check`. The harness also frees the argument of `path_basename` *after* using the result, which
  shows the borrow's lifetime is the argument's.
