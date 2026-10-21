/* gridload.c: your implementation of gridload.h goes here.
 * Read with fgetc/fgets/fread into buffers you allocate yourself. Do not use getline,
 * getdelim or asprintf: the fault-injection build cannot see their allocations. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gridload.h"
