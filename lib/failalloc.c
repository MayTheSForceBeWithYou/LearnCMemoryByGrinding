/* failalloc.c -- link-time allocation fault injection (used by `make fault`).
 *
 * The fault build links the student's objects with
 *   -Wl,--wrap=malloc,--wrap=calloc,--wrap=realloc,--wrap=reallocarray,
 *          --wrap=free,--wrap=strdup,--wrap=strndup
 * so every call to malloc() in the student's code goes to __wrap_malloc() here,
 * and __real_malloc() reaches the real allocator (ASan's, in the fault build).
 *
 * Environment:
 *   FAIL_AT=N   the Nth allocation call (malloc, calloc, realloc, reallocarray,
 *               strdup, strndup -- free is not counted) returns NULL with
 *               errno = ENOMEM. Only that one call fails. A failed realloc or
 *               reallocarray leaves the original block untouched and alive.
 *   FA_COUNT=1  print "failalloc: N allocations" to stderr at exit.
 *
 * LIMITATION: --wrap only rewrites references in the object files on the link
 * line that are linked with the flag, i.e. the student's own .c files. Calls
 * made from *inside* libc are invisible: getline(), asprintf(), open_memstream(),
 * fopen()/stdio buffers, qsort temporaries, etc. still allocate (and can still
 * fail) without being counted or injected. Either wrap such a function explicitly
 * or keep exercises that depend on it out of [FAULT].
 * Not thread-safe; course exercises are single-threaded.
 *
 * macOS: Apple's ld64 rejects --wrap, so common.mk renames the student's
 * allocation calls at compile time instead (-Dmalloc=__wrap_malloc, ...),
 * and the fault build compiles this file plain, so its calls below reach the
 * real allocator. The __real_* names below are therefore defined, on macOS,
 * as the real functions themselves. reallocarray is absent from the macOS SDK
 * entirely; the implementation is lib/lcmg_reallocarray.c, linked into every
 * macOS binary by common.mk, and __real_reallocarray names it. See docs/macos.md.
 */
#define _GNU_SOURCE
#include "failalloc.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#define __real_malloc malloc
#define __real_calloc calloc
#define __real_realloc realloc
#define __real_free free
#define __real_strdup strdup
#define __real_strndup strndup
/* lcmg_reallocarray is lib/lcmg_reallocarray.c, linked into every macOS
 * binary by common.mk. */
#define __real_reallocarray lcmg_reallocarray
#endif

static unsigned long fa_calls;
static unsigned long fa_fail_at;
static int fa_ready;

static void fa_report(void)
{
    fprintf(stderr, "failalloc: %lu allocations\n", fa_calls);
}

static void fa_init(void)
{
    if (fa_ready)
        return;
    fa_ready = 1;
    const char *s = getenv("FAIL_AT");
    if (s && *s)
        fa_fail_at = strtoul(s, NULL, 10);
    s = getenv("FA_COUNT");
    if (s && strcmp(s, "1") == 0)
        atexit(fa_report);
}

/* Runs before main, so the count is printed even if nothing is allocated. */
__attribute__((constructor)) static void fa_ctor(void) { fa_init(); }

/* Counts one allocation call; returns 1 if this call must fail. */
static int fa_should_fail(void)
{
    fa_init();
    ++fa_calls;
    if (fa_fail_at != 0 && fa_calls == fa_fail_at) {
        errno = ENOMEM;
        return 1;
    }
    return 0;
}

void *__wrap_malloc(size_t size)
{
    return fa_should_fail() ? NULL : __real_malloc(size);
}

void *__wrap_calloc(size_t n, size_t size)
{
    return fa_should_fail() ? NULL : __real_calloc(n, size);
}

void *__wrap_realloc(void *p, size_t size)
{
    return fa_should_fail() ? NULL : __real_realloc(p, size); /* p stays alive on failure */
}

void *__wrap_reallocarray(void *p, size_t n, size_t size)
{
    return fa_should_fail() ? NULL : __real_reallocarray(p, n, size);
}

void __wrap_free(void *p)
{
    __real_free(p);
}

char *__wrap_strdup(const char *s)
{
    return fa_should_fail() ? NULL : __real_strdup(s);
}

char *__wrap_strndup(const char *s, size_t n)
{
    return fa_should_fail() ? NULL : __real_strndup(s, n);
}
