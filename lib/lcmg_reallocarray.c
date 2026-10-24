/* lcmg_reallocarray.c -- reallocarray() for macOS.
 *
 * reallocarray() is a BSD/glibc function Apple has never shipped in the macOS
 * SDK, so M09 would not compile there. On macOS, common.mk renames the
 * student's reallocarray calls to lcmg_reallocarray (-Dreallocarray=...) and
 * links this file into every binary. Semantics are identical to glibc's:
 * overflow-checked n * size, then realloc; a failed call leaves the original
 * block untouched and alive, exactly like realloc.
 *
 * This file is only ever built on macOS (common.mk adds the object to the
 * build only when UNAME_S is Darwin). It is compiled plain -- never with the
 * fault-build renames -- so its call to realloc() always reaches the real
 * allocator (ASan's, in the sanitised builds).
 */
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

void *lcmg_reallocarray(void *p, size_t n, size_t size)
{
    if (size != 0 && n > SIZE_MAX / size) {
        errno = ENOMEM;
        return NULL;
    }
    return realloc(p, n * size);
}
