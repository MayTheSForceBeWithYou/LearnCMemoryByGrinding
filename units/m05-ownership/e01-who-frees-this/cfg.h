#ifndef CFG_H
#define CFG_H

struct cfg;

struct cfg *cfg_new(const char *path);
void        cfg_free(struct cfg *c);
const char *cfg_get(const struct cfg *c, const char *key);
char       *cfg_dup(const struct cfg *c, const char *key);
const char *cfg_path(const struct cfg *c);
int         cfg_reload(struct cfg *c);

#endif
