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

bool vec_push(VecInt *v, int x);       /* provided in main.c */
void vec_destroy(VecInt *v);           /* provided in main.c */

/* Inserts x before index i (valid: i <= len; i == len appends). Order of the others is kept.
 * False, and *v unchanged, if i > len or growth fails. */
bool vec_insert(VecInt *v, size_t i, int x);
/* Removes element i (valid: i < len) into *out (out may be NULL). Order of the others is kept.
 * False, and *v unchanged, if i >= len. */
bool vec_remove(VecInt *v, size_t i, int *out);

#endif
