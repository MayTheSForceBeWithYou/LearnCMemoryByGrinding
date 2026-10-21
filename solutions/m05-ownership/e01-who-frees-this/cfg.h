#ifndef CFG_H
#define CFG_H

struct cfg;

/* Returns a new config (copies path). Caller owns it: cfg_free(). NULL on OOM. */
struct cfg *cfg_new(const char *path);
/* Frees c and every string it owns. Accepts NULL. */
void        cfg_free(struct cfg *c);
/* Borrowed: owned by c, valid until cfg_reload(c) or cfg_free(c). Do not free. NULL if no such key. */
const char *cfg_get(const struct cfg *c, const char *key);
/* Owned: a new copy of the value; caller frees. NULL if no such key or on OOM. */
char       *cfg_dup(const struct cfg *c, const char *key);
/* Borrowed: owned by c, valid until cfg_free(c) (reload does not touch it). */
const char *cfg_path(const struct cfg *c);
/* Rebuilds all values: invalidates every pointer returned by cfg_get. 0, or -1 on OOM (old values kept). */
int         cfg_reload(struct cfg *c);

#endif
