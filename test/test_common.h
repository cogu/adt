#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "CuTest.h"
#include <stdlib.h>
#include <string.h>

#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

#ifdef _MSC_VER
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

void vfree(void *arg);

#endif /* TEST_COMMON_H */
