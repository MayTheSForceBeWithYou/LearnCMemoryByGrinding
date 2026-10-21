#ifndef VEC_H
#define VEC_H

#include <stdbool.h>
#include <stddef.h>

/* A growable array of int. VecInt v = {0}; is a valid empty vector.
 * Invariant: len <= cap; data holds cap ints (NULL when cap == 0); data[0..len) are set.
 * The vector owns data. Pointers into data are invalidated by any call that may grow or
 * shrink the buffer. */
typedef struct {
    int    *data;
    size_t  len;
    size_t  cap;
} VecInt;

/* Appends x. On failure returns false and leaves *v unchanged. */
bool vec_push(VecInt *v, int x);
/* Removes the last element into *out. False (and *out untouched) if empty. */
bool vec_pop(VecInt *v, int *out);
/* Copies element i into *out. False (and *out untouched) if i >= len. */
bool vec_get(const VecInt *v, size_t i, int *out);
/* Frees the buffer and leaves *v as {NULL, 0, 0}. Safe to call twice. */
void vec_destroy(VecInt *v);

#endif
