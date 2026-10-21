#ifndef ROSTER_H
#define ROSTER_H

#include <stddef.h>

/* These contracts are correct. The code disagrees with them in two places. */

struct roster;

/* New empty roster; caller owns it (roster_destroy). NULL on OOM. */
struct roster *roster_create(void);
/* Adds a player. name is borrowed for the call only: the roster stores its own copy,
 * and the caller may free or reuse name immediately. 0, or -1 on OOM (roster unchanged). */
int roster_add(struct roster *r, const char *name);
/* Number of players. */
size_t roster_size(const struct roster *r);
/* Borrowed: owned by the roster, valid until roster_destroy. Do not free. */
const char *roster_name(const struct roster *r, size_t i);
/* Frees the roster and every name it owns. Accepts NULL. */
void roster_destroy(struct roster *r);

#endif
