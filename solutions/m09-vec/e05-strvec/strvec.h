#ifndef STRVEC_H
#define STRVEC_H

#include <stdbool.h>
#include <stddef.h>

/* A growable array of heap strings that OWNS every string it holds.
 * StrVec v = {0}; is a valid empty vector. Fields are readable; only these functions write them.
 * Invariant: len <= cap; data[0..len) are non-NULL strings owned by the vector.
 * Release everything with strvec_destroy(). */
typedef struct {
    char  **data;
    size_t  len;
    size_t  cap;
} StrVec;

/* Appends a heap copy of s. s is borrowed for the call only.
 * Failure (copy or growth): returns false; contents and len unchanged (cap may have grown). */
bool        strvec_push_copy(StrVec *v, const char *s);

/* Appends s itself and takes ownership of it: s must come from malloc/strdup, and the
 * caller must not free it or use it after strvec_destroy/strvec_pop gives it back.
 * Failure (growth): returns false and s is STILL OWNED BY THE CALLER, who must free it. */
bool        strvec_push_take(StrVec *v, char *s);

/* Removes the last string and transfers it to the caller, who must free() it.
 * Returns NULL if the vector is empty. */
char       *strvec_pop(StrVec *v);

/* Borrowed view of string i, or NULL if i >= len. Valid until that element is popped or the
 * vector is destroyed. (Growth moves the table of pointers, not the strings, so pushes do NOT
 * invalidate it; the pointer to the slot, &v->data[i], is invalidated by growth.) */
const char *strvec_get(const StrVec *v, size_t i);

/* Frees every string, then the table, and leaves *v as {NULL, 0, 0}. Safe to call twice. */
void        strvec_destroy(StrVec *v);

#endif
