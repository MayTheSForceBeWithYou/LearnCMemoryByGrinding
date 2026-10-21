# M06 e01: Two steps, three outcomes `[FAULT]`

## Goal

Implement a two-allocation constructor and its destructor:

```c
struct image { int w, h; unsigned char *px; };   /* px: w * h bytes, zeroed */

struct image *image_create(int w, int h);
void          image_destroy(struct image *im);
```

`image_create` returns NULL on invalid dimensions (`!(w > 0 && h > 0)`), if `w * h` exceeds `IMAGE_MAX_PIXELS`,
or on allocation failure. The harness creates several images and checks their pixels. Under
`make fault`, the first allocation, then the second, fails in each image's construction.

**The starter will not build until you write both definitions.** The link error is intentional.

## Constraints

- Failure at the second allocation must free the first. No leak on any `FAIL_AT`.
- `image_destroy(NULL)` is a no-op.
- Reject `w * h > IMAGE_MAX_PIXELS` (in `image.h`) without ever computing an overflowing `int` product. UBSan traps signed overflow.
- `main` frees whatever it built so far and returns `EXIT_OOM` when a create fails.

## How to verify

- `make fault`: every `FAIL_AT` exits 0 or 3, with no leak and no report.
- `make check`: checks, ASan, Valgrind, and the sweep.
