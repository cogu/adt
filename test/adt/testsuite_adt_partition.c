/*****************************************************************************
* \file      testsuite_adt_partition.c
* \author    Conny Gustafsson
* \date      2026-10-03
* \brief     Unit tests for adt_u16_partition_t and adt_u32_partition_t
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "test_common.h"
#include "adt_partition.h"
#ifdef MEM_LEAK_CHECK
# include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_adt_u16_partition_create(CuTest* tc);
static void test_adt_u16_partition_new_delete(CuTest* tc);
static void test_adt_u16_partition_vdelete(CuTest* tc);
static void test_adt_u16_partition_append_and_find(CuTest* tc);
static void test_adt_u16_partition_get_segment(CuTest* tc);
static void test_adt_u16_partition_reserve(CuTest* tc);
static void test_adt_u16_partition_clear(CuTest* tc);
static void test_adt_u16_partition_overflow(CuTest* tc);
static void test_adt_u16_partition_simulated_large_node(CuTest* tc);

static void test_adt_u32_partition_create(CuTest* tc);
static void test_adt_u32_partition_new_delete(CuTest* tc);
static void test_adt_u32_partition_vdelete(CuTest* tc);
static void test_adt_u32_partition_append_and_find(CuTest* tc);
static void test_adt_u32_partition_get_segment(CuTest* tc);
static void test_adt_u32_partition_large_offsets(CuTest* tc);
static void test_adt_u32_partition_reserve_and_clear(CuTest* tc);

//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_adt_partition(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_u16_partition_create);
   SUITE_ADD_TEST(suite, test_adt_u16_partition_new_delete);
   SUITE_ADD_TEST(suite, test_adt_u16_partition_vdelete);
   SUITE_ADD_TEST(suite, test_adt_u16_partition_append_and_find);
   SUITE_ADD_TEST(suite, test_adt_u16_partition_get_segment);
   SUITE_ADD_TEST(suite, test_adt_u16_partition_reserve);
   SUITE_ADD_TEST(suite, test_adt_u16_partition_clear);
   SUITE_ADD_TEST(suite, test_adt_u16_partition_overflow);
   SUITE_ADD_TEST(suite, test_adt_u16_partition_simulated_large_node);

   SUITE_ADD_TEST(suite, test_adt_u32_partition_create);
   SUITE_ADD_TEST(suite, test_adt_u32_partition_new_delete);
   SUITE_ADD_TEST(suite, test_adt_u32_partition_vdelete);
   SUITE_ADD_TEST(suite, test_adt_u32_partition_append_and_find);
   SUITE_ADD_TEST(suite, test_adt_u32_partition_get_segment);
   SUITE_ADD_TEST(suite, test_adt_u32_partition_large_offsets);
   SUITE_ADD_TEST(suite, test_adt_u32_partition_reserve_and_clear);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS: adt_u16_partition_t
//////////////////////////////////////////////////////////////////////////////

static void test_adt_u16_partition_create(CuTest* tc)
{
   adt_u16_partition_t part;
   adt_u16_partition_create(&part);

   CuAssertTrue(tc, adt_u16_partition_is_empty(&part));
   CuAssertUIntEquals(tc, 0u, adt_u16_partition_length(&part));
   CuAssertUIntEquals(tc, 0u, adt_u16_partition_total_size(&part));
   CuAssertIntEquals(tc, -1, adt_u16_partition_find(&part, 0u));
   CuAssertIntEquals(tc, -1, adt_u16_partition_find(&part, 10u));

   adt_u16_partition_destroy(&part);
}

static void test_adt_u16_partition_new_delete(CuTest* tc)
{
   adt_u16_partition_t *part = adt_u16_partition_new();
   CuAssertPtrNotNull(tc, part);
   CuAssertTrue(tc, adt_u16_partition_is_empty(part));
   CuAssertUIntEquals(tc, 0u, adt_u16_partition_length(part));

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(part, 16u));
   CuAssertFalse(tc, adt_u16_partition_is_empty(part));
   CuAssertUIntEquals(tc, 1u, adt_u16_partition_length(part));
   CuAssertUIntEquals(tc, 16u, adt_u16_partition_total_size(part));

   adt_u16_partition_delete(part);
}

static void test_adt_u16_partition_vdelete(CuTest* tc)
{
   adt_u16_partition_t *part = adt_u16_partition_new();
   CuAssertPtrNotNull(tc, part);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(part, 8u));
   adt_u16_partition_vdelete((void*)part);
}

static void test_adt_u16_partition_append_and_find(CuTest* tc)
{
   adt_u16_partition_t part;
   adt_u16_partition_create(&part);

   /* Create 3 contiguous segments with lengths: (4, 2, 1) -> total size: 7 */
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 4u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 2u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 1u));

   CuAssertUIntEquals(tc, 3u, adt_u16_partition_length(&part));
   CuAssertUIntEquals(tc, 7u, adt_u16_partition_total_size(&part));

   /* Segment 0: [0, 4) */
   CuAssertIntEquals(tc, 0, adt_u16_partition_find(&part, 0u));
   CuAssertIntEquals(tc, 0, adt_u16_partition_find(&part, 1u));
   CuAssertIntEquals(tc, 0, adt_u16_partition_find(&part, 2u));
   CuAssertIntEquals(tc, 0, adt_u16_partition_find(&part, 3u));

   /* Segment 1: [4, 6) */
   CuAssertIntEquals(tc, 1, adt_u16_partition_find(&part, 4u));
   CuAssertIntEquals(tc, 1, adt_u16_partition_find(&part, 5u));

   /* Segment 2: [6, 7) */
   CuAssertIntEquals(tc, 2, adt_u16_partition_find(&part, 6u));

   /* Out of bounds checks: offset >= 7 */
   CuAssertIntEquals(tc, -1, adt_u16_partition_find(&part, 7u));
   CuAssertIntEquals(tc, -1, adt_u16_partition_find(&part, 8u));
   CuAssertIntEquals(tc, -1, adt_u16_partition_find(&part, 1000u));

   adt_u16_partition_destroy(&part);
}

