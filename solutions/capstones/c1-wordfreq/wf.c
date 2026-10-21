#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wf.h"

/* Open addressing, linear probing, capacity a power of two, load <= 1/2.
 * slots[i].word == NULL marks an empty slot (no deletion, so no tombstones). */
struct slot {
    char *word;     /* owned */
    size_t count;
};

struct wf {
    struct slot *slots;
    size_t cap;     /* power of two */
    size_t size;
};

#define WF_INITIAL_CAP 16

static size_t hash(const char *s, size_t len)      /* FNV-1a */
{
    uint64_t h = 1469598103934665603u;
    for (size_t i = 0; i < len; i++) {
        h ^= (unsigned char)s[i];
        h *= 1099511628211u;
    }
    return (size_t)h;
}

struct wf *wf_new(void)
{
    struct wf *t = malloc(sizeof *t);
    if (t == NULL)
        return NULL;
    t->slots = calloc(WF_INITIAL_CAP, sizeof *t->slots);
    if (t->slots == NULL) {
        free(t);
        return NULL;
    }
    t->cap = WF_INITIAL_CAP;
    t->size = 0;
    return t;
}

void wf_free(struct wf *t)
{
    if (t == NULL)
        return;
    for (size_t i = 0; i < t->cap; i++)
        free(t->slots[i].word);
    free(t->slots);
    free(t);
}

/* Index of the slot holding word[0..len-1], or of the empty slot where it belongs. */
static size_t find(const struct slot *slots, size_t cap, const char *word, size_t len)
{
    size_t i = hash(word, len) & (cap - 1);
    while (slots[i].word != NULL &&
           !(strncmp(slots[i].word, word, len) == 0 && slots[i].word[len] == '\0'))
        i = (i + 1) & (cap - 1);
    return i;
}

/* Double the capacity. Words move by pointer: no string is copied or freed.
 * On failure the old array is untouched. */
static int grow(struct wf *t)
{
    if (t->cap > SIZE_MAX / 2 / sizeof *t->slots)
        return -1;
    size_t ncap = t->cap * 2;
    struct slot *ns = calloc(ncap, sizeof *ns);
    if (ns == NULL)
        return -1;
    for (size_t i = 0; i < t->cap; i++) {
        if (t->slots[i].word != NULL) {
            const char *w = t->slots[i].word;
            ns[find(ns, ncap, w, strlen(w))] = t->slots[i];
        }
    }
    free(t->slots);
    t->slots = ns;
    t->cap = ncap;
    return 0;
}

int wf_add(struct wf *t, const char *word, size_t len)
{
    size_t i = find(t->slots, t->cap, word, len);
    if (t->slots[i].word != NULL) {
        t->slots[i].count++;
        return 0;
    }
    char *copy = malloc(len + 1);                   /* copy first: nothing to undo if it fails */
    if (copy == NULL)
        return -1;
    memcpy(copy, word, len);
    copy[len] = '\0';
    if ((t->size + 1) * 2 > t->cap) {
        if (grow(t) != 0) {
            free(copy);
            return -1;
        }
        i = find(t->slots, t->cap, word, len);     /* old index is meaningless now */
    }
    t->slots[i].word = copy;
    t->slots[i].count = 1;
    t->size++;
    return 0;
}

size_t wf_size(const struct wf *t)
{
    return t->size;
}

size_t wf_count(const struct wf *t, const char *word)
{
    const struct slot *s = &t->slots[find(t->slots, t->cap, word, strlen(word))];
    return s->word != NULL ? s->count : 0;
}

static int by_count_then_word(const void *a, const void *b)
{
    const struct wf_entry *x = a, *y = b;
    if (x->count != y->count)
        return x->count < y->count ? 1 : -1;
    return strcmp(x->word, y->word);
}

struct wf_entry *wf_sorted(const struct wf *t, size_t *n)
{
    struct wf_entry *e = calloc(t->size ? t->size : 1, sizeof *e);
    if (e == NULL)
        return NULL;
    size_t k = 0;
    for (size_t i = 0; i < t->cap; i++)
        if (t->slots[i].word != NULL)
            e[k++] = (struct wf_entry){ t->slots[i].word, t->slots[i].count };
    qsort(e, k, sizeof *e, by_count_then_word);
    *n = k;
    return e;
}

int wf_read(struct wf *t, FILE *f)
{
    size_t cap = 16, len = 0;
    char *buf = malloc(cap);
    if (buf == NULL)
        return -1;
    int c, rc = 0;
    do {
        c = fgetc(f);
        if (c != EOF && (isalpha(c) || c == '\'')) {
            if (len == cap) {
                char *nb = cap <= SIZE_MAX / 2 ? realloc(buf, cap * 2) : NULL;
                if (nb == NULL) {
                    rc = -1;
                    break;
                }
                buf = nb;
                cap *= 2;
            }
            buf[len++] = (char)tolower(c);
        } else if (len > 0) {
            if (wf_add(t, buf, len) != 0) {
                rc = -1;
                break;
            }
            len = 0;
        }
    } while (c != EOF);
    free(buf);
    if (rc == 0 && ferror(f))       /* EOF from fgetc means end of file OR error */
        rc = -2;
    return rc;
}
