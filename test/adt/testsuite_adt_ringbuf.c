/*****************************************************************************
* \file      testsuite_adt_ringbuf.c
* \author    Conny Gustafsson
* \date      2018-10-17
* \brief     Unit tests for adt_ringbuf
*
* Copyright (c) 2018-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <limits.h>
#include "test_common.h"
#include "adt_ringbuf.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
#if (ADT_RBFH_ENABLE)
static void test_adt_rbfh_nextLen(CuTest* tc);
static void test_adt_rbfh_insert_then_grow(CuTest* tc);
static void test_adt_rbfh_insert_then_grow2(CuTest* tc);
static void test_adt_rbfh_insert_then_remove(CuTest* tc);
static void test_adt_rbfh_peek(CuTest* tc);
static void test_adt_rbfh_limits(CuTest* tc);
static void test_adt_rbfh_new_delete(CuTest* tc);
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_adt_ringbuf(void)
{
   CuSuite* suite = CuSuiteNew();

#if (ADT_RBFH_ENABLE)
   SUITE_ADD_TEST(suite, test_adt_rbfh_nextLen);
   SUITE_ADD_TEST(suite, test_adt_rbfh_insert_then_grow);
   SUITE_ADD_TEST(suite, test_adt_rbfh_insert_then_grow2);
   SUITE_ADD_TEST(suite, test_adt_rbfh_insert_then_remove);
   SUITE_ADD_TEST(suite, test_adt_rbfh_peek);
   SUITE_ADD_TEST(suite, test_adt_rbfh_limits);
   SUITE_ADD_TEST(suite, test_adt_rbfh_new_delete);
#endif

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
#if (ADT_RBFH_ENABLE)
static void test_adt_rbfh_nextLen(CuTest* tc)
{
   CuAssertUIntEquals(tc, 10u, adt_rbfh_nextLen(1));
   CuAssertUIntEquals(tc, 10u, adt_rbfh_nextLen(5));
   CuAssertUIntEquals(tc, 10u, adt_rbfh_nextLen(10));
   CuAssertUIntEquals(tc, 50u, adt_rbfh_nextLen(11));
   CuAssertUIntEquals(tc, 50u, adt_rbfh_nextLen(45));
   CuAssertUIntEquals(tc, 50u, adt_rbfh_nextLen(50));
   CuAssertUIntEquals(tc, 100u, adt_rbfh_nextLen(51));
   CuAssertUIntEquals(tc, 500u, adt_rbfh_nextLen(101));
   CuAssertUIntEquals(tc, 1000u, adt_rbfh_nextLen(501));
   CuAssertUIntEquals(tc, 5000u, adt_rbfh_nextLen(1001));
   CuAssertUIntEquals(tc, 10000u, adt_rbfh_nextLen(5001));
   CuAssertUIntEquals(tc, 50000u, adt_rbfh_nextLen(10001));
   CuAssertUIntEquals(tc, 65535u, adt_rbfh_nextLen(50001));
}

static void test_adt_rbfh_insert_then_grow(CuTest* tc)
{
   adt_rbfh_t buf;
   uint32_t value;
   adt_rbfh_create_ex(&buf, sizeof(uint32_t), 5, 0);
   CuAssertPtrNotNull(tc, buf.u8AllocBuf);
   CuAssertUIntEquals(tc, 5, buf.u16AllocLen);
   CuAssertUIntEquals(tc, 0, adt_rbfh_length(&buf));
   value = 1;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 2;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 3;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 4;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 5;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 5, adt_rbfh_length(&buf));
   //grow the buffer
   value = 6;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 10, buf.u16AllocLen);
   CuAssertUIntEquals(tc, 6, adt_rbfh_length(&buf));

   adt_rbfh_destroy(&buf);
}

static void test_adt_rbfh_insert_then_grow2(CuTest* tc)
{
   adt_rbfh_t buf;
   uint32_t value;
   adt_rbfh_create_ex(&buf, sizeof(uint32_t), 5, 0);
   CuAssertPtrNotNull(tc, buf.u8AllocBuf);
   CuAssertUIntEquals(tc, 5, buf.u16AllocLen);
   CuAssertUIntEquals(tc, 0, adt_rbfh_length(&buf));
   value = 1;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 2;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 3;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 4;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 5;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 5, adt_rbfh_length(&buf));
   //Remove two items
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 1, value);
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 2, value);
   CuAssertUIntEquals(tc, 3, adt_rbfh_length(&buf));
   //Add two new items
   value = 6;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 7;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 5, adt_rbfh_length(&buf));
   CuAssertUIntEquals(tc, 5, buf.u16AllocLen);
   //Adding another item should grow the buffer
   value = 8;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 10, buf.u16AllocLen);
   CuAssertUIntEquals(tc, 6, adt_rbfh_length(&buf));

   adt_rbfh_destroy(&buf);
}

static void test_adt_rbfh_insert_then_remove(CuTest* tc)
{
   adt_rbfh_t buf;
   uint32_t inValue;
   uint32_t outValue;
   adt_rbfh_create_ex(&buf, sizeof(uint32_t), 5, 0);
   CuAssertPtrNotNull(tc, buf.u8AllocBuf);
   CuAssertUIntEquals(tc, 0, adt_rbfh_length(&buf));
   inValue = 1;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 2;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 3;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 4;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 5;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 6;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   CuAssertUIntEquals(tc, 6, adt_rbfh_length(&buf));
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 1, outValue);
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 2, outValue);
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 3, outValue);
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 4, outValue);
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 5, outValue);
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 6, outValue);
   CuAssertUIntEquals(tc, BUF_E_UNDERFLOW, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   adt_rbfh_destroy(&buf);
}

static void test_adt_rbfh_peek(CuTest* tc)
{
   adt_rbfh_t buf;
   uint32_t inValue;
   uint32_t outValue;
   adt_rbfh_create_ex(&buf, sizeof(uint32_t), 0, 0);

   inValue = 1;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 2;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 3;
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   CuAssertUIntEquals(tc, 10, buf.u16AllocLen);

   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_peek(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 1, outValue);
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_peek(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 2, outValue);
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_peek(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 3, outValue);
   CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, BUF_E_UNDERFLOW, adt_rbfh_peek(&buf, (uint8_t*) &outValue));

   adt_rbfh_destroy(&buf);
}

static void test_adt_rbfh_limits(CuTest* tc)
{
   adt_rbfh_t buf;
   uint32_t i;
   uint32_t outValue;
   uint32_t expectedSize = 0;
   adt_rbfh_create(&buf, sizeof(uint32_t));
   CuAssertUIntEquals(tc, 0, adt_rbfh_length(&buf));
   CuAssertUIntEquals(tc, USHRT_MAX, adt_rbfh_free(&buf));
   for (i=0;i<USHRT_MAX;i++)
   {
      CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_insert(&buf, (uint8_t*) &i));
      CuAssertUIntEquals(tc, ++expectedSize, adt_rbfh_length(&buf));
   }
   CuAssertUIntEquals(tc, BUF_E_OVERFLOW, adt_rbfh_insert(&buf, (uint8_t*) &i));
   CuAssertUIntEquals(tc, USHRT_MAX, buf.u16AllocLen);
   CuAssertUIntEquals(tc, 0, adt_rbfh_free(&buf));

   for (i=0;i<USHRT_MAX;i++)
   {
      CuAssertUIntEquals(tc, BUF_E_OK, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
      CuAssertUIntEquals(tc, i, outValue);
      CuAssertUIntEquals(tc, --expectedSize, adt_rbfh_length(&buf));
   }
   CuAssertUIntEquals(tc, BUF_E_UNDERFLOW, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, USHRT_MAX, adt_rbfh_free(&buf));

   adt_rbfh_destroy(&buf);
}

static void test_adt_rbfh_new_delete(CuTest* tc)
{
   adt_rbfh_t *rbf = adt_rbfh_new(sizeof(uint32_t));
   CuAssertPtrNotNull(tc, rbf);

   uint32_t val = 42, out = 0;
   CuAssertIntEquals(tc, BUF_E_OK, adt_rbfh_insert(rbf, (const uint8_t*)&val));
   CuAssertIntEquals(tc, BUF_E_OK, adt_rbfh_remove(rbf, (uint8_t*)&out));
   CuAssertUIntEquals(tc, 42, out);

   adt_rbfh_delete(rbf);

   adt_rbfh_t *rbfEx = adt_rbfh_new_ex(sizeof(uint16_t), 10, 100);
   CuAssertPtrNotNull(tc, rbfEx);
   adt_rbfh_delete(rbfEx);
}
#endif
