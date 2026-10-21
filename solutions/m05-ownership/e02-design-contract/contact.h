#ifndef CONTACT_H
#define CONTACT_H

#include <stddef.h>

/* A contact owns every string it points to: name, email, each tags[i], and the tags array.
 * Fields are visible for testing; mutate only through the functions below. */
struct contact {
    char *name;
    char *email;
    char **tags;
    size_t ntags, cap;
};

/* Returns a new contact with copies of name and email (both borrowed for the call only;
 * the caller may free them immediately). Caller owns the result: contact_destroy().
 * NULL on allocation failure, with nothing leaked. */
struct contact *contact_create(const char *name, const char *email);

/* Frees c and everything it owns. Accepts NULL. Every pointer previously returned
 * by contact_name() is invalid afterwards. */
void            contact_destroy(struct contact *c);

/* Appends a copy of tag (borrowed for the call). 0 on success; -1 on allocation failure,
 * with c unchanged. Does not invalidate contact_name()'s result. */
int             contact_add_tag(struct contact *c, const char *tag);

/* Borrowed: owned by c, valid until contact_destroy(c). Do not free or modify. */
const char     *contact_name(const struct contact *c);

/* Replaces the email with a copy of email (borrowed). 0 on success; -1 on allocation
 * failure, with the old email still in place. */
int             contact_set_email(struct contact *c, const char *email);

/* Returns a new string "name <email> [t1,t2,...]" sized exactly. Caller frees with free().
 * NULL on allocation failure. */
char           *contact_to_string(const struct contact *c);

#endif
