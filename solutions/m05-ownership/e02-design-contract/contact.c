#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contact.h"

struct contact *contact_create(const char *name, const char *email)
{
    struct contact *c = calloc(1, sizeof *c);     /* every pointer starts NULL */
    if (c == NULL)
        return NULL;
    c->name = strdup(name);
    c->email = strdup(email);
    if (c->name == NULL || c->email == NULL) {
        contact_destroy(c);                       /* safe on a half-built contact */
        return NULL;
    }
    return c;
}

void contact_destroy(struct contact *c)
{
    if (c == NULL)
        return;
    for (size_t i = 0; i < c->ntags; i++)
        free(c->tags[i]);
    free(c->tags);
    free(c->email);
    free(c->name);
    free(c);
}

int contact_add_tag(struct contact *c, const char *tag)
{
    char *copy = strdup(tag);
    if (copy == NULL)
        return -1;
    if (c->ntags == c->cap) {
        size_t nc = c->cap ? c->cap * 2 : 4;
        if (nc > SIZE_MAX / sizeof *c->tags) {
            free(copy);
            return -1;
        }
        char **tmp = realloc(c->tags, nc * sizeof *tmp);
        if (tmp == NULL) {
            free(copy);
            return -1;
        }
        c->tags = tmp;
        c->cap = nc;
    }
    c->tags[c->ntags++] = copy;
    return 0;
}

const char *contact_name(const struct contact *c)
{
    return c->name;
}

int contact_set_email(struct contact *c, const char *email)
{
    char *copy = strdup(email);
    if (copy == NULL)
        return -1;
    free(c->email);
    c->email = copy;
    return 0;
}

char *contact_to_string(const struct contact *c)
{
    size_t len = strlen(c->name) + 2 + strlen(c->email) + 3 + 1;   /* " <", "> [", "]" */
    for (size_t i = 0; i < c->ntags; i++)
        len += strlen(c->tags[i]) + (i > 0);                     /* commas between tags */
    char *s = malloc(len + 1);
    if (s == NULL)
        return NULL;
    char *p = s;
    p += sprintf(p, "%s <%s> [", c->name, c->email);
    for (size_t i = 0; i < c->ntags; i++)
        p += sprintf(p, "%s%s", i ? "," : "", c->tags[i]);
    *p++ = ']';
    *p = '\0';
    return s;
}
