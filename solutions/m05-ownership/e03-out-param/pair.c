#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "pair.h"

int parse_pair(const char *line, struct pair **out)
{
    const char *eq = strchr(line, '=');
    if (eq == NULL || eq == line)
        return -1;

    struct pair *p = malloc(sizeof *p);
    if (p == NULL)
        return -1;
    p->key = strndup(line, (size_t)(eq - line));
    if (p->key == NULL)
        goto fail_key;
    p->value = strdup(eq + 1);
    if (p->value == NULL)
        goto fail_value;

    *out = p;          /* commit last, only on success */
    return 0;

fail_value:
    free(p->key);
fail_key:
    free(p);
    return -1;
}

void pair_destroy(struct pair *p)
{
    if (p == NULL)
        return;
    free(p->key);
    free(p->value);
    free(p);
}