static void test_adt_u16_partition_get_segment(CuTest* tc)
{
   adt_u16_partition_t part;
   adt_u16_partition_create(&part);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 4u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 2u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 1u));

   uint16_t start = 0u;
   uint16_t length = 0u;

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_get_segment(&part, 0u, &start, &length));
   CuAssertUIntEquals(tc, 0u, start);
   CuAssertUIntEquals(tc, 4u, length);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_get_segment(&part, 1u, &start, &length));
   CuAssertUIntEquals(tc, 4u, start);
   CuAssertUIntEquals(tc, 2u, length);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_get_segment(&part, 2u, &start, &length));
   CuAssertUIntEquals(tc, 6u, start);
   CuAssertUIntEquals(tc, 1u, length);

   /* Out of bounds index */
   CuAssertIntEquals(tc, ADT_INDEX_OUT_OF_BOUNDS_ERROR, adt_u16_partition_get_segment(&part, 3u, &start, &length));

   adt_u16_partition_destroy(&part);
}

static void test_adt_u16_partition_reserve(CuTest* tc)
{
   adt_u16_partition_t part;
   adt_u16_partition_create(&part);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_reserve(&part, 100u));
   CuAssertUIntEquals(tc, 0u, adt_u16_partition_length(&part));

   uint32_t i;
   for (i = 0u; i < 100u; i++)
   {
      CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 1u));
   }
   CuAssertUIntEquals(tc, 100u, adt_u16_partition_length(&part));
   CuAssertUIntEquals(tc, 100u, adt_u16_partition_total_size(&part));

   for (i = 0u; i < 100u; i++)
   {
      CuAssertIntEquals(tc, (int32_t)i, adt_u16_partition_find(&part, (uint16_t)i));
   }
   CuAssertIntEquals(tc, -1, adt_u16_partition_find(&part, 100u));

   adt_u16_partition_destroy(&part);
}

