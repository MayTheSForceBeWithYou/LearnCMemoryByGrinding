#ifndef NAMES_H
#define NAMES_H

const char *names_default(void);
char       *names_join(const char *a, const char *b);
const char *names_upper(const char *s);
const char *names_pick(const char *const *list, int i);

#endif
