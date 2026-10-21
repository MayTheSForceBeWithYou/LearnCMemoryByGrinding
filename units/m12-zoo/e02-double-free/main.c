/* m12-e02-double-free [BUG] */
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "course.h"

typedef struct {
    char *title; /* owned */
    int year;
} Doc;

/* Initializes d with a copy of title. Returns 0, or -1 on allocation failure. */
static int doc_init(Doc *d, const char *title, int year)
{
    d->title = strdup(title);
    if (!d->title)
        return -1;
    d->year = year;
    return 0;
}

/* Makes dst an independent copy of src. Returns 0, or -1 on allocation failure. */
static int doc_copy(Doc *dst, const Doc *src)
{
    *dst = *src;
    return 0;
}

static void doc_fini(Doc *d)
{
    free(d->title);
    d->title = NULL;
}

int main(void)
{
    Doc a, b;
    if (doc_init(&a, "Notes on Heaps", 1998) != 0)
        return EXIT_OOM;
    if (doc_copy(&b, &a) != 0) {
        doc_fini(&a);
        return EXIT_OOM;
    }
    b.year = 2026;
    check(strcmp(b.title, "Notes on Heaps") == 0, "copy has the title");
    check(a.year == 1998 && b.year == 2026, "copies are independent");
    doc_fini(&a);
    doc_fini(&b);
    return checks_done("m12-e02-double-free");
}
