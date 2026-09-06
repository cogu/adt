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
static void test_adt_ringbuf_aliases(CuTest* tc);
static void test_adt_rbfs_basic(CuTest* tc);
static void test_adt_rbfu16_basic(CuTest* tc);
#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
static void test_adt_rbfh_nextLen(CuTest* tc);
static void test_adt_rbfh_insert_then_grow(CuTest* tc);
static void test_adt_rbfh_insert_then_grow2(CuTest* tc);
static void test_adt_rbfh_insert_then_remove(CuTest* tc);
static void test_adt_rbfh_peek(CuTest* tc);
static void test_adt_rbfh_limits(CuTest* tc);
static void test_adt_rbfh_new_delete(CuTest* tc);
static void test_adt_rbfh_invalid_args(CuTest* tc);
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

   SUITE_ADD_TEST(suite, test_adt_ringbuf_aliases);
   SUITE_ADD_TEST(suite, test_adt_rbfs_basic);
   SUITE_ADD_TEST(suite, test_adt_rbfu16_basic);
#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
   SUITE_ADD_TEST(suite, test_adt_rbfh_nextLen);
   SUITE_ADD_TEST(suite, test_adt_rbfh_insert_then_grow);
   SUITE_ADD_TEST(suite, test_adt_rbfh_insert_then_grow2);
   SUITE_ADD_TEST(suite, test_adt_rbfh_insert_then_remove);
   SUITE_ADD_TEST(suite, test_adt_rbfh_peek);
   SUITE_ADD_TEST(suite, test_adt_rbfh_limits);
   SUITE_ADD_TEST(suite, test_adt_rbfh_new_delete);
   SUITE_ADD_TEST(suite, test_adt_rbfh_invalid_args);
#endif

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void test_adt_ringbuf_aliases(CuTest* tc)
{
   CuAssertIntEquals(tc, ADT_NO_ERROR, BUF_E_OK);
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, BUF_E_NOT_OK);
   CuAssertIntEquals(tc, ADT_OVERFLOW_ERROR, BUF_E_OVERFLOW);
   CuAssertIntEquals(tc, ADT_UNDERFLOW_ERROR, BUF_E_UNDERFLOW);
}

static void test_adt_rbfs_basic(CuTest* tc)
{
   uint32_t raw_buf[5];
   adt_rbfs_t rbf;

   // Invalid argument checks
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfs_create(NULL, (uint8_t*)raw_buf, 5, sizeof(uint32_t)));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfs_create(&rbf, NULL, 5, sizeof(uint32_t)));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfs_create(&rbf, (uint8_t*)raw_buf, 0, sizeof(uint32_t)));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfs_create(&rbf, (uint8_t*)raw_buf, 5, 0));

   adt_error_t res = adt_rbfs_create(&rbf, (uint8_t*)raw_buf, 5, sizeof(uint32_t));
   CuAssertIntEquals(tc, ADT_NO_ERROR, res);
   CuAssertUIntEquals(tc, 0, adt_rbfs_size(&rbf));
   CuAssertUIntEquals(tc, 5, adt_rbfs_free(&rbf));

   // Underflow on empty
   uint32_t val = 0;
   CuAssertIntEquals(tc, ADT_UNDERFLOW_ERROR, adt_rbfs_remove(&rbf, (uint8_t*)&val));
   CuAssertIntEquals(tc, ADT_UNDERFLOW_ERROR, adt_rbfs_peek(&rbf, (uint8_t*)&val));

   // NULL argument handling on operations
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfs_insert(NULL, (const uint8_t*)&val));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfs_insert(&rbf, NULL));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfs_remove(NULL, (uint8_t*)&val));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfs_remove(&rbf, NULL));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfs_peek(NULL, (uint8_t*)&val));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfs_peek(&rbf, NULL));

   // Fill buffer (1..5)
   for (uint32_t i = 1; i <= 5; i++) {
      CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfs_insert(&rbf, (const uint8_t*)&i));
   }
   CuAssertUIntEquals(tc, 5, adt_rbfs_size(&rbf));
   CuAssertUIntEquals(tc, 0, adt_rbfs_free(&rbf));

   // Overflow
   val = 99;
   CuAssertIntEquals(tc, ADT_OVERFLOW_ERROR, adt_rbfs_insert(&rbf, (const uint8_t*)&val));

   // Peek
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfs_peek(&rbf, (uint8_t*)&val));
   CuAssertUIntEquals(tc, 1, val);

   // Remove 2 elements (1, 2)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfs_remove(&rbf, (uint8_t*)&val));
   CuAssertUIntEquals(tc, 1, val);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfs_remove(&rbf, (uint8_t*)&val));
   CuAssertUIntEquals(tc, 2, val);
   CuAssertUIntEquals(tc, 3, adt_rbfs_size(&rbf));
   CuAssertUIntEquals(tc, 2, adt_rbfs_free(&rbf));

   // Insert 2 elements (6, 7) - triggers wrap-around
   val = 6;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfs_insert(&rbf, (const uint8_t*)&val));
   val = 7;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfs_insert(&rbf, (const uint8_t*)&val));
   CuAssertUIntEquals(tc, 5, adt_rbfs_size(&rbf));

   // Remove remaining elements: should be 3, 4, 5, 6, 7
   uint32_t expected[] = {3, 4, 5, 6, 7};
   for (int i = 0; i < 5; i++) {
      CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfs_remove(&rbf, (uint8_t*)&val));
      CuAssertUIntEquals(tc, expected[i], val);
   }
   CuAssertUIntEquals(tc, 0, adt_rbfs_size(&rbf));
   CuAssertIntEquals(tc, ADT_UNDERFLOW_ERROR, adt_rbfs_remove(&rbf, (uint8_t*)&val));

   // Test clear
   val = 100;
   adt_rbfs_insert(&rbf, (const uint8_t*)&val);
   CuAssertUIntEquals(tc, 1, adt_rbfs_size(&rbf));
   adt_rbfs_clear(&rbf);
   CuAssertUIntEquals(tc, 0, adt_rbfs_size(&rbf));
}

