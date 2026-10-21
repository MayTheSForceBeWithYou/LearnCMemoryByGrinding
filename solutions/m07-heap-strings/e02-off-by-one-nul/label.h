#ifndef LABEL_H
#define LABEL_H

/* "[Lv <level>] <name>!" as a new string; caller frees. NULL on OOM. level >= 0. */
char *label_make(const char *name, int level);
/* Same, with the name upper-cased. */
char *label_upper(const char *name, int level);

#endif
