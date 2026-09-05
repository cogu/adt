/*****************************************************************************
* \file      test_common.h
* \author    Conny Gustafsson
* \date      2026-08-31
* \brief     Common test definitions and utilities
*
* Copyright (c) 2026-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "CuTest.h"
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

void vfree(void *arg);

#endif /* TEST_COMMON_H */
