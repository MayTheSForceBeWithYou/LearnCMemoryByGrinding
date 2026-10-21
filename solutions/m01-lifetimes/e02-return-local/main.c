/* M01 e02 solution: the caller provides the storage. */
#include <stdio.h>
#include <string.h>
#include "course.h"

/* Writes "Hello, <name>!" into dst (at most cap bytes, NUL included).
 * Returns dst, or NULL if the greeting did not fit (dst then holds a truncated string). */
char *make_greeting(char *dst, size_t cap, const char *name);

char *make_greeting(char *dst, size_t cap, const char *name)
{
    int n = snprintf(dst, cap, "Hello, %s!", name);

    if (n < 0 || (size_t)n >= cap)
        return NULL;
    return dst;
}

static void clobber_stack(void)
{
    volatile char junk[128];

    for (size_t i = 0; i < sizeof junk; i++)
        junk[i] = 'X';
}

int main(void)
{
    char gbuf[64], hbuf[64];
    char *g = make_greeting(gbuf, sizeof gbuf, "Nathan");

    clobber_stack();
    check(g != NULL, "make_greeting returns a string");
    check(g != NULL && strcmp(g, "Hello, Nathan!") == 0, "greeting survives later calls");

    char *h = make_greeting(hbuf, sizeof hbuf, "Bruce");
    check(h != NULL && strcmp(h, "Hello, Bruce!") == 0, "second greeting");
    return checks_done("m01-e02-return-local");
}