static void test_adt_rbfu16_basic(CuTest* tc)
{
   uint16_t raw_buf[5];
   adt_rbfu16_t rbf;

   // Error handling on invalid create
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfu16_create(NULL, raw_buf, 5));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfu16_create(&rbf, NULL, 5));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfu16_create(&rbf, raw_buf, 0));

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfu16_create(&rbf, raw_buf, 5));
   CuAssertUIntEquals(tc, 0, adt_rbfu16_length(&rbf));

   // Underflow
   uint16_t val = 0;
   CuAssertIntEquals(tc, ADT_UNDERFLOW_ERROR, adt_rbfu16_remove(&rbf, &val));
   CuAssertIntEquals(tc, ADT_UNDERFLOW_ERROR, adt_rbfu16_peek(&rbf, &val));

   // NULL argument handling
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfu16_insert(NULL, 10));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfu16_remove(NULL, &val));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfu16_remove(&rbf, NULL));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfu16_peek(NULL, &val));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfu16_peek(&rbf, NULL));

   // Insert 5 elements (10, 20, 30, 40, 50)
   for (uint16_t i = 1; i <= 5; i++) {
      CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfu16_insert(&rbf, (uint16_t)(i * 10)));
   }
   CuAssertUIntEquals(tc, 5, adt_rbfu16_length(&rbf));

   // Overflow
   CuAssertIntEquals(tc, ADT_OVERFLOW_ERROR, adt_rbfu16_insert(&rbf, 60));

   // Peek
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfu16_peek(&rbf, &val));
   CuAssertUIntEquals(tc, 10, val);

   // Remove 2 elements (10, 20)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfu16_remove(&rbf, &val));
   CuAssertUIntEquals(tc, 10, val);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfu16_remove(&rbf, &val));
   CuAssertUIntEquals(tc, 20, val);
   CuAssertUIntEquals(tc, 3, adt_rbfu16_length(&rbf));

   // Insert 2 elements (60, 70) - wrap around
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfu16_insert(&rbf, 60));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfu16_insert(&rbf, 70));
   CuAssertUIntEquals(tc, 5, adt_rbfu16_length(&rbf));

   // Read back remaining: 30, 40, 50, 60, 70
   uint16_t expected[] = {30, 40, 50, 60, 70};
   for (int i = 0; i < 5; i++) {
      CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfu16_remove(&rbf, &val));
      CuAssertUIntEquals(tc, expected[i], val);
   }
   CuAssertUIntEquals(tc, 0, adt_rbfu16_length(&rbf));
   CuAssertIntEquals(tc, ADT_UNDERFLOW_ERROR, adt_rbfu16_remove(&rbf, &val));
}

#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
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
   adt_rbfh_create_with_params(&buf, sizeof(uint32_t), 5, 0);
   CuAssertPtrNotNull(tc, buf.u8AllocBuf);
   CuAssertUIntEquals(tc, 5, buf.u16AllocLen);
   CuAssertUIntEquals(tc, 0, adt_rbfh_length(&buf));
   value = 1;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 2;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 3;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 4;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 5;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 5, adt_rbfh_length(&buf));
   //grow the buffer
   value = 6;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 10, buf.u16AllocLen);
   CuAssertUIntEquals(tc, 6, adt_rbfh_length(&buf));

   adt_rbfh_destroy(&buf);
}

