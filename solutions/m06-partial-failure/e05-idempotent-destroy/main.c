#include <string.h>
#include "course.h"
#include "widget.h"

int main(void)
{
    struct widget *a = widget_create("gauge", 16, 32);
    struct widget *b = a ? widget_create("dial", 4, 8) : NULL;
    if (a == NULL || b == NULL) {
        widget_destroy(a);
        fprintf(stderr, "m06-e05-idempotent-destroy: out of memory\n");
        return EXIT_OOM;
    }
    check(strcmp(a->label, "gauge") == 0 && a->nsamples == 16 && a->slot_size == 32, "a fields");
    check(a->samples[15] == 0.0 && a->slots[2][31] == 0, "a zeroed");
    memcpy(b->slots[0], "1234567", 8);
    check(strcmp(b->slots[0], "1234567") == 0 && b->slots[1] != b->slots[2], "b slots usable and distinct");
    widget_destroy(b);
    widget_destroy(a);
    widget_destroy(NULL);
    return checks_done("m06-e05-idempotent-destroy");
}
