/* m12-e03-uaf [BUG] */
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "course.h"

#define NSLOTS 4

typedef struct {
    const char *key;   /* borrowed: string literals only */
    char *value;       /* owned, or NULL */
} Setting;

typedef struct {
    Setting slots[NSLOTS];
} Settings;

/* Returns the value for key (borrowed, valid until the next settings_set
 * on the same key), or NULL if absent. */
static const char *settings_get(const Settings *s, const char *key)
{
    for (int i = 0; i < NSLOTS; i++)
        if (s->slots[i].key && strcmp(s->slots[i].key, key) == 0)
            return s->slots[i].value;
    return NULL;
}

/* Stores a copy of value under key. Returns 0, -1 on allocation failure
 * (old value kept), or -2 when every slot is taken. */
static int settings_set(Settings *s, const char *key, const char *value)
{
    char *copy = strdup(value);
    if (!copy)
        return -1;
    Setting *free_slot = NULL;
    for (int i = 0; i < NSLOTS; i++) {
        Setting *e = &s->slots[i];
        if (e->key && strcmp(e->key, key) == 0) {
            free(e->value);
            e->value = copy;
            return 0;
        }
        if (!e->key && !free_slot)
            free_slot = e;
    }
    if (!free_slot) {
        free(copy);
        return -2;
    }
    free_slot->key = key;
    free_slot->value = copy;
    return 0;
}

static void settings_clear(Settings *s)
{
    for (int i = 0; i < NSLOTS; i++) {
        free(s->slots[i].value);
        s->slots[i].key = NULL;
        s->slots[i].value = NULL;
    }
}

int main(void)
{
    Settings s = {0};
    if (settings_set(&s, "theme", "dark") != 0)
        goto oom;
    const char *before = settings_get(&s, "theme");
    if (settings_set(&s, "theme", "light") != 0)
        goto oom;
    const char *after = settings_get(&s, "theme");
    check(strcmp(after, "light") == 0, "new value is stored");
    check(strcmp(before, "dark") == 0, "we remember what the theme was");
    settings_clear(&s);
    return checks_done("m12-e03-uaf");
oom:
    settings_clear(&s);
    fprintf(stderr, "m12-e03: out of memory\n");
    return EXIT_OOM;
}
