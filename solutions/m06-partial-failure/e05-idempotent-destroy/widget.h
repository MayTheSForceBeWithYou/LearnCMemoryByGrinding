#ifndef WIDGET_H
#define WIDGET_H

#include <stddef.h>

struct widget {
    char *label;           /* owned copy */
    double *samples;       /* nsamples, zeroed */
    char *slots[3];        /* three heap buffers of slot_size bytes, zeroed */
    size_t nsamples, slot_size;
};

/* New widget; caller owns it (widget_destroy). NULL on allocation failure, nothing leaked.
 * Built into a zeroed struct; on any failure it calls widget_destroy on the partial widget. */
struct widget *widget_create(const char *label, size_t nsamples, size_t slot_size);
/* Frees w and everything it owns. Accepts NULL and any partially built widget. */
void widget_destroy(struct widget *w);

#endif
