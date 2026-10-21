#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "contact.h"

static int oom(struct contact *c, char *s)
{
    free(s);
    contact_destroy(c);
    fprintf(stderr, "m05-e02-design-contract: out of memory\n");
    return EXIT_OOM;
}

int main(void)
{
    char *name = strdup("Ada Lovelace");
    if (name == NULL)
        return oom(NULL, NULL);
    struct contact *c = contact_create(name, "ada@example.org");
    if (c == NULL)
        return oom(NULL, name);
    free(name);                             /* create copied it */
    const char *n = contact_name(c);
    check(strcmp(n, "Ada Lovelace") == 0, "name copied");

    char tag[16];
    for (int i = 0; i < 20; i++) {
        snprintf(tag, sizeof tag, "t%d", i);
        if (contact_add_tag(c, tag) != 0)
            return oom(c, NULL);
    }
    check(c->ntags == 20 && strcmp(c->tags[19], "t19") == 0, "tags copied and grown");
    check(n == contact_name(c) && strcmp(n, "Ada Lovelace") == 0, "name pointer stable across add_tag");

    if (contact_set_email(c, "countess@example.org") != 0)
        return oom(c, NULL);
    check(strcmp(c->email, "countess@example.org") == 0, "email replaced");

    struct contact *d = contact_create("Bo", "bo@b.io");
    if (d == NULL)
        return oom(c, NULL);
    if (contact_add_tag(d, "x") != 0 || contact_add_tag(d, "yz") != 0) {
        contact_destroy(d);
        return oom(c, NULL);
    }
    char *s = contact_to_string(d);
    if (s == NULL) {
        contact_destroy(d);
        return oom(c, NULL);
    }
    check(strcmp(s, "Bo <bo@b.io> [x,yz]") == 0, "to_string format");
    free(s);
    contact_destroy(d);

    struct contact *e = contact_create("Cy", "c@y");
    if (e == NULL)
        return oom(c, NULL);
    s = contact_to_string(e);
    if (s == NULL) {
        contact_destroy(e);
        return oom(c, NULL);
    }
    check(strcmp(s, "Cy <c@y> []") == 0, "to_string with no tags");
    free(s);
    contact_destroy(e);

    contact_destroy(c);
    contact_destroy(NULL);
    return checks_done("m05-e02-design-contract");
}
