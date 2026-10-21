#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include "widget.h"

struct widget *widget_create(const char *label, size_t nsamples, size_t slot_size)
{
    struct widget *w = calloc(1, sizeof *w);    /* every owned pointer starts NULL */
    if (w == NULL)
        return NULL;
    w->nsamples = nsamples;
    w->slot_size = slot_size;
    if ((w->label = strdup(label)) == NULL)
        goto fail;
    if ((w->samples = calloc(nsamples, sizeof *w->samples)) == NULL)
        goto fail;
    for (size_t i = 0; i < 3; i++)
        if ((w->slots[i] = calloc(slot_size, 1)) == NULL)
            goto fail;
    return w;

fail:
    widget_destroy(w);
    return NULL;
}

/* Invariant: every owned pointer field is either NULL or a live block owned by w.
 * widget_create establishes it with calloc before the first allocation, and only ever
 * stores a successful result into a field, so destroy is correct at every stage. */
void widget_destroy(struct widget *w)
{
    if (w == NULL)
        return;
    for (size_t i = 3; i-- > 0;)
        free(w->slots[i]);
    free(w->samples);
    free(w->label);
    free(w);
}
