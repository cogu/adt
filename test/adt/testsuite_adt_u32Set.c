/*****************************************************************************
* \file      testsuite_adt_u32Set.c
* \author    Conny Gustafsson
* \date      2018-07-27
* \brief     Unit tests for adt_u32Set_t
*
* Copyright (c) 2018-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#include <stdbool.h>
#include <assert.h>
#include <malloc.h>
#include "test_common.h"
#include "adt_set.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_adt_u32Set_create(CuTest* tc);
static void test_adt_u32Set_new_delete(CuTest* tc);
static void test_adt_u32Set_insert_in_empty_set(CuTest* tc);
static void test_adt_u32Set_insert_multiple_values(CuTest* tc);
static void test_adt_u32Set_insert_duplicates(CuTest* tc);
static void test_adt_u32Set_contains(CuTest* tc);
static void test_adt_u32Set_remove(CuTest* tc);
static void test_adt_u32Set_is_empty(CuTest* tc);
static void test_adt_u32Set_vdelete(CuTest* tc);
static void test_adt_u32Set_value_bounds(CuTest* tc);
static void test_adt_u32Set_large(CuTest* tc);
static void test_adt_u32Set_errors(CuTest* tc);

//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_adt_u32Set(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_u32Set_create);
   SUITE_ADD_TEST(suite, test_adt_u32Set_new_delete);
   SUITE_ADD_TEST(suite, test_adt_u32Set_insert_in_empty_set);
   SUITE_ADD_TEST(suite, test_adt_u32Set_insert_multiple_values);
   SUITE_ADD_TEST(suite, test_adt_u32Set_insert_duplicates);
   SUITE_ADD_TEST(suite, test_adt_u32Set_contains);
   SUITE_ADD_TEST(suite, test_adt_u32Set_remove);
   SUITE_ADD_TEST(suite, test_adt_u32Set_is_empty);
   SUITE_ADD_TEST(suite, test_adt_u32Set_vdelete);
   SUITE_ADD_TEST(suite, test_adt_u32Set_value_bounds);
   SUITE_ADD_TEST(suite, test_adt_u32Set_large);
   SUITE_ADD_TEST(suite, test_adt_u32Set_errors);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void test_adt_u32Set_create(CuTest* tc)
{
   adt_u32Set_t set;
   adt_u32Set_create(&set);
   CuAssertTrue(tc, adt_u32Set_is_empty(&set));
   CuAssertIntEquals(tc, 0, adt_u32Set_length(&set));
   CuAssertPtrEquals(tc, NULL, set.pAlloc);
   CuAssertIntEquals(tc, 0, set.s32AllocLen);
   CuAssertIntEquals(tc, 0, set.s32CurLen);
   adt_u32Set_destroy(&set);
}

static void test_adt_u32Set_insert_in_empty_set(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32Set_create(&set);

   adt_u32Set_insert(&set, 10);
   CuAssertIntEquals(tc, 1, adt_u32Set_length(&set));

   //cleanup
   adt_u32Set_destroy(&set);
}

static void test_adt_u32Set_new_delete(CuTest* tc)
{
   adt_u32Set_t *set = adt_u32Set_new();
   CuAssertPtrNotNull(tc, set);
   adt_u32Set_delete(set);
}

static void test_adt_u32Set_insert_multiple_values(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32Set_create(&set);

   //ordered insertion
   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 3);
   adt_u32Set_insert(&set, 4);
   adt_u32Set_insert(&set, 5);

   CuAssertIntEquals(tc, 5, adt_u32Set_length(&set));

   //ordered insertion (backwards)
   adt_u32Set_clear(&set);
   adt_u32Set_insert(&set, 5);
   adt_u32Set_insert(&set, 4);
   adt_u32Set_insert(&set, 3);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 1);

   CuAssertIntEquals(tc, 5, adt_u32Set_length(&set));
   //make sure set is sorted
   CuAssertUIntEquals(tc, 1, adt_u32Set_value(&set, 0));
   CuAssertUIntEquals(tc, 2, adt_u32Set_value(&set, 1));
   CuAssertUIntEquals(tc, 3, adt_u32Set_value(&set, 2));
   CuAssertUIntEquals(tc, 4, adt_u32Set_value(&set, 3));
   CuAssertUIntEquals(tc, 5, adt_u32Set_value(&set, 4));

   //unordered insertion
   adt_u32Set_clear(&set);
   adt_u32Set_insert(&set, 3);
   adt_u32Set_insert(&set, 5);
   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 4);
   adt_u32Set_insert(&set, 2);

   CuAssertIntEquals(tc, 5, adt_u32Set_length(&set));
   //make sure set is still sorted
   CuAssertUIntEquals(tc, 1, adt_u32Set_value(&set, 0));
   CuAssertUIntEquals(tc, 2, adt_u32Set_value(&set, 1));
   CuAssertUIntEquals(tc, 3, adt_u32Set_value(&set, 2));
   CuAssertUIntEquals(tc, 4, adt_u32Set_value(&set, 3));
   CuAssertUIntEquals(tc, 5, adt_u32Set_value(&set, 4));

   //cleanup
   adt_u32Set_destroy(&set);
}

static void test_adt_u32Set_insert_duplicates(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32Set_create(&set);

   adt_u32Set_insert(&set, 3);
   adt_u32Set_insert(&set, 5);
   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 4);
   adt_u32Set_insert(&set, 5);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 4);
   adt_u32Set_insert(&set, 3);
   adt_u32Set_insert(&set, 2);
   CuAssertIntEquals(tc, 5, adt_u32Set_length(&set));
   CuAssertUIntEquals(tc, 1, adt_u32Set_value(&set, 0));
   CuAssertUIntEquals(tc, 2, adt_u32Set_value(&set, 1));
   CuAssertUIntEquals(tc, 3, adt_u32Set_value(&set, 2));
   CuAssertUIntEquals(tc, 4, adt_u32Set_value(&set, 3));
   CuAssertUIntEquals(tc, 5, adt_u32Set_value(&set, 4));

   //cleanup
   adt_u32Set_destroy(&set);
}

static void test_adt_u32Set_contains(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32Set_create(&set);
   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 6);
   adt_u32Set_insert(&set, 5);
   adt_u32Set_insert(&set, 9);
   adt_u32Set_insert(&set, 11);
   adt_u32Set_insert(&set, 8);

   CuAssertTrue(tc, adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 3));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 4));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 5));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 6));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 7));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 8));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 9));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 10));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 11));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 12));

   //cleanup
   adt_u32Set_destroy(&set);

}

static void test_adt_u32Set_remove(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32Set_create(&set);
   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 3);

   CuAssertTrue(tc, adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 3));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 4));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 1));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 2));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 3));
   CuAssertTrue(tc, !adt_u32Set_remove(&set, 4));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 3));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 4));
   CuAssertIntEquals(tc, 0, adt_u32Set_length(&set));

   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 3);
   CuAssertTrue(tc, adt_u32Set_remove(&set, 2));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 3));

   adt_u32Set_insert(&set, 0xFFFFFFFF);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 4);
   CuAssertTrue(tc, adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 3));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 4));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 0xFFFFFFFF));

   CuAssertTrue(tc, adt_u32Set_remove(&set, 1));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 3));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 4));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 0xFFFFFFFF));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 3));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 4));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 0xFFFFFFFF));


   //cleanup
   adt_u32Set_destroy(&set);

}

static void test_adt_u32Set_is_empty(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32Set_create(&set);
   CuAssertTrue(tc, adt_u32Set_is_empty(&set));
   adt_u32Set_insert(&set, 1);
   CuAssertTrue(tc, !adt_u32Set_is_empty(&set));

   //cleanup
   adt_u32Set_destroy(&set);

}

static void test_adt_u32Set_vdelete(CuTest* tc)
{
   adt_u32Set_t *set = adt_u32Set_new();
   CuAssertPtrNotNull(tc, set);
   adt_u32Set_insert(set, 123);
   adt_u32Set_vdelete((void*) set);
}

static void test_adt_u32Set_value_bounds(CuTest* tc)
{
   adt_u32Set_t set;
   adt_u32Set_create(&set);

   CuAssertUIntEquals(tc, 0, adt_u32Set_value(&set, 0));
   CuAssertUIntEquals(tc, 0, adt_u32Set_value(&set, -1));
   CuAssertUIntEquals(tc, 0, adt_u32Set_value(NULL, 0));

   adt_u32Set_insert(&set, 42);
   CuAssertUIntEquals(tc, 42, adt_u32Set_value(&set, 0));
   CuAssertUIntEquals(tc, 0, adt_u32Set_value(&set, 1));
   CuAssertUIntEquals(tc, 0, adt_u32Set_value(&set, -1));

   adt_u32Set_destroy(&set);
}

static void test_adt_u32Set_large(CuTest* tc)
{
   adt_u32Set_t set;
   adt_u32Set_create(&set);

   const int count = 500;
   uint32_t seed = 42;
   for (int i = 0; i < count; i++)
   {
      seed = seed * 1103515245 + 12345;
      uint32_t val = seed % 200;
      adt_u32Set_insert(&set, val);
   }

   int32_t len = adt_u32Set_length(&set);
   CuAssertTrue(tc, len > 0 && len <= 200);

   for (int32_t i = 0; i < len - 1; i++)
   {
      uint32_t a = adt_u32Set_value(&set, i);
      uint32_t b = adt_u32Set_value(&set, i + 1);
      CuAssertTrue(tc, a < b);
      CuAssertTrue(tc, adt_u32Set_contains(&set, a));
      CuAssertTrue(tc, adt_u32Set_contains(&set, b));
   }

   for (int32_t i = len - 1; i >= 0; i -= 2)
   {
      uint32_t val = adt_u32Set_value(&set, i);
      CuAssertTrue(tc, adt_u32Set_remove(&set, val));
      CuAssertTrue(tc, !adt_u32Set_contains(&set, val));
   }

   adt_u32Set_destroy(&set);
}

static void test_adt_u32Set_errors(CuTest* tc)
{
   adt_u32Set_t set;
   adt_u32Set_create(&set);

   // NULL self check
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_u32Set_insert(NULL, 10));

   // Normal insert returns ADT_NO_ERROR
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32Set_insert(&set, 10));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32Set_insert(&set, 20));

   // Duplicate insert returns ADT_NO_ERROR (idempotent)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32Set_insert(&set, 10));
   CuAssertIntEquals(tc, 2, adt_u32Set_length(&set));

   // Overflow test: artificially set s32CurLen to INT32_MAX
   set.s32CurLen = INT32_MAX;
   CuAssertIntEquals(tc, ADT_OVERFLOW_ERROR, adt_u32Set_insert(&set, 99));

   // Reset before destroy
   set.s32CurLen = 2;
   adt_u32Set_destroy(&set);
}

