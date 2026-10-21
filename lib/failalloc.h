/* failalloc.h -- prototypes for the link-time fault-injection wrappers.
 * Students never include or call this; they write plain malloc/free.
 * See failalloc.c for how it works and its limitations. */
#ifndef FAILALLOC_H
#define FAILALLOC_H

#include <stddef.h>

void *__real_malloc(size_t size);
void *__real_calloc(size_t n, size_t size);
void *__real_realloc(void *p, size_t size);
void *__real_reallocarray(void *p, size_t n, size_t size);
void  __real_free(void *p);
char *__real_strdup(const char *s);
char *__real_strndup(const char *s, size_t n);

void *__wrap_malloc(size_t size);
void *__wrap_calloc(size_t n, size_t size);
void *__wrap_realloc(void *p, size_t size);
void *__wrap_reallocarray(void *p, size_t n, size_t size);
void  __wrap_free(void *p);
char *__wrap_strdup(const char *s);
char *__wrap_strndup(const char *s, size_t n);

#endif
