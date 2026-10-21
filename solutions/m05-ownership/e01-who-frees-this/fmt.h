#ifndef FMT_H
#define FMT_H

#include <stddef.h>

/* Static: one buffer, overwritten by the next fmt_int call. Never free. */
const char *fmt_int(int n);
/* Caller's storage: writes into buf (cap bytes, truncating) and returns buf. Nothing allocated. */
char       *fmt_into(char *buf, size_t cap, int n);
/* Owned: new "0x..." string; caller frees. NULL on OOM. */
char       *fmt_hex_new(unsigned n);
/* Literal: valid forever, never free. */
const char *fmt_version(void);

#endif
