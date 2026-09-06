/*****************************************************************************
* \file      testsuite_adt_error.c
* \author    Conny Gustafsson
* \date      2026-09-06
* \brief     Unit tests for adt_error
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_error.h"
#include "test_common.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_adt_error_str_known(CuTest *tc);
static void test_adt_error_str_unknown(CuTest *tc);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite *testsuite_adt_error(void)
{
   CuSuite *suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_error_str_known);
   SUITE_ADD_TEST(suite, test_adt_error_str_unknown);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void test_adt_error_str_known(CuTest *tc)
{
   CuAssertStrEquals(tc, "No error", adt_error_str(ADT_NO_ERROR));
   CuAssertStrEquals(tc, "Invalid argument", adt_error_str(ADT_INVALID_ARGUMENT_ERROR));
   CuAssertStrEquals(tc, "Out of memory", adt_error_str(ADT_MEM_ERROR));
   CuAssertStrEquals(tc, "Index out of bounds", adt_error_str(ADT_INDEX_OUT_OF_BOUNDS_ERROR));
   CuAssertStrEquals(tc, "Invalid length", adt_error_str(ADT_LENGTH_ERROR));
   CuAssertStrEquals(tc, "Array too large", adt_error_str(ADT_ARRAY_TOO_LARGE_ERROR));
   CuAssertStrEquals(tc, "Not implemented", adt_error_str(ADT_NOT_IMPLEMENTED_ERROR));
   CuAssertStrEquals(tc, "Unknown encoding", adt_error_str(ADT_UNKNOWN_ENCODING_ERROR));
   CuAssertStrEquals(tc, "Object compare error", adt_error_str(ADT_OBJECT_COMPARE_ERROR));
   CuAssertStrEquals(tc, "Buffer overflow", adt_error_str(ADT_OVERFLOW_ERROR));
   CuAssertStrEquals(tc, "Buffer underflow", adt_error_str(ADT_UNDERFLOW_ERROR));
   CuAssertStrEquals(tc, "Not found", adt_error_str(ADT_NOT_FOUND_ERROR));
   CuAssertStrEquals(tc, "Already exists", adt_error_str(ADT_ALREADY_EXISTS_ERROR));
}

static void test_adt_error_str_unknown(CuTest *tc)
{
   CuAssertStrEquals(tc, "Unknown error", adt_error_str((adt_error_t)-1));
   CuAssertStrEquals(tc, "Unknown error", adt_error_str((adt_error_t)99));
   CuAssertStrEquals(tc, "Unknown error", adt_error_str((adt_error_t)-50));
}
