#ifndef PAIR_H
#define PAIR_H

struct pair { char *key; char *value; };

/* TODO: contract (who owns *out, what happens to line, what happens on failure) */
int  parse_pair(const char *line, struct pair **out);

/* TODO: contract */
void pair_destroy(struct pair *p);

#endif
