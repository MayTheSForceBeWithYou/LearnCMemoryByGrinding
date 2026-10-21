/* M07 e06: write the four path functions here.
 * Until you do, the build fails at link time with "undefined reference". */
#define _POSIX_C_SOURCE 200809L   /* strdup/strndup are POSIX, not ISO C */
#include <stdlib.h>
#include <string.h>
#include "path.h"
