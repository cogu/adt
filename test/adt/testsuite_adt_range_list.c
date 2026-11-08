/*****************************************************************************
* \file      testsuite_adt_range_list.c
* \author    Conny Gustafsson
* \date      2026-11-08
* \brief     Unit tests for adt_u16_range_list_t and adt_u32_range_list_t
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
#include "adt_range_list.h"
#ifdef MEM_LEAK_CHECK
# include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_adt_u16_range_list_create(CuTest* tc);
static void test_adt_u16_range_list_new_delete(CuTest* tc);
static void test_adt_u16_range_list_vdelete(CuTest* tc);
static void test_adt_u16_range_list_insert_disjoint(CuTest* tc);
static void test_adt_u16_range_list_insert_adjacent(CuTest* tc);
static void test_adt_u16_range_list_insert_overlapping(CuTest* tc);
static void test_adt_u16_range_list_insert_bridge_multiple(CuTest* tc);
static void test_adt_u16_range_list_insert_max_gap(CuTest* tc);
static void test_adt_u16_range_list_find_contains(CuTest* tc);
static void test_adt_u16_range_list_clear_reserve(CuTest* tc);
static void test_adt_u16_range_list_overflow(CuTest* tc);

static void test_adt_u32_range_list_create(CuTest* tc);
static void test_adt_u32_range_list_new_delete(CuTest* tc);
static void test_adt_u32_range_list_vdelete(CuTest* tc);
static void test_adt_u32_range_list_insert_disjoint(CuTest* tc);
static void test_adt_u32_range_list_insert_adjacent(CuTest* tc);
static void test_adt_u32_range_list_insert_overlapping(CuTest* tc);
static void test_adt_u32_range_list_insert_bridge_multiple(CuTest* tc);
static void test_adt_u32_range_list_insert_max_gap(CuTest* tc);
static void test_adt_u32_range_list_find_contains(CuTest* tc);
static void test_adt_u32_range_list_large_offsets(CuTest* tc);
static void test_adt_u32_range_list_clear_reserve(CuTest* tc);
static void test_adt_u32_range_list_overflow(CuTest* tc);
static void test_adt_u32_range_list_simulated_port_updates(CuTest* tc);

//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_adt_range_list(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_u16_range_list_create);
   SUITE_ADD_TEST(suite, test_adt_u16_range_list_new_delete);
   SUITE_ADD_TEST(suite, test_adt_u16_range_list_vdelete);
   SUITE_ADD_TEST(suite, test_adt_u16_range_list_insert_disjoint);
   SUITE_ADD_TEST(suite, test_adt_u16_range_list_insert_adjacent);
   SUITE_ADD_TEST(suite, test_adt_u16_range_list_insert_overlapping);
   SUITE_ADD_TEST(suite, test_adt_u16_range_list_insert_bridge_multiple);
   SUITE_ADD_TEST(suite, test_adt_u16_range_list_insert_max_gap);
   SUITE_ADD_TEST(suite, test_adt_u16_range_list_find_contains);
   SUITE_ADD_TEST(suite, test_adt_u16_range_list_clear_reserve);
   SUITE_ADD_TEST(suite, test_adt_u16_range_list_overflow);

   SUITE_ADD_TEST(suite, test_adt_u32_range_list_create);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_new_delete);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_vdelete);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_insert_disjoint);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_insert_adjacent);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_insert_overlapping);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_insert_bridge_multiple);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_insert_max_gap);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_find_contains);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_large_offsets);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_clear_reserve);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_overflow);
   SUITE_ADD_TEST(suite, test_adt_u32_range_list_simulated_port_updates);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS: adt_u16_range_list_t
//////////////////////////////////////////////////////////////////////////////

static void test_adt_u16_range_list_create(CuTest* tc)
{
   adt_u16_range_list_t list;
   adt_u16_range_list_create(&list);

   CuAssertTrue(tc, adt_u16_range_list_is_empty(&list));
   CuAssertUIntEquals(tc, 0u, adt_u16_range_list_length(&list));
   CuAssertUIntEquals(tc, 0u, adt_u16_range_list_total_length(&list));
   CuAssertIntEquals(tc, -1, adt_u16_range_list_find(&list, 0u));
   CuAssertFalse(tc, adt_u16_range_list_contains(&list, 0u));

   adt_u16_range_list_destroy(&list);
}

static void test_adt_u16_range_list_new_delete(CuTest* tc)
{
   adt_u16_range_list_t *list = adt_u16_range_list_new();
   CuAssertPtrNotNull(tc, list);
   CuAssertTrue(tc, adt_u16_range_list_is_empty(list));

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(list, 10u, 5u, 0u));
   CuAssertFalse(tc, adt_u16_range_list_is_empty(list));
   CuAssertUIntEquals(tc, 1u, adt_u16_range_list_length(list));
   CuAssertUIntEquals(tc, 5u, adt_u16_range_list_total_length(list));

   adt_u16_range_list_delete(list);
}

static void test_adt_u16_range_list_vdelete(CuTest* tc)
{
   adt_u16_range_list_t *list = adt_u16_range_list_new();
   CuAssertPtrNotNull(tc, list);
   adt_u16_range_list_vdelete(list);
}

static void test_adt_u16_range_list_insert_disjoint(CuTest* tc)
{
   adt_u16_range_list_t list;
   adt_u16_range_list_create(&list);

   // Insert [20, 25)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 20u, 5u, 0u));
   // Insert before: [5, 10)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 5u, 5u, 0u));
   // Insert after: [40, 50)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 40u, 10u, 0u));
   // Insert in between: [15, 18)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 15u, 3u, 0u));

   CuAssertUIntEquals(tc, 4u, adt_u16_range_list_length(&list));
   CuAssertUIntEquals(tc, 23u, adt_u16_range_list_total_length(&list));

   adt_u16_range_t range;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_get_range(&list, 0u, &range));
   CuAssertUIntEquals(tc, 5u, range.start);
   CuAssertUIntEquals(tc, 5u, range.length);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_get_range(&list, 1u, &range));
   CuAssertUIntEquals(tc, 15u, range.start);
   CuAssertUIntEquals(tc, 3u, range.length);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_get_range(&list, 2u, &range));
   CuAssertUIntEquals(tc, 20u, range.start);
   CuAssertUIntEquals(tc, 5u, range.length);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_get_range(&list, 3u, &range));
   CuAssertUIntEquals(tc, 40u, range.start);
   CuAssertUIntEquals(tc, 10u, range.length);

   adt_u16_range_list_destroy(&list);
}

static void test_adt_u16_range_list_insert_adjacent(CuTest* tc)
{
   adt_u16_range_list_t list;
   adt_u16_range_list_create(&list);

   // Insert [10, 15)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 10u, 5u, 0u));
   // Append adjacent [15, 20) -> should merge into [10, 20)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 15u, 5u, 0u));

   CuAssertUIntEquals(tc, 1u, adt_u16_range_list_length(&list));
   const adt_u16_range_t *r0 = adt_u16_range_list_at(&list, 0u);
   CuAssertPtrNotNull(tc, r0);
   CuAssertUIntEquals(tc, 10u, r0->start);
   CuAssertUIntEquals(tc, 10u, r0->length);

   // Prepend adjacent [6, 10) -> should merge into [6, 20)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 6u, 4u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u16_range_list_length(&list));
   r0 = adt_u16_range_list_at(&list, 0u);
   CuAssertUIntEquals(tc, 6u, r0->start);
   CuAssertUIntEquals(tc, 14u, r0->length);

   adt_u16_range_list_destroy(&list);
}

static void test_adt_u16_range_list_insert_overlapping(CuTest* tc)
{
   adt_u16_range_list_t list;
   adt_u16_range_list_create(&list);

   // Insert [10, 20)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 10u, 10u, 0u));

   // Insert subset [12, 16) -> no change to [10, 20)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 12u, 4u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u16_range_list_length(&list));
   CuAssertUIntEquals(tc, 10u, list.ranges[0].start);
   CuAssertUIntEquals(tc, 10u, list.ranges[0].length);

   // Insert overlapping right [18, 25) -> expands to [10, 25)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 18u, 7u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u16_range_list_length(&list));
   CuAssertUIntEquals(tc, 10u, list.ranges[0].start);
   CuAssertUIntEquals(tc, 15u, list.ranges[0].length);

   // Insert overlapping left [5, 12) -> expands to [5, 25)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 5u, 7u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u16_range_list_length(&list));
   CuAssertUIntEquals(tc, 5u, list.ranges[0].start);
   CuAssertUIntEquals(tc, 20u, list.ranges[0].length);

   // Insert superset [0, 30) -> expands to [0, 30)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 0u, 30u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u16_range_list_length(&list));
   CuAssertUIntEquals(tc, 0u, list.ranges[0].start);
   CuAssertUIntEquals(tc, 30u, list.ranges[0].length);

   adt_u16_range_list_destroy(&list);
}

static void test_adt_u16_range_list_insert_bridge_multiple(CuTest* tc)
{
   adt_u16_range_list_t list;
   adt_u16_range_list_create(&list);

   // Setup 4 disjoint ranges: [0, 2), [4, 6), [8, 10), [12, 14)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 0u, 2u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 4u, 2u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 8u, 2u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 12u, 2u, 0u));
   CuAssertUIntEquals(tc, 4u, adt_u16_range_list_length(&list));

   // Insert [1, 9) -> bridges range 0 [0, 2), range 1 [4, 6), and range 2 [8, 10)
   // Result should merge into [0, 10), leaving [12, 14) intact
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 1u, 8u, 0u));
   CuAssertUIntEquals(tc, 2u, adt_u16_range_list_length(&list));

   CuAssertUIntEquals(tc, 0u, list.ranges[0].start);
   CuAssertUIntEquals(tc, 10u, list.ranges[0].length);

   CuAssertUIntEquals(tc, 12u, list.ranges[1].start);
   CuAssertUIntEquals(tc, 2u, list.ranges[1].length);

   // Bridge remaining: insert [9, 13) -> connects [0, 10) and [12, 14) into [0, 14)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 9u, 4u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u16_range_list_length(&list));
   CuAssertUIntEquals(tc, 0u, list.ranges[0].start);
   CuAssertUIntEquals(tc, 14u, list.ranges[0].length);

   adt_u16_range_list_destroy(&list);
}

static void test_adt_u16_range_list_insert_max_gap(CuTest* tc)
{
   adt_u16_range_list_t list;
   adt_u16_range_list_create(&list);

   // Insert [0, 2)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 0u, 2u, 0u));

   // Gap is 2 bytes (bytes 2..3). With max_gap = 1, [4, 6) does NOT merge
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 4u, 2u, 1u));
   CuAssertUIntEquals(tc, 2u, adt_u16_range_list_length(&list));

   // Clear and retry with max_gap = 2: gap between end=2 and start=4 is 2 <= 2 -> merges into [0, 6)
   adt_u16_range_list_clear(&list);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 0u, 2u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 4u, 2u, 2u));
   CuAssertUIntEquals(tc, 1u, adt_u16_range_list_length(&list));
   CuAssertUIntEquals(tc, 0u, list.ranges[0].start);
   CuAssertUIntEquals(tc, 6u, list.ranges[0].length);

   adt_u16_range_list_destroy(&list);
}

static void test_adt_u16_range_list_find_contains(CuTest* tc)
{
   adt_u16_range_list_t list;
   adt_u16_range_list_create(&list);

   // Ranges: [10, 15) and [30, 40)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 10u, 5u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 30u, 10u, 0u));

   // In bounds of range 0
   CuAssertIntEquals(tc, 0, adt_u16_range_list_find(&list, 10u));
   CuAssertIntEquals(tc, 0, adt_u16_range_list_find(&list, 14u));
   CuAssertTrue(tc, adt_u16_range_list_contains(&list, 10u));
   CuAssertTrue(tc, adt_u16_range_list_contains(&list, 14u));

   // Out of bounds / in gap
   CuAssertIntEquals(tc, -1, adt_u16_range_list_find(&list, 9u));
   CuAssertIntEquals(tc, -1, adt_u16_range_list_find(&list, 15u));
   CuAssertIntEquals(tc, -1, adt_u16_range_list_find(&list, 20u));
   CuAssertIntEquals(tc, -1, adt_u16_range_list_find(&list, 29u));
   CuAssertFalse(tc, adt_u16_range_list_contains(&list, 15u));

   // In bounds of range 1
   CuAssertIntEquals(tc, 1, adt_u16_range_list_find(&list, 30u));
   CuAssertIntEquals(tc, 1, adt_u16_range_list_find(&list, 39u));
   CuAssertIntEquals(tc, -1, adt_u16_range_list_find(&list, 40u));

   adt_u16_range_list_destroy(&list);
}

static void test_adt_u16_range_list_clear_reserve(CuTest* tc)
{
   adt_u16_range_list_t list;
   adt_u16_range_list_create(&list);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_reserve(&list, 16u));
   CuAssertTrue(tc, list.capacity >= 16u);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 0u, 10u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u16_range_list_length(&list));

   adt_u16_range_list_clear(&list);
   CuAssertUIntEquals(tc, 0u, adt_u16_range_list_length(&list));
   CuAssertTrue(tc, adt_u16_range_list_is_empty(&list));
   CuAssertTrue(tc, list.capacity >= 16u);

   adt_u16_range_list_destroy(&list);
}

static void test_adt_u16_range_list_overflow(CuTest* tc)
{
   adt_u16_range_list_t list;
   adt_u16_range_list_create(&list);

   // Zero length is a no-op
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 10u, 0u, 0u));
   CuAssertUIntEquals(tc, 0u, adt_u16_range_list_length(&list));

   // Overflow: 65530 + 10 > 65535
   CuAssertIntEquals(tc, ADT_OVERFLOW_ERROR, adt_u16_range_list_insert(&list, 65530u, 10u, 0u));

   // Valid at boundary: 65530 + 5 = 65535
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16_range_list_insert(&list, 65530u, 5u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u16_range_list_length(&list));

   adt_u16_range_list_destroy(&list);
}

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS: adt_u32_range_list_t
//////////////////////////////////////////////////////////////////////////////

static void test_adt_u32_range_list_create(CuTest* tc)
{
   adt_u32_range_list_t list;
   adt_u32_range_list_create(&list);

   CuAssertTrue(tc, adt_u32_range_list_is_empty(&list));
   CuAssertUIntEquals(tc, 0u, adt_u32_range_list_length(&list));
   CuAssertUIntEquals(tc, 0u, (uint32_t)adt_u32_range_list_total_length(&list));

   adt_u32_range_list_destroy(&list);
}

static void test_adt_u32_range_list_new_delete(CuTest* tc)
{
   adt_u32_range_list_t *list = adt_u32_range_list_new();
   CuAssertPtrNotNull(tc, list);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(list, 100u, 50u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u32_range_list_length(list));
   adt_u32_range_list_delete(list);
}

static void test_adt_u32_range_list_vdelete(CuTest* tc)
{
   adt_u32_range_list_t *list = adt_u32_range_list_new();
   CuAssertPtrNotNull(tc, list);
   adt_u32_range_list_vdelete(list);
}

static void test_adt_u32_range_list_insert_disjoint(CuTest* tc)
{
   adt_u32_range_list_t list;
   adt_u32_range_list_create(&list);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 500u, 100u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 100u, 50u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 300u, 50u, 0u));

   CuAssertUIntEquals(tc, 3u, adt_u32_range_list_length(&list));
   CuAssertUIntEquals(tc, 200u, (uint32_t)adt_u32_range_list_total_length(&list));

   adt_u32_range_t r;
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_get_range(&list, 0u, &r));
   CuAssertUIntEquals(tc, 100u, r.start);
   CuAssertUIntEquals(tc, 50u, r.length);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_get_range(&list, 1u, &r));
   CuAssertUIntEquals(tc, 300u, r.start);
   CuAssertUIntEquals(tc, 50u, r.length);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_get_range(&list, 2u, &r));
   CuAssertUIntEquals(tc, 500u, r.start);
   CuAssertUIntEquals(tc, 100u, r.length);

   adt_u32_range_list_destroy(&list);
}

static void test_adt_u32_range_list_insert_adjacent(CuTest* tc)
{
   adt_u32_range_list_t list;
   adt_u32_range_list_create(&list);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 1000u, 500u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 1500u, 500u, 0u));

   CuAssertUIntEquals(tc, 1u, adt_u32_range_list_length(&list));
   const adt_u32_range_t *r0 = adt_u32_range_list_at(&list, 0u);
   CuAssertUIntEquals(tc, 1000u, r0->start);
   CuAssertUIntEquals(tc, 1000u, r0->length);

   adt_u32_range_list_destroy(&list);
}

static void test_adt_u32_range_list_insert_overlapping(CuTest* tc)
{
   adt_u32_range_list_t list;
   adt_u32_range_list_create(&list);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 100u, 100u, 0u));
   // Overlap right
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 150u, 100u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u32_range_list_length(&list));
   CuAssertUIntEquals(tc, 100u, list.ranges[0].start);
   CuAssertUIntEquals(tc, 150u, list.ranges[0].length);

   // Overlap left
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 50u, 80u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u32_range_list_length(&list));
   CuAssertUIntEquals(tc, 50u, list.ranges[0].start);
   CuAssertUIntEquals(tc, 200u, list.ranges[0].length);

   adt_u32_range_list_destroy(&list);
}

static void test_adt_u32_range_list_insert_bridge_multiple(CuTest* tc)
{
   adt_u32_range_list_t list;
   adt_u32_range_list_create(&list);

   // Setup 5 disjoint ranges:
   // [100, 150), [200, 250), [300, 350), [400, 450), [500, 550)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 100u, 50u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 200u, 50u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 300u, 50u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 400u, 50u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 500u, 50u, 0u));
   CuAssertUIntEquals(tc, 5u, adt_u32_range_list_length(&list));

   // Insert [180, 420) -> spans across ranges 1, 2, and 3!
   // Should merge ranges 1, 2, 3 into [180, 450)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 180u, 240u, 0u));
   CuAssertUIntEquals(tc, 3u, adt_u32_range_list_length(&list));

   CuAssertUIntEquals(tc, 100u, list.ranges[0].start);
   CuAssertUIntEquals(tc, 50u, list.ranges[0].length);

   CuAssertUIntEquals(tc, 180u, list.ranges[1].start);
   CuAssertUIntEquals(tc, 270u, list.ranges[1].length); // 450 - 180 = 270

   CuAssertUIntEquals(tc, 500u, list.ranges[2].start);
   CuAssertUIntEquals(tc, 50u, list.ranges[2].length);

   adt_u32_range_list_destroy(&list);
}

static void test_adt_u32_range_list_insert_max_gap(CuTest* tc)
{
   adt_u32_range_list_t list;
   adt_u32_range_list_create(&list);

   // Simulate 6-byte header threshold (NumHeader + RMF Address header)
   uint32_t const apx_header_overhead = 6u;

   // Range A: [100, 104) (4 bytes)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 100u, 4u, apx_header_overhead));

   // Range B: [108, 112) (gap of 4 bytes: 104..107). 4 <= 6 -> should coalesce into [100, 112)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 108u, 4u, apx_header_overhead));
   CuAssertUIntEquals(tc, 1u, adt_u32_range_list_length(&list));
   CuAssertUIntEquals(tc, 100u, list.ranges[0].start);
   CuAssertUIntEquals(tc, 12u, list.ranges[0].length);

   // Range C: [150, 160) (gap of 38 bytes). 38 > 6 -> remains separate range
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 150u, 10u, apx_header_overhead));
   CuAssertUIntEquals(tc, 2u, adt_u32_range_list_length(&list));

   adt_u32_range_list_destroy(&list);
}

static void test_adt_u32_range_list_find_contains(CuTest* tc)
{
   adt_u32_range_list_t list;
   adt_u32_range_list_create(&list);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 1000u, 500u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 3000u, 200u, 0u));

   CuAssertIntEquals(tc, 0, adt_u32_range_list_find(&list, 1000u));
   CuAssertIntEquals(tc, 0, adt_u32_range_list_find(&list, 1499u));
   CuAssertIntEquals(tc, -1, adt_u32_range_list_find(&list, 1500u));

   CuAssertIntEquals(tc, 1, adt_u32_range_list_find(&list, 3000u));
   CuAssertIntEquals(tc, 1, adt_u32_range_list_find(&list, 3199u));
   CuAssertIntEquals(tc, -1, adt_u32_range_list_find(&list, 3200u));

   CuAssertTrue(tc, adt_u32_range_list_contains(&list, 1200u));
   CuAssertFalse(tc, adt_u32_range_list_contains(&list, 2000u));

   adt_u32_range_list_destroy(&list);
}

static void test_adt_u32_range_list_large_offsets(CuTest* tc)
{
   adt_u32_range_list_t list;
   adt_u32_range_list_create(&list);

   // Large 32-bit offsets (e.g. 1 GB, 2 GB)
   uint32_t const addr1 = 0x40000000u; // 1GB
   uint32_t const addr2 = 0x80000000u; // 2GB

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, addr1, 1024u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, addr2, 2048u, 0u));

   CuAssertUIntEquals(tc, 2u, adt_u32_range_list_length(&list));
   CuAssertUIntEquals(tc, 3072u, (uint32_t)adt_u32_range_list_total_length(&list));

   CuAssertIntEquals(tc, 0, adt_u32_range_list_find(&list, addr1 + 512u));
   CuAssertIntEquals(tc, 1, adt_u32_range_list_find(&list, addr2 + 1024u));

   adt_u32_range_list_destroy(&list);
}

static void test_adt_u32_range_list_clear_reserve(CuTest* tc)
{
   adt_u32_range_list_t list;
   adt_u32_range_list_create(&list);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_reserve(&list, 32u));
   CuAssertTrue(tc, list.capacity >= 32u);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 0u, 100u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u32_range_list_length(&list));

   adt_u32_range_list_clear(&list);
   CuAssertUIntEquals(tc, 0u, adt_u32_range_list_length(&list));
   CuAssertTrue(tc, list.capacity >= 32u);

   adt_u32_range_list_destroy(&list);
}

static void test_adt_u32_range_list_overflow(CuTest* tc)
{
   adt_u32_range_list_t list;
   adt_u32_range_list_create(&list);

   // Zero length is no-op
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 100u, 0u, 0u));
   CuAssertUIntEquals(tc, 0u, adt_u32_range_list_length(&list));

   // Overflow: UINT32_MAX - 10 + 20 > UINT32_MAX
   CuAssertIntEquals(tc, ADT_OVERFLOW_ERROR, adt_u32_range_list_insert(&list, UINT32_MAX - 10u, 20u, 0u));

   // Boundary: UINT32_MAX - 10 + 10 = UINT32_MAX - 1 (within range)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, UINT32_MAX - 10u, 10u, 0u));
   CuAssertUIntEquals(tc, 1u, adt_u32_range_list_length(&list));

   adt_u32_range_list_destroy(&list);
}

static void test_adt_u32_range_list_simulated_port_updates(CuTest* tc)
{
   adt_u32_range_list_t list;
   adt_u32_range_list_create(&list);

   // Simulate out-of-order port connection count writes (2 bytes each at port_id * 2)
   // Port 3: offset 6, size 2
   // Port 0: offset 0, size 2
   // Port 2: offset 4, size 2
   // Port 1: offset 2, size 2
   // All 4 ports should coalesce into one single range [0, 8)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 6u, 2u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 0u, 2u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 4u, 2u, 0u));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 2u, 2u, 0u));

   CuAssertUIntEquals(tc, 1u, adt_u32_range_list_length(&list));
   const adt_u32_range_t *r0 = adt_u32_range_list_at(&list, 0u);
   CuAssertPtrNotNull(tc, r0);
   CuAssertUIntEquals(tc, 0u, r0->start);
   CuAssertUIntEquals(tc, 8u, r0->length);

   // Simulate port 10: offset 20, size 2 (gap of 12 bytes from offset 8). With max_gap = 6, disjoint:
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 20u, 2u, 6u));
   CuAssertUIntEquals(tc, 2u, adt_u32_range_list_length(&list));

   // Simulate port 9: offset 18, size 2. Adjacent to port 10 (offset 20) -> merges with [20, 22) into [18, 22)
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32_range_list_insert(&list, 18u, 2u, 6u));
   CuAssertUIntEquals(tc, 2u, adt_u32_range_list_length(&list));
   const adt_u32_range_t *r1 = adt_u32_range_list_at(&list, 1u);
   CuAssertUIntEquals(tc, 18u, r1->start);
   CuAssertUIntEquals(tc, 4u, r1->length);

   adt_u32_range_list_destroy(&list);
}

