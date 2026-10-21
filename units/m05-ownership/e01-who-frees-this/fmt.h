#ifndef FMT_H
#define FMT_H

#include <stddef.h>

const char *fmt_int(int n);
char       *fmt_into(char *buf, size_t cap, int n);
char       *fmt_hex_new(unsigned n);
const char *fmt_version(void);

#endif
