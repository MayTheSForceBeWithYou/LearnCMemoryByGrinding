#ifndef STRVEC_H
#define STRVEC_H

#include <stdbool.h>
#include <stddef.h>

/* TODO: the ownership contract for the type and for every function below. */

typedef struct {
    char  **data;
    size_t  len;
    size_t  cap;
} StrVec;

bool        strvec_push_copy(StrVec *v, const char *s);
bool        strvec_push_take(StrVec *v, char *s);
char       *strvec_pop(StrVec *v);
const char *strvec_get(const StrVec *v, size_t i);
void        strvec_destroy(StrVec *v);

#endif
