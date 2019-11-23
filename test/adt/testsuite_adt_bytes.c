/*****************************************************************************
* \file      testsuite_adt_bytes.c
* \author    Conny Gustafsson
* \date      2019-08-13
* \brief     Unit tests for adt_bytes_t
*
* Copyright (c) 2019 Conny Gustafsson
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
#include "adt_bytes.h"
#include "adt_bytearray.h"
#include "CMemLeak.h"

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
static void test_adt_bytes_new(CuTest* tc);
static void test_adt_bytes_length(CuTest* tc);
static void test_adt_bytes_constData(CuTest* tc);
static void test_adt_bytes_bytearray(CuTest* tc);
static void test_adt_bytes_clone(CuTest* tc);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

CuSuite* testsuite_adt_bytes(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_bytes_new);
   SUITE_ADD_TEST(suite, test_adt_bytes_length);
   SUITE_ADD_TEST(suite, test_adt_bytes_constData);
   SUITE_ADD_TEST(suite, test_adt_bytes_bytearray);
   SUITE_ADD_TEST(suite, test_adt_bytes_clone);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void test_adt_bytes_new(CuTest* tc)
{
   uint8_t data[5] = {1, 2, 3, 4, 5};

   adt_bytes_t *bytes = adt_bytes_new(&data[0], sizeof(data));
   CuAssertPtrNotNull(tc, bytes);
   CuAssertUIntEquals(tc, 5, bytes->dataLen);
   CuAssertIntEquals(tc, 0, memcmp(&data[0], bytes->dataBuf, 5));
   adt_bytes_delete(bytes);
}

static void test_adt_bytes_length(CuTest* tc)
{
   uint8_t data[5] = {1, 2, 3, 4, 5};

   adt_bytes_t *bytes = adt_bytes_new(&data[0], sizeof(data));
   CuAssertPtrNotNull(tc, bytes);
   CuAssertUIntEquals(tc, bytes->dataLen, adt_bytes_length(bytes));

   adt_bytes_delete(bytes);
}



static void test_adt_bytes_constData(CuTest* tc)
{
   uint8_t data[5] = {1, 2, 3, 4, 5};

   adt_bytes_t *bytes = adt_bytes_new(&data[0], sizeof(data));
   const uint8_t *p;
   CuAssertPtrNotNull(tc, bytes);
   p = adt_bytes_constData(bytes);
   CuAssertPtrNotNull(tc, p);
   CuAssertIntEquals(tc, 0, memcmp(p, &data[0], sizeof(data)));
   adt_bytes_delete(bytes);
}


static void test_adt_bytes_bytearray(CuTest* tc)
{
   uint8_t data[5] = {1, 2, 3, 4, 5};

   adt_bytes_t *bytes = adt_bytes_new(&data[0], sizeof(data));
   CuAssertPtrNotNull(tc, bytes);
   adt_bytearray_t *array = adt_bytes_bytearray(bytes, ADT_BYTE_ARRAY_NO_GROWTH);
   CuAssertPtrNotNull(tc, array);
   CuAssertUIntEquals(tc, 5, adt_bytearray_length(array));
   CuAssertIntEquals(tc, 0, memcmp(&data[0], adt_bytearray_data(array), 5));
   adt_bytearray_delete(array);
   adt_bytes_delete(bytes);
}

static void test_adt_bytes_clone(CuTest* tc)
{
   uint8_t data[5] = {1, 2, 3, 4, 5};
   adt_bytes_t *bytes1 = adt_bytes_new(&data[0], sizeof(data));
   CuAssertPtrNotNull(tc, bytes1);
   adt_bytes_t *bytes2 = adt_bytes_clone(bytes1);
   CuAssertPtrNotNull(tc, bytes2);
   CuAssertUIntEquals(tc, 5, adt_bytes_length(bytes2));
   CuAssertIntEquals(tc, 0, memcmp(&data[0], adt_bytes_constData(bytes2), 5));
   CuAssertTrue(tc, adt_bytes_equals(bytes1, bytes2));

   adt_bytes_delete(bytes1);
   adt_bytes_delete(bytes2);
}