static void test_adt_u16_partition_clear(CuTest* tc)
{
   adt_u16_partition_t part;
   adt_u16_partition_create(&part);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 10u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 20u));
   CuAssertUIntEquals(tc, 2u, adt_u16_partition_length(&part));
   CuAssertUIntEquals(tc, 30u, adt_u16_partition_total_size(&part));

   adt_u16_partition_clear(&part);
   CuAssertTrue(tc, adt_u16_partition_is_empty(&part));
   CuAssertUIntEquals(tc, 0u, adt_u16_partition_length(&part));
   CuAssertUIntEquals(tc, 0u, adt_u16_partition_total_size(&part));
   CuAssertIntEquals(tc, -1, adt_u16_partition_find(&part, 0u));

   /* Re-append after clear */
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 5u));
   CuAssertUIntEquals(tc, 1u, adt_u16_partition_length(&part));
   CuAssertUIntEquals(tc, 5u, adt_u16_partition_total_size(&part));
   CuAssertIntEquals(tc, 0, adt_u16_partition_find(&part, 4u));
   CuAssertIntEquals(tc, -1, adt_u16_partition_find(&part, 5u));

   adt_u16_partition_destroy(&part);
}

static void test_adt_u16_partition_overflow(CuTest* tc)
{
   adt_u16_partition_t part;
   adt_u16_partition_create(&part);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 60000u));
   /* 60000 + 10000 = 70000 > UINT16_MAX (65535) -> OVERFLOW */
   CuAssertIntEquals(tc, ADT_OVERFLOW_ERROR, adt_u16_partition_append(&part, 10000u));
   CuAssertUIntEquals(tc, 1u, adt_u16_partition_length(&part));
   CuAssertUIntEquals(tc, 60000u, adt_u16_partition_total_size(&part));

   /* 60000 + 5535 = 65535 (exact UINT16_MAX limit) */
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, 5535u));
   CuAssertUIntEquals(tc, 2u, adt_u16_partition_length(&part));
   CuAssertUIntEquals(tc, 65535u, adt_u16_partition_total_size(&part));

   CuAssertIntEquals(tc, 1, adt_u16_partition_find(&part, 65534u));
   CuAssertIntEquals(tc, -1, adt_u16_partition_find(&part, 65535u));

   adt_u16_partition_destroy(&part);
}

static void test_adt_u16_partition_simulated_large_node(CuTest* tc)
{
   adt_u16_partition_t part;
   adt_u16_partition_create(&part);

   /* Simulate 1223 ports with repeating sizes 1, 2, 4, 1, 8 bytes */
   const uint16_t sizes[] = {1u, 2u, 4u, 1u, 8u};
   const uint32_t num_ports = 1223u;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_reserve(&part, num_ports));

   uint32_t i;
   for (i = 0u; i < num_ports; i++)
   {
      uint16_t sz = sizes[i % 5u];
      CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_partition_append(&part, sz));
   }
   CuAssertUIntEquals(tc, num_ports, adt_u16_partition_length(&part));

   /* Verify every single byte offset from 0 to total_size - 1 maps to correct port */
   uint16_t total = adt_u16_partition_total_size(&part);
   uint16_t current_offset = 0u;
   for (i = 0u; i < num_ports; i++)
   {
      uint16_t sz = sizes[i % 5u];
      uint16_t byte_idx;
      for (byte_idx = 0u; byte_idx < sz; byte_idx++)
      {
         int32_t found = adt_u16_partition_find(&part, current_offset + byte_idx);
         CuAssertIntEquals(tc, (int32_t)i, found);
      }
      current_offset += sz;
   }
   CuAssertUIntEquals(tc, total, current_offset);
   CuAssertIntEquals(tc, -1, adt_u16_partition_find(&part, total));

   adt_u16_partition_destroy(&part);
}

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS: adt_u32_partition_t
//////////////////////////////////////////////////////////////////////////////

static void test_adt_u32_partition_create(CuTest* tc)
{
   adt_u32_partition_t part;
   adt_u32_partition_create(&part);

   CuAssertTrue(tc, adt_u32_partition_is_empty(&part));
   CuAssertUIntEquals(tc, 0u, adt_u32_partition_length(&part));
   CuAssertUIntEquals(tc, 0u, adt_u32_partition_total_size(&part));
   CuAssertIntEquals(tc, -1, adt_u32_partition_find(&part, 0u));

   adt_u32_partition_destroy(&part);
}

