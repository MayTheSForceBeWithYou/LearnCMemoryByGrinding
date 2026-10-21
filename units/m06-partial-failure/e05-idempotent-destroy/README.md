# M06 e05: Destroy anything, even half-built `[FAULT]`

## Goal

Implement

```c
struct widget {
    char *label;
    double *samples;       /* nsamples */
    char *slots[3];        /* three heap buffers of slot_size bytes */
    size_t nsamples, slot_size;
};

struct widget *widget_create(const char *label, size_t nsamples, size_t slot_size);
void           widget_destroy(struct widget *w);
```

Here, `widget_create` doesn't get its own cleanup ladder. Instead, **it builds into a zeroed struct and
calls `widget_destroy` on any failure.** `widget_destroy` must therefore be correct on a widget at every
stage of construction, from "only the struct exists" to "fully built".

**The starter will not build until you write both definitions.** The link error is intentional.

## Constraints

- `widget_create` contains no `free` calls of its own, apart from what `widget_destroy` does.
- `widget_destroy(NULL)` is a no-op.
- No leak and no double free on any `FAIL_AT` (there are six allocation points per widget).
- In a comment above `widget_destroy`, state the invariant that makes it safe on partial objects.

## How to verify

- `make fault` and `make check`.
