/* course.h -- the tiny shared test harness for LearnCMemoryByGrinding.
 *
 *     check(r != NULL, "create returns an object");
 *     ...
 *     return checks_done("m06-e01");
 *
 * check() prints a FAIL line (to stdout) and counts the failure; it never aborts.
 * checks_done() prints "<id>: all checks passed" and returns 0, or prints the
 * failure count and returns 1. Return its value from main.
 * Graceful out-of-memory: free what you own, print one line to stderr, return EXIT_OOM.
 */
#ifndef COURSE_H
#define COURSE_H

#include <stdio.h>

#ifdef __APPLE__
#include <stddef.h>
/* reallocarray is absent from the macOS SDK; the course provides it in
 * lib/lcmg_reallocarray.c (renamed by common.mk). Declared here, where every
 * exercise sees it, so student code compiles -- Apple Clang errors on
 * implicit function declarations. */
void *lcmg_reallocarray(void *p, size_t n, size_t size);
/* The fault build renames allocation calls to the __wrap_* wrappers at
 * compile time (common.mk FAULT_DEFS). The other wrappers need no
 * declaration here: the SDK headers declare malloc/calloc/realloc/free and
 * strdup/strndup, and the rename applies to those declarations too. Only
 * reallocarray has no SDK declaration to rename, so its wrapper is
 * declared here. */
void *__wrap_reallocarray(void *p, size_t n, size_t size);
#endif

#define EXIT_OOM 3

static inline int *course_failures_(void)
{
    static int failures;
    return &failures;
}

static inline void course_check_(int ok, const char *msg, const char *file, int line)
{
    if (!ok) {
        printf("FAIL %s:%d: %s\n", file, line, msg);
        fflush(stdout);
        ++*course_failures_();
    }
}

#define check(cond, msg) course_check_((cond) ? 1 : 0, (msg), __FILE__, __LINE__)

static inline int checks_done(const char *id)
{
    int n = *course_failures_();
    if (n == 0) {
        printf("%s: all checks passed\n", id);
        return 0;
    }
    printf("%s: %d check(s) failed\n", id, n);
    return 1;
}

#endif
