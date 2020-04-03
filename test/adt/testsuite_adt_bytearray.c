/*****************************************************************************
* \file      testsuite_adt_bytearray.c
* \author    Conny Gustafsson
* \date      2017-01-27
* \brief     Unit Tests for adt_bytearray_t
*
* Copyright (c) 2017-2019 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "adt_bytearray.h"
#include "adt_bytes.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define ARRAY_GROW_LEN 128

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_adt_bytearray_new(CuTest* tc);
static void test_adt_bytearray_resize(CuTest* tc);
static void test_adt_bytearray_make(CuTest* tc);
static void test_adt_bytearray_make_cstr(CuTest* tc);
static void test_adt_bytearray_equals(CuTest* tc);
static void test_adt_bytearray_manual_grow(CuTest* tc);
static void test_adt_bytearray_manual_shrink(CuTest* tc);
static void test_adt_bytearray_bytes(CuTest* tc);
static void test_adt_bytearray_bytearray_clone(CuTest* tc);


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
   SUITE_ADD_TEST(suite, test_adt_bytearray_manual_grow);
   SUITE_ADD_TEST(suite, test_adt_bytearray_manual_shrink);
   SUITE_ADD_TEST(suite, test_adt_bytearray_bytes);
   SUITE_ADD_TEST(suite, test_adt_bytearray_bytearray_clone);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void test_adt_bytearray_new(CuTest* tc)
{
   adt_bytearray_t *pArray = adt_bytearray_new(ADT_BYTE_ARRAY_DEFAULT_GROW_SIZE);
   CuAssertPtrNotNull(tc, pArray);
   CuAssertPtrEquals(tc, 0,pArray->pData);
   CuAssertIntEquals(tc, 0,pArray->u32CurLen);
   CuAssertIntEquals(tc, 0,pArray->u32AllocLen);
   CuAssertIntEquals(tc, ADT_BYTE_ARRAY_DEFAULT_GROW_SIZE,pArray->u32GrowSize);
   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_resize(CuTest* tc)
{
   int8_t result;
   adt_bytearray_t *pArray = adt_bytearray_new(4096);
   result = adt_bytearray_resize(pArray, 1132);
   CuAssertIntEquals(tc,0,result);
   CuAssertIntEquals(tc,1132,pArray->u32CurLen);
   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_make(CuTest* tc)
{
   adt_bytearray_t *pArray;
   const uint8_t data[] = {100,240,127,0,5};
   pArray = adt_bytearray_make(data, 5, ADT_BYTE_ARRAY_DEFAULT_GROW_SIZE);
   CuAssertPtrNotNull(tc, pArray);
   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_make_cstr(CuTest* tc)
{
   adt_bytearray_t *pArray;
   const char* cstr= "Test1";
   pArray = adt_bytearray_make_cstr(cstr, ADT_BYTE_ARRAY_NO_GROWTH);
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
   const uint8_t data1[] = {100,240,127,0,5};
   const uint8_t data2[] = {100,240,128,0,5};
   const uint8_t data3[] = {100,240,127};
   pArray1 = adt_bytearray_make(data1, 5, ARRAY_GROW_LEN);
   pArray2 = adt_bytearray_make(data1, 5, ARRAY_GROW_LEN);
   pArray3 = adt_bytearray_make(data2, 5, ARRAY_GROW_LEN);
   pArray4 = adt_bytearray_make(data3, 3, ARRAY_GROW_LEN);
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

static void test_adt_bytearray_manual_grow(CuTest* tc)
{
   const uint8_t data[] = {100, 240, 127, 0, 5};
   adt_bytearray_t *pArray = adt_bytearray_new(ADT_BYTE_ARRAY_NO_GROWTH);
   CuAssertIntEquals(tc, 0u, pArray->u32CurLen);
   CuAssertIntEquals(tc, 0u, pArray->u32AllocLen);
   CuAssertIntEquals(tc, 0u, pArray->u32GrowSize);
   adt_bytearray_append(pArray, &data[0], (uint32_t) sizeof(data));
   CuAssertIntEquals(tc, 5u, pArray->u32CurLen);
   CuAssertIntEquals(tc, 5u, pArray->u32AllocLen);
   CuAssertIntEquals(tc, 0, memcmp(&data[0], adt_bytearray_data(pArray), 5u));
   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_manual_shrink(CuTest* tc)
{
   const uint8_t data[] = {100, 240, 127, 0, 5};
   adt_bytearray_t *pArray = adt_bytearray_new(ADT_BYTE_ARRAY_NO_GROWTH);
   adt_bytearray_append(pArray, &data[0], (uint32_t) sizeof(data));
   CuAssertIntEquals(tc, 5u, pArray->u32CurLen);
   CuAssertIntEquals(tc, 5u, pArray->u32AllocLen);
   CuAssertIntEquals(tc, 0, memcmp(&data[0], adt_bytearray_data(pArray), 5u));
   adt_bytearray_resize(pArray, 2u);
   CuAssertIntEquals(tc, 2u, pArray->u32CurLen);
   CuAssertIntEquals(tc, 2u, pArray->u32AllocLen);
   CuAssertIntEquals(tc, 0, memcmp(&data[0], adt_bytearray_data(pArray), 2u));
   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_bytes(CuTest* tc)
{
   adt_bytearray_t *array = adt_bytearray_new(128u);
   adt_bytearray_push(array, 17);
   adt_bytearray_push(array, 255);
   adt_bytearray_push(array, 93);
   adt_bytearray_push(array, 0);
   adt_bytes_t *bytes = adt_bytearray_bytes(array);
   CuAssertPtrNotNull(tc, bytes);
   CuAssertUIntEquals(tc, 4, adt_bytes_length(bytes));
   const uint8_t *data = adt_bytes_constData(bytes);
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
   adt_bytearray_t *array1 = adt_bytearray_new(16u);
   adt_bytearray_push(array1, 1);
   adt_bytearray_push(array1, 2);
   adt_bytearray_push(array1, 3);
   adt_bytearray_push(array1, 4);
   adt_bytearray_push(array1, 5);
   adt_bytearray_t *array2 = adt_bytearray_clone(array1, ADT_BYTEARRAY_NO_GROWTH);
   CuAssertPtrNotNull(tc, array2);
   CuAssertUIntEquals(tc, 5, adt_bytearray_length(array2));
   CuAssertTrue(tc, adt_bytearray_equals(array1, array2));

   adt_bytearray_delete(array1);
   adt_bytearray_delete(array2);

}

