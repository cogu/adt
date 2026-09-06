/*****************************************************************************
* \file      testsuite_adt_bytearray.c
* \author    Conny Gustafsson
* \date      2017-01-27
* \brief     Unit tests for adt_bytearray_t
*
* Copyright (c) 2017-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "test_common.h"
#include "adt_bytearray.h"
#include "adt_bytes.h"
#include "adt_ary.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_adt_bytearray_new(CuTest* tc);
static void test_adt_bytearray_resize(CuTest* tc);
static void test_adt_bytearray_make(CuTest* tc);
static void test_adt_bytearray_make_cstr(CuTest* tc);
static void test_adt_bytearray_equals(CuTest* tc);
static void test_adt_bytearray_geometric_growth(CuTest* tc);
static void test_adt_bytearray_set_growth_size(CuTest* tc);
static void test_adt_bytearray_manual_shrink(CuTest* tc);
static void test_adt_bytearray_bytes(CuTest* tc);
static void test_adt_bytearray_bytearray_clone(CuTest* tc);
static void test_adt_bytearray_trim_left(CuTest* tc);
static void test_adt_bytearray_trim_left_bounds(CuTest* tc);
static void test_adt_bytearray_vdelete(CuTest* tc);


//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_adt_bytearray(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_bytearray_new);
   SUITE_ADD_TEST(suite, test_adt_bytearray_resize);
   SUITE_ADD_TEST(suite, test_adt_bytearray_make);
   SUITE_ADD_TEST(suite, test_adt_bytearray_make_cstr);
   SUITE_ADD_TEST(suite, test_adt_bytearray_equals);
   SUITE_ADD_TEST(suite, test_adt_bytearray_geometric_growth);
   SUITE_ADD_TEST(suite, test_adt_bytearray_set_growth_size);
   SUITE_ADD_TEST(suite, test_adt_bytearray_manual_shrink);
   SUITE_ADD_TEST(suite, test_adt_bytearray_bytes);
   SUITE_ADD_TEST(suite, test_adt_bytearray_bytearray_clone);
   SUITE_ADD_TEST(suite, test_adt_bytearray_trim_left);
   SUITE_ADD_TEST(suite, test_adt_bytearray_trim_left_bounds);
   SUITE_ADD_TEST(suite, test_adt_bytearray_vdelete);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void test_adt_bytearray_new(CuTest* tc)
{
   adt_bytearray_t *pArray = adt_bytearray_new();
   CuAssertPtrNotNull(tc, pArray);
   CuAssertPtrEquals(tc, NULL, pArray->pData);
   CuAssertIntEquals(tc, 0, pArray->u32CurLen);
   CuAssertIntEquals(tc, 0, pArray->u32AllocLen);
   CuAssertIntEquals(tc, 0u, pArray->u32GrowSize);
   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_resize(CuTest* tc)
{
   adt_bytearray_t *pArray = adt_bytearray_new();
   adt_error_t result = adt_bytearray_resize(pArray, 1132);
   CuAssertIntEquals(tc, ADT_NO_ERROR, result);
   CuAssertIntEquals(tc, 1132, pArray->u32CurLen);
   CuAssertTrue(tc, pArray->u32AllocLen >= 1132);
   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_make(CuTest* tc)
{
   adt_bytearray_t *pArray;
   const uint8_t data[] = {100, 240, 127, 0, 5};
   pArray = adt_bytearray_make(data, 5);
   CuAssertPtrNotNull(tc, pArray);
   CuAssertIntEquals(tc, 5, pArray->u32CurLen);
   CuAssertIntEquals(tc, 0, memcmp(data, pArray->pData, 5));
   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_make_cstr(CuTest* tc)
{
   adt_bytearray_t *pArray;
   const char* cstr = "Test1";
   pArray = adt_bytearray_make_cstr(cstr);
   CuAssertPtrNotNull(tc, pArray);
   CuAssertIntEquals(tc, 5, pArray->u32CurLen);
   CuAssertIntEquals(tc, 0, memcmp(cstr, pArray->pData, 5));
   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_equals(CuTest* tc)
{
   adt_bytearray_t *pArray1;
   adt_bytearray_t *pArray2;
   adt_bytearray_t *pArray3;
   adt_bytearray_t *pArray4;
   const uint8_t data1[] = {100, 240, 127, 0, 5};
   const uint8_t data2[] = {100, 240, 128, 0, 5};
   const uint8_t data3[] = {100, 240, 127};
   pArray1 = adt_bytearray_make(data1, 5);
   pArray2 = adt_bytearray_make(data1, 5);
   pArray3 = adt_bytearray_make(data2, 5);
   pArray4 = adt_bytearray_make(data3, 3);
   CuAssertPtrNotNull(tc, pArray1);
   CuAssertPtrNotNull(tc, pArray2);
   CuAssertPtrNotNull(tc, pArray3);
   CuAssertPtrNotNull(tc, pArray4);
   CuAssertTrue(tc, adt_bytearray_equals(pArray1, pArray2) == true);
   CuAssertTrue(tc, adt_bytearray_equals(pArray1, pArray3) == false);
   CuAssertTrue(tc, adt_bytearray_equals(pArray1, pArray4) == false);
   adt_bytearray_delete(pArray1);
   adt_bytearray_delete(pArray2);
   adt_bytearray_delete(pArray3);
   adt_bytearray_delete(pArray4);
}

static void test_adt_bytearray_geometric_growth(CuTest* tc)
{
   adt_bytearray_t *pArray = adt_bytearray_new();
   CuAssertPtrNotNull(tc, pArray);
   CuAssertIntEquals(tc, 0u, pArray->u32AllocLen);

   // Pushing 1st byte triggers base capacity (16 bytes)
   adt_error_t err = adt_bytearray_push(pArray, 0xAA);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertIntEquals(tc, 1u, pArray->u32CurLen);
   CuAssertIntEquals(tc, 16u, pArray->u32AllocLen);

   // Fill up to 16 bytes
   for (uint8_t i = 1; i < 16; i++)
   {
      err = adt_bytearray_push(pArray, i);
      CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   }
   CuAssertIntEquals(tc, 16u, pArray->u32CurLen);
   CuAssertIntEquals(tc, 16u, pArray->u32AllocLen);

   // Pushing 17th byte triggers doubling to 32
   err = adt_bytearray_push(pArray, 0xFF);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertIntEquals(tc, 17u, pArray->u32CurLen);
   CuAssertIntEquals(tc, 32u, pArray->u32AllocLen);

   // Reserving 100 bytes triggers doubling: 32 -> 64 -> 128
   err = adt_bytearray_reserve(pArray, 100);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertIntEquals(tc, 128u, pArray->u32AllocLen);
   CuAssertIntEquals(tc, 17u, pArray->u32CurLen);

   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_set_growth_size(CuTest* tc)
{
   adt_bytearray_t *pArray = adt_bytearray_new();
   adt_bytearray_set_growth_size(pArray, 64u);
   CuAssertIntEquals(tc, 64u, pArray->u32GrowSize);

   adt_error_t err = adt_bytearray_push(pArray, 1);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertIntEquals(tc, 64u, pArray->u32AllocLen);

   // Revert to geometric growth by setting to 0
   adt_bytearray_set_growth_size(pArray, 0u);
   CuAssertIntEquals(tc, 0u, pArray->u32GrowSize);

   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_manual_shrink(CuTest* tc)
{
   const uint8_t data[] = {100, 240, 127, 0, 5};
   adt_bytearray_t *pArray = adt_bytearray_new();
   adt_bytearray_append(pArray, &data[0], (uint32_t) sizeof(data));
   CuAssertIntEquals(tc, 5u, pArray->u32CurLen);
   CuAssertIntEquals(tc, 16u, pArray->u32AllocLen);
   CuAssertIntEquals(tc, 0, memcmp(&data[0], adt_bytearray_data(pArray), 5u));
   adt_bytearray_resize(pArray, 2u);
   CuAssertIntEquals(tc, 2u, pArray->u32CurLen);
   CuAssertIntEquals(tc, 16u, pArray->u32AllocLen);
   CuAssertIntEquals(tc, 0, memcmp(&data[0], adt_bytearray_data(pArray), 2u));
   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_bytes(CuTest* tc)
{
   adt_bytearray_t *array = adt_bytearray_new();
   adt_bytearray_push(array, 17);
   adt_bytearray_push(array, 255);
   adt_bytearray_push(array, 93);
   adt_bytearray_push(array, 0);
   adt_bytes_t *bytes = adt_bytearray_bytes(array);
   CuAssertPtrNotNull(tc, bytes);
   CuAssertUIntEquals(tc, 4, adt_bytes_length(bytes));
   const uint8_t *data = adt_bytes_const_data(bytes);
   CuAssertIntEquals(tc, 17, data[0]);
   CuAssertIntEquals(tc, 255, data[1]);
   CuAssertIntEquals(tc, 93, data[2]);
   CuAssertIntEquals(tc, 0, data[3]);
   CuAssertPtrNotNull(tc, data);

   adt_bytes_delete(bytes);
   adt_bytearray_delete(array);
}

static void test_adt_bytearray_bytearray_clone(CuTest* tc)
{
   adt_bytearray_t *array1 = adt_bytearray_new();
   adt_bytearray_push(array1, 1);
   adt_bytearray_push(array1, 2);
   adt_bytearray_push(array1, 3);
   adt_bytearray_push(array1, 4);
   adt_bytearray_push(array1, 5);
   adt_bytearray_t *array2 = adt_bytearray_clone(array1);
   CuAssertPtrNotNull(tc, array2);
   CuAssertUIntEquals(tc, 5, adt_bytearray_length(array2));
   CuAssertTrue(tc, adt_bytearray_equals(array1, array2));

   adt_bytearray_delete(array1);
   adt_bytearray_delete(array2);
}

static void test_adt_bytearray_trim_left(CuTest* tc)
{
   const char *orig = "Hello, World!";
   adt_bytearray_t *array = adt_bytearray_make_cstr(orig);
   CuAssertPtrNotNull(tc, array);
   CuAssertUIntEquals(tc, 13, adt_bytearray_length(array));

   // Trim at start (no shift)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_bytearray_trim_left(array, adt_bytearray_data(array)));
   CuAssertUIntEquals(tc, 13, adt_bytearray_length(array));

   // Trim 7 bytes from the left ("World!")
   uint8_t *pMid = adt_bytearray_data(array) + 7;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_bytearray_trim_left(array, pMid));
   CuAssertUIntEquals(tc, 6, adt_bytearray_length(array));
   CuAssertIntEquals(tc, 0, memcmp(adt_bytearray_data(array), "World!", 6));

   // Trim all remaining bytes
   uint8_t *pEnd = adt_bytearray_data(array) + 6;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_bytearray_trim_left(array, pEnd));
   CuAssertUIntEquals(tc, 0, adt_bytearray_length(array));

   adt_bytearray_delete(array);
}

static void test_adt_bytearray_trim_left_bounds(CuTest* tc)
{
   const char *orig = "Hello, World!";
   adt_bytearray_t *array = adt_bytearray_make_cstr(orig);
   CuAssertPtrNotNull(tc, array);
   CuAssertUIntEquals(tc, 13, adt_bytearray_length(array));

   uint8_t *pData = adt_bytearray_data(array);

   // Pointer before array start -> invalid argument error
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_bytearray_trim_left(array, pData - 1));

   // Pointer beyond valid length -> invalid argument error
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_bytearray_trim_left(array, pData + 14));

   // NULL pointers -> invalid argument error
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_bytearray_trim_left(array, NULL));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_bytearray_trim_left(NULL, pData));

   // Verify array contents remain unmodified after failed boundary calls
   CuAssertUIntEquals(tc, 13, adt_bytearray_length(array));
   CuAssertIntEquals(tc, 0, memcmp(pData, orig, 13));

   adt_bytearray_delete(array);
}

static void test_adt_bytearray_vdelete(CuTest* tc)
{
   adt_ary_t *outer = adt_ary_new(adt_bytearray_vdelete);
   adt_bytearray_t *inner1 = adt_bytearray_make_cstr("inner1_bytes");
   adt_bytearray_t *inner2 = adt_bytearray_make_cstr("inner2_bytes");

   adt_ary_push(outer, inner1);
   adt_ary_push(outer, inner2);
   CuAssertIntEquals(tc, 2, adt_ary_length(outer));

   // adt_ary_delete will invoke adt_bytearray_vdelete on inner1 and inner2
   adt_ary_delete(outer);
}

