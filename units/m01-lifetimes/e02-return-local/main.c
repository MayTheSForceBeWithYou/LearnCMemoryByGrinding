/* M01 e02: the returned local. This program builds and is wrong. */
#include <stdio.h>
#include <string.h>
#include "course.h"

char *make_greeting(const char *name);

char *make_greeting(const char *name)
{
    char buf[64];

    snprintf(buf, sizeof buf, "Hello, %s!", name);
    return buf;
}

/* Uses a fresh frame of roughly the same size, as any later call would. */
static void clobber_stack(void)
{
    volatile char junk[128];

    for (size_t i = 0; i < sizeof junk; i++)
        junk[i] = 'X';
}

int main(void)
{
    char *g = make_greeting("Nathan");

    clobber_stack();
    check(g != NULL, "make_greeting returns a string");
    check(g != NULL && strcmp(g, "Hello, Nathan!") == 0, "greeting survives later calls");

    char *h = make_greeting("Bruce");
    check(h != NULL && strcmp(h, "Hello, Bruce!") == 0, "second greeting");
    return checks_done("m01-e02-return-local");
}
