/* m12-e14-strv-dup [FAULT]: reference solution. */
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "course.h"

/* Returns a deep copy of the NULL-terminated string vector v: a new
 * NULL-terminated array of new strings. The caller frees it with strv_free.
 * On allocation failure returns NULL and has freed everything it allocated. */
char **strv_dup(const char *const *v);

/* Frees a vector from strv_dup and every string in it. Accepts NULL. */
void strv_free(char **v);

char **strv_dup(const char *const *v)
{
    size_t n = 0;
    while (v[n])
        n++;
    char **out = malloc((n + 1) * sizeof *out);
    if (!out)
        return NULL;
    for (size_t i = 0; i < n; i++) {
        out[i] = strdup(v[i]);
        if (!out[i]) {
            while (i > 0)
                free(out[--i]);
            free(out);
            return NULL;
        }
    }
    out[n] = NULL;
    return out;
}

void strv_free(char **v)
{
    if (!v)
        return;
    for (char **p = v; *p; p++)
        free(*p);
    free(v);
}

int main(void)
{
    const char *src[] = {"alpha", "", "gamma delta", "e", NULL};
    char **copy = strv_dup(src);
    if (!copy) {
        fprintf(stderr, "m12-e14: out of memory\n");
        return EXIT_OOM;
    }
    int same = 1;
    for (int i = 0; i < 4; i++)
        same = same && copy[i] && copy[i] != src[i] && strcmp(copy[i], src[i]) == 0;
    check(same, "every string is copied, not shared");
    check(copy[4] == NULL, "copy is NULL-terminated");
    strv_free(copy);

    const char *empty[] = {NULL};
    copy = strv_dup(empty);
    if (!copy) {
        fprintf(stderr, "m12-e14: out of memory\n");
        return EXIT_OOM;
    }
    check(copy[0] == NULL, "empty vector copies to an empty vector");
    strv_free(copy);
    strv_free(NULL);
    return checks_done("m12-e14-strv-dup");
}
