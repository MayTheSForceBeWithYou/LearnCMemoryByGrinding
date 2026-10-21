#ifndef PAIR_H
#define PAIR_H

struct pair { char *key; char *value; };

/* Parses "key=value" (key non-empty; value may be empty). line is borrowed for the call.
 * On success returns 0 and stores a new pair in *out; the caller owns it (pair_destroy).
 * On malformed input or allocation failure returns -1, leaves *out unchanged, leaks nothing. */
int  parse_pair(const char *line, struct pair **out);

/* Frees p, its key and its value. Accepts NULL. */
void pair_destroy(struct pair *p);

#endif
