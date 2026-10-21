#ifndef SPLIT_FIELDS_H
#define SPLIT_FIELDS_H
#include <stddef.h>

/* Splits line at every occurrence of sep into heap-allocated field strings.
 * "a,,b" gives "a", "", "b"; "" gives one empty field.
 * Returns a NULL-terminated array owned by the caller (free with free_fields)
 * and stores the number of fields in *count. On allocation failure returns
 * NULL, leaves *count unchanged, and has freed everything it allocated. */
char **split_fields(const char *line, char sep, size_t *count);

/* Frees an array returned by split_fields and every string in it. Accepts NULL. */
void free_fields(char **fields);

#endif
