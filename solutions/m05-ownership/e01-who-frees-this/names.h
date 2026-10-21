#ifndef NAMES_H
#define NAMES_H

/* Literal: static storage, valid forever, read-only, never free. */
const char *names_default(void);
/* Owned: new "a b" string; caller frees. a and b are borrowed for the call. NULL on OOM. */
char       *names_join(const char *a, const char *b);
/* Static: one buffer in names.c (31 chars max), overwritten by the next call. Never free. */
const char *names_upper(const char *s);
/* Borrowed from the caller: returns list[i] itself; valid as long as the caller's string is. */
const char *names_pick(const char *const *list, int i);

#endif
