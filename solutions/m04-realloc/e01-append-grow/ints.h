#ifndef INTS_H
#define INTS_H

#include <stddef.h>

/* A growable int array. {0} is a valid empty array. The struct owns data. */
struct ints {
    int *data;
    size_t len, cap;
};

/* Appends x. 0 on success; -1 on allocation failure, with *v unchanged. */
int ints_push(struct ints *v, int x);
/* Frees the storage and leaves *v empty ({0}) and reusable. */
void ints_free(struct ints *v);

#endif
