#ifndef RECORDS_H
#define RECORDS_H

#include <stddef.h>
#include <stdint.h>

struct record { int32_t id; int32_t value; };   /* 8 bytes on the wire, little-endian */

#define RECORDS_MAX 100000

/* Input: int32 count (little-endian), then count records.
 * Returns a heap array of count records plus one zeroed sentinel (caller frees),
 * and stores count in *out_n. NULL on bad input or allocation failure. */
struct record *records_parse(const unsigned char *buf, size_t len, size_t *out_n);

#endif