static void test_adt_rbfh_insert_then_grow2(CuTest* tc)
{
   adt_rbfh_t buf;
   uint32_t value;
   adt_rbfh_create_with_params(&buf, sizeof(uint32_t), 5, 0);
   CuAssertPtrNotNull(tc, buf.u8AllocBuf);
   CuAssertUIntEquals(tc, 5, buf.u16AllocLen);
   CuAssertUIntEquals(tc, 0, adt_rbfh_length(&buf));
   value = 1;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 2;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 3;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 4;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 5;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 5, adt_rbfh_length(&buf));
   //Remove two items
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 1, value);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 2, value);
   CuAssertUIntEquals(tc, 3, adt_rbfh_length(&buf));
   //Add two new items
   value = 6;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   value = 7;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 5, adt_rbfh_length(&buf));
   CuAssertUIntEquals(tc, 5, buf.u16AllocLen);
   //Adding another item should grow the buffer
   value = 8;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &value));
   CuAssertUIntEquals(tc, 10, buf.u16AllocLen);
   CuAssertUIntEquals(tc, 6, adt_rbfh_length(&buf));

   adt_rbfh_destroy(&buf);
}

static void test_adt_rbfh_insert_then_remove(CuTest* tc)
{
   adt_rbfh_t buf;
   uint32_t inValue;
   uint32_t outValue;
   adt_rbfh_create_with_params(&buf, sizeof(uint32_t), 5, 0);
   CuAssertPtrNotNull(tc, buf.u8AllocBuf);
   CuAssertUIntEquals(tc, 0, adt_rbfh_length(&buf));
   inValue = 1;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 2;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 3;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 4;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 5;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 6;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   CuAssertUIntEquals(tc, 6, adt_rbfh_length(&buf));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 1, outValue);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 2, outValue);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 3, outValue);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 4, outValue);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 5, outValue);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 6, outValue);
   CuAssertIntEquals(tc, ADT_UNDERFLOW_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   adt_rbfh_destroy(&buf);
}

static void test_adt_rbfh_peek(CuTest* tc)
{
   adt_rbfh_t buf;
   uint32_t inValue;
   uint32_t outValue;
   adt_rbfh_create_with_params(&buf, sizeof(uint32_t), 0, 0);

   inValue = 1;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 2;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   inValue = 3;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &inValue));
   CuAssertUIntEquals(tc, 10, buf.u16AllocLen);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_peek(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 1, outValue);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_peek(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 2, outValue);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_peek(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, 3, outValue);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertIntEquals(tc, ADT_UNDERFLOW_ERROR, adt_rbfh_peek(&buf, (uint8_t*) &outValue));

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
      CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &i));
      CuAssertUIntEquals(tc, ++expectedSize, adt_rbfh_length(&buf));
   }
   CuAssertIntEquals(tc, ADT_OVERFLOW_ERROR, adt_rbfh_insert(&buf, (uint8_t*) &i));
   CuAssertUIntEquals(tc, USHRT_MAX, buf.u16AllocLen);
   CuAssertUIntEquals(tc, 0, adt_rbfh_free(&buf));

   for (i=0;i<USHRT_MAX;i++)
   {
      CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
      CuAssertUIntEquals(tc, i, outValue);
      CuAssertUIntEquals(tc, --expectedSize, adt_rbfh_length(&buf));
   }
   CuAssertIntEquals(tc, ADT_UNDERFLOW_ERROR, adt_rbfh_remove(&buf, (uint8_t*) &outValue));
   CuAssertUIntEquals(tc, USHRT_MAX, adt_rbfh_free(&buf));

   adt_rbfh_destroy(&buf);
}

static void test_adt_rbfh_new_delete(CuTest* tc)
{
   adt_rbfh_t *rbf = adt_rbfh_new(sizeof(uint32_t));
   CuAssertPtrNotNull(tc, rbf);

   uint32_t val = 42, out = 0;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_insert(rbf, (const uint8_t*)&val));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_remove(rbf, (uint8_t*)&out));
   CuAssertUIntEquals(tc, 42, out);

   adt_rbfh_delete(rbf);

   adt_rbfh_t *rbf_with_params = adt_rbfh_new_with_params(sizeof(uint16_t), 10, 100);
   CuAssertPtrNotNull(tc, rbf_with_params);
   adt_rbfh_delete(rbf_with_params);
}

static void test_adt_rbfh_invalid_args(CuTest* tc)
{
   adt_rbfh_t buf;
   uint32_t val = 42;

   // create with invalid params
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfh_create(NULL, sizeof(uint32_t)));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfh_create(&buf, 0));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfh_create_with_params(NULL, sizeof(uint32_t), 5, 10));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfh_create_with_params(&buf, 0, 5, 10));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfh_create_with_params(&buf, sizeof(uint32_t), 10, 5));

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_rbfh_create(&buf, sizeof(uint32_t)));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfh_insert(NULL, (const uint8_t*)&val));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfh_insert(&buf, NULL));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfh_remove(NULL, (uint8_t*)&val));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfh_remove(&buf, NULL));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfh_peek(NULL, (uint8_t*)&val));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_rbfh_peek(&buf, NULL));

   adt_rbfh_destroy(&buf);
}
#endif
