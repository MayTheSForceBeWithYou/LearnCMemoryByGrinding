#ifndef SSTACK_H
#define SSTACK_H

#include <stddef.h>

/* An opaque stack of strings. The definition lives in sstack.c only. */
typedef struct sstack sstack;

/* TODO: contract */
sstack     *sstack_create(void);
/* TODO: contract */
void        sstack_destroy(sstack *s);
/* TODO: contract */
int         sstack_push(sstack *s, const char *str);
/* TODO: contract */
char       *sstack_pop(sstack *s);
/* TODO: contract */
const char *sstack_peek(const sstack *s);
/* TODO: contract */
size_t      sstack_size(const sstack *s);

#endif
