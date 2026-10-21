/* m09-e05-strvec: a vector that owns its strings. Contract first (strvec.h), then code. */
#define _POSIX_C_SOURCE 200809L    /* strdup() under -std=c17; before any #include */
#define _DEFAULT_SOURCE            /* reallocarray() */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"
#include "strvec.h"

static int oom(StrVec *v, char *loose)
{
    free(loose);
    strvec_destroy(v);
    fprintf(stderr, "m09-e05-strvec: out of memory\n");
    return EXIT_OOM;
}

int main(void)
{
    StrVec v = {0};
    check(strvec_pop(&v) == NULL, "pop on empty gives NULL");
    check(strvec_get(&v, 0) == NULL, "get out of range gives NULL");

    char word[16];
    for (int i = 0; i < 50; i++) {
        snprintf(word, sizeof word, "w%02d", i);
        if (!strvec_push_copy(&v, word))
            return oom(&v, NULL);
    }
    strcpy(word, "changed");
    check(v.len == 50 && strcmp(strvec_get(&v, 7), "w07") == 0, "push_copy copies");

    char *mine = strdup("taken");
    if (!mine)
        return oom(&v, NULL);
    if (!strvec_push_take(&v, mine))
        return oom(&v, mine);   /* course contract: a failed take leaves s with the caller */
    check(strvec_get(&v, 50) == mine, "push_take stores the very pointer");

    /* The failure half of the contract: a take whose growth cannot succeed (the doubled
     * byte count overflows) must leave s with the caller. A take that frees s on failure
     * makes the strcmp a use-after-free and the free(keep) below a double free. */
    StrVec full = {NULL, SIZE_MAX / 8, SIZE_MAX / 8};
    char *keep = strdup("kept");
    if (!keep)
        return oom(&v, NULL);
    check(!strvec_push_take(&full, keep), "take fails when growth cannot succeed");
    check(full.data == NULL && full.len == SIZE_MAX / 8 && full.cap == SIZE_MAX / 8, "failed take changed nothing");
    check(strcmp(keep, "kept") == 0, "after a failed take, s is intact and still the caller's");
    free(keep);

    char *p = strvec_pop(&v);
    check(p == mine && v.len == 50, "pop transfers the last string out");
    free(p);                                       /* ours now */
    p = strvec_pop(&v);
    check(p && strcmp(p, "w49") == 0, "pop returns owned copies too");
    free(p);

    strvec_destroy(&v);
    check(v.data == NULL && v.len == 0 && v.cap == 0, "destroy empties the vector");
    strvec_destroy(&v);
    return checks_done("m09-e05-strvec");
}

/* ---- Your definitions of the strvec.h functions go below this line. ---- */

static bool strvec_room(StrVec *v)
{
    if (v->len < v->cap)
        return true;
    if (v->cap > SIZE_MAX / 2)
        return false;
    size_t new_cap = v->cap ? v->cap * 2 : 8;
    char **tmp = reallocarray(v->data, new_cap, sizeof *tmp);
    if (!tmp)
        return false;
    v->data = tmp;
    v->cap = new_cap;
    return true;
}

bool strvec_push_take(StrVec *v, char *s)
{
    if (!strvec_room(v))
        return false;               /* contract: s is still the caller's */
    v->data[v->len++] = s;
    return true;
}

bool strvec_push_copy(StrVec *v, const char *s)
{
    if (!strvec_room(v))            /* grow first: then the copy cannot be orphaned */
        return false;
    char *copy = strdup(s);
    if (!copy)
        return false;               /* any growth stays; harmless, still owned by v */
    v->data[v->len++] = copy;
    return true;
}

char *strvec_pop(StrVec *v)
{
    if (v->len == 0)
        return NULL;
    return v->data[--v->len];
}

const char *strvec_get(const StrVec *v, size_t i)
{
    return i < v->len ? v->data[i] : NULL;
}

void strvec_destroy(StrVec *v)
{
    for (size_t i = 0; i < v->len; i++)
        free(v->data[i]);
    free(v->data);
    *v = (StrVec){0};
}
