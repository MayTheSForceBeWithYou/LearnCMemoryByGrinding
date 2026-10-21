#ifndef SSTACK_H
#define SSTACK_H

#include <stddef.h>

/* An opaque stack of strings. The definition lives in sstack.c only. */
typedef struct sstack sstack;

/* New empty stack; caller owns it (sstack_destroy). NULL on OOM. */
sstack     *sstack_create(void);
/* Frees s and every string still on it. Accepts NULL. */
void        sstack_destroy(sstack *s);
/* Pushes a copy of str (borrowed for the call). 0, or -1 on OOM with s unchanged
 * (the caller still owns str either way; it was only borrowed). */
int         sstack_push(sstack *s, const char *str);
/* Removes the top string and transfers it: the caller owns the result (free()).
 * NULL if the stack is empty. */
char       *sstack_pop(sstack *s);
/* Borrowed: the top string, owned by s. Valid until the next sstack_pop or
 * sstack_destroy on s (a push does not move existing strings). NULL if empty. */
const char *sstack_peek(const sstack *s);
/* Number of strings on s. */
size_t      sstack_size(const sstack *s);

#endif
