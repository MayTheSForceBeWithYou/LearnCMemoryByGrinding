#ifndef CONTACT_H
#define CONTACT_H

#include <stddef.h>

/* TODO: write the ownership contract above every declaration (see README). */

struct contact {
    char *name;
    char *email;
    char **tags;
    size_t ntags, cap;
};

struct contact *contact_create(const char *name, const char *email);
void            contact_destroy(struct contact *c);
int             contact_add_tag(struct contact *c, const char *tag);
const char     *contact_name(const struct contact *c);
int             contact_set_email(struct contact *c, const char *email);
char           *contact_to_string(const struct contact *c);

#endif