static void test_adt_u32_partition_new_delete(CuTest* tc)
{
   adt_u32_partition_t *part = adt_u32_partition_new();
   CuAssertPtrNotNull(tc, part);
   CuAssertTrue(tc, adt_u32_partition_is_empty(part));

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_append(part, 1024u));
   CuAssertFalse(tc, adt_u32_partition_is_empty(part));
   CuAssertUIntEquals(tc, 1u, adt_u32_partition_length(part));
   CuAssertUIntEquals(tc, 1024u, adt_u32_partition_total_size(part));

   adt_u32_partition_delete(part);
}

static void test_adt_u32_partition_vdelete(CuTest* tc)
{
   adt_u32_partition_t *part = adt_u32_partition_new();
   CuAssertPtrNotNull(tc, part);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_append(part, 128u));
   adt_u32_partition_vdelete((void*)part);
}

static void test_adt_u32_partition_append_and_find(CuTest* tc)
{
   adt_u32_partition_t part;
   adt_u32_partition_create(&part);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_append(&part, 100u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_append(&part, 200u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_append(&part, 300u));

   CuAssertUIntEquals(tc, 3u, adt_u32_partition_length(&part));
   CuAssertUIntEquals(tc, 600u, adt_u32_partition_total_size(&part));

   CuAssertIntEquals(tc, 0, adt_u32_partition_find(&part, 0u));
   CuAssertIntEquals(tc, 0, adt_u32_partition_find(&part, 99u));
   CuAssertIntEquals(tc, 1, adt_u32_partition_find(&part, 100u));
   CuAssertIntEquals(tc, 1, adt_u32_partition_find(&part, 299u));
   CuAssertIntEquals(tc, 2, adt_u32_partition_find(&part, 300u));
   CuAssertIntEquals(tc, 2, adt_u32_partition_find(&part, 599u));
   CuAssertIntEquals(tc, -1, adt_u32_partition_find(&part, 600u));

   adt_u32_partition_destroy(&part);
}

static void test_adt_u32_partition_get_segment(CuTest* tc)
{
   adt_u32_partition_t part;
   adt_u32_partition_create(&part);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_append(&part, 50u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_append(&part, 75u));

   uint32_t start = 0u;
   uint32_t length = 0u;

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_get_segment(&part, 0u, &start, &length));
   CuAssertUIntEquals(tc, 0u, start);
   CuAssertUIntEquals(tc, 50u, length);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_get_segment(&part, 1u, &start, &length));
   CuAssertUIntEquals(tc, 50u, start);
   CuAssertUIntEquals(tc, 75u, length);

   CuAssertIntEquals(tc, ADT_INDEX_OUT_OF_BOUNDS_ERROR, adt_u32_partition_get_segment(&part, 2u, &start, &length));

   adt_u32_partition_destroy(&part);
}

static void test_adt_u32_partition_large_offsets(CuTest* tc)
{
   adt_u32_partition_t part;
   adt_u32_partition_create(&part);

   /* Offsets exceeding 16-bit range (> 65535) */
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_append(&part, 100000u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_append(&part, 200000u));
   CuAssertUIntEquals(tc, 300000u, adt_u32_partition_total_size(&part));

   CuAssertIntEquals(tc, 0, adt_u32_partition_find(&part, 99999u));
   CuAssertIntEquals(tc, 1, adt_u32_partition_find(&part, 100000u));
   CuAssertIntEquals(tc, 1, adt_u32_partition_find(&part, 150000u));
   CuAssertIntEquals(tc, 1, adt_u32_partition_find(&part, 299999u));
   CuAssertIntEquals(tc, -1, adt_u32_partition_find(&part, 300000u));

   adt_u32_partition_destroy(&part);
}

static void test_adt_u32_partition_reserve_and_clear(CuTest* tc)
{
   adt_u32_partition_t part;
   adt_u32_partition_create(&part);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_reserve(&part, 50u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_partition_append(&part, 10u));
   CuAssertUIntEquals(tc, 1u, adt_u32_partition_length(&part));

   adt_u32_partition_clear(&part);
   CuAssertTrue(tc, adt_u32_partition_is_empty(&part));
   CuAssertUIntEquals(tc, 0u, adt_u32_partition_length(&part));
   CuAssertUIntEquals(tc, 0u, adt_u32_partition_total_size(&part));

   adt_u32_partition_destroy(&part);
}
