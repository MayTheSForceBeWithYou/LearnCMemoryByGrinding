#define _POSIX_C_SOURCE 200809L   /* strdup/strndup are POSIX, not ISO C */
#include <stdlib.h>
#include <string.h>
#include "path.h"

char *path_join(const char *dir, const char *file)
{
    size_t ld = strlen(dir), lf = strlen(file);
    int slash = ld > 0 && dir[ld - 1] != '/';
    char *s = malloc(ld + (size_t)slash + lf + 1);
    if (s == NULL)
        return NULL;
    memcpy(s, dir, ld);
    if (slash)
        s[ld] = '/';
    memcpy(s + ld + slash, file, lf + 1);       /* includes the NUL */
    return s;
}

const char *path_basename(const char *path)
{
    const char *slash = strrchr(path, '/');
    return slash != NULL ? slash + 1 : path;
}

char *path_dirname(const char *path)
{
    const char *slash = strrchr(path, '/');
    if (slash == NULL)
        return strdup(".");
    if (slash == path)
        return strdup("/");
    return strndup(path, (size_t)(slash - path));
}

char *path_with_ext(const char *path, const char *ext)
{
    const char *base = path_basename(path);
    const char *dot = strrchr(base, '.');
    size_t keep = dot != NULL ? (size_t)(dot - path) : strlen(path);
    size_t le = strlen(ext);
    char *s = malloc(keep + le + 1);
    if (s == NULL)
        return NULL;
    memcpy(s, path, keep);
    memcpy(s + keep, ext, le + 1);
    return s;
}
