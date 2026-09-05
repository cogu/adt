/*****************************************************************************
* \file      testsuite_adt_list.c
* \author    Conny Gustafsson
* \date      2017-01-27
* \brief     Unit tests for adt_list_t
*
* Copyright (c) 2017-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "test_common.h"
#include "adt_list.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

/**************** Private Function Declarations *******************/
static void test_adt_list_new(CuTest* tc);
static void test_adt_list_insert_remove(CuTest* tc);
static void test_adt_list_insert_unique(CuTest* tc);
static void test_adt_list_insert_erase(CuTest* tc);
static void test_adt_list_clear(CuTest* tc);
static void test_adt_list_insert_before_after(CuTest* tc);
static void test_adt_list_iterators_and_accessors(CuTest* tc);
static void test_adt_list_destructor_enable(CuTest* tc);
static void test_adt_list_vdelete(CuTest* tc);

/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
CuSuite* testsuite_adt_list(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_list_new);
   SUITE_ADD_TEST(suite, test_adt_list_insert_remove);
   SUITE_ADD_TEST(suite, test_adt_list_insert_unique);
   SUITE_ADD_TEST(suite, test_adt_list_insert_erase);
   SUITE_ADD_TEST(suite, test_adt_list_clear);
   SUITE_ADD_TEST(suite, test_adt_list_insert_before_after);
   SUITE_ADD_TEST(suite, test_adt_list_iterators_and_accessors);
   SUITE_ADD_TEST(suite, test_adt_list_destructor_enable);
   SUITE_ADD_TEST(suite, test_adt_list_vdelete);

   return suite;
}

/***************** Private Function Definitions *******************/
static void test_adt_list_new(CuTest* tc)
{
   adt_list_t *list;
   list = adt_list_new(vfree);
   CuAssertPtrNotNull(tc,list);
   adt_list_delete(list);
}

static void test_adt_list_insert_remove(CuTest* tc)
{
   adt_list_t *list;
   list = adt_list_new(NULL);
   char *hello=STRDUP("hello");
   char *world=STRDUP("world");
   CuAssertPtrNotNull(tc,list);
   adt_list_insert(list, hello);
   adt_list_insert(list, world);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   adt_list_remove(list, hello);
   CuAssertIntEquals(tc, 1, adt_list_length(list));
   adt_list_remove(list, hello);
   CuAssertIntEquals(tc, 1, adt_list_length(list));
   adt_list_remove(list, world);
   CuAssertIntEquals(tc, 0, adt_list_length(list));
   adt_list_delete(list);
   free(hello);
   free(world);
}

static void test_adt_list_insert_unique(CuTest* tc)
{
   adt_list_t *list;
   list = adt_list_new(vfree);
   char *hello=STRDUP("hello");
   char *world=STRDUP("world");
   CuAssertPtrNotNull(tc,list);
   adt_list_insert_unique(list, hello);
   adt_list_insert_unique(list, world);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   adt_list_insert_unique(list, hello);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   adt_list_insert_unique(list, world);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   adt_list_delete(list);
}

static void test_adt_list_insert_erase(CuTest* tc)
{
   adt_list_t *list;
   adt_list_elem_t *iter;
   list = adt_list_new(NULL);
   char *hello=STRDUP("hello");
   char *world=STRDUP("world");
   CuAssertPtrNotNull(tc,list);
   adt_list_insert(list, hello);
   adt_list_insert(list, world);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   iter = adt_list_iter_last(list);
   CuAssertPtrNotNull(tc, iter);
   CuAssertPtrEquals(tc, world, iter->pItem);
   adt_list_erase(list, iter);
   CuAssertIntEquals(tc, 1, adt_list_length(list));
   iter = adt_list_iter_last(list);
   CuAssertPtrNotNull(tc, iter);
   CuAssertPtrEquals(tc, hello, iter->pItem);
   adt_list_erase(list, iter);
   CuAssertIntEquals(tc, 0, adt_list_length(list));
   adt_list_delete(list);
   free(hello);
   free(world);
}

static void test_adt_list_clear(CuTest* tc)
{
   adt_list_t *list;
   list = adt_list_new(vfree);
   char *hello=STRDUP("hello");
   char *world=STRDUP("world");
   char *foo=STRDUP("foo");
   CuAssertPtrNotNull(tc,list);
   adt_list_insert(list, hello);
   adt_list_insert(list, world);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   adt_list_clear(list);
   CuAssertIntEquals(tc, 0, adt_list_length(list));
   adt_list_insert(list, foo);
   CuAssertIntEquals(tc, 1, adt_list_length(list));
   adt_list_delete(list);
}

static void test_adt_list_insert_before_after(CuTest* tc)
{
   adt_list_t *list = adt_list_new(vfree);
   char *item1 = STRDUP("1");
   char *item3 = STRDUP("3");
   char *item2 = STRDUP("2");
   char *item0 = STRDUP("0");
   char *item4 = STRDUP("4");

   adt_list_insert(list, item1);
   adt_list_insert(list, item3);

   // Insert item2 before item3
   adt_list_elem_t *iter3 = adt_list_find(list, item3);
   CuAssertPtrNotNull(tc, iter3);
   adt_list_insert_before(list, iter3, item2);

   // Insert item0 before item1 (pFirst update)
   adt_list_elem_t *iter1 = adt_list_find(list, item1);
   CuAssertPtrNotNull(tc, iter1);
   adt_list_insert_before(list, iter1, item0);

   // Insert item4 after item3 (pLast update)
   iter3 = adt_list_find(list, item3);
   adt_list_insert_after(list, iter3, item4);

   CuAssertIntEquals(tc, 5, adt_list_length(list));

   // Verify order: 0, 1, 2, 3, 4
   adt_list_elem_t *it = adt_list_iter_first(list);
   CuAssertPtrNotNull(tc, it);
   CuAssertStrEquals(tc, "0", (char*)it->pItem);
   it = adt_list_iter_next(it);
   CuAssertStrEquals(tc, "1", (char*)it->pItem);
   it = adt_list_iter_next(it);
   CuAssertStrEquals(tc, "2", (char*)it->pItem);
   it = adt_list_iter_next(it);
   CuAssertStrEquals(tc, "3", (char*)it->pItem);
   it = adt_list_iter_next(it);
   CuAssertStrEquals(tc, "4", (char*)it->pItem);
   it = adt_list_iter_next(it);
   CuAssertPtrEquals(tc, NULL, it);

   adt_list_delete(list);
}

static void test_adt_list_iterators_and_accessors(CuTest* tc)
{
   adt_list_t *list = adt_list_new(vfree);
   CuAssertTrue(tc, adt_list_is_empty(list));
   CuAssertPtrEquals(tc, NULL, adt_list_first(list));
   CuAssertPtrEquals(tc, NULL, adt_list_last(list));
   CuAssertPtrEquals(tc, NULL, adt_list_iter_first(list));
   CuAssertPtrEquals(tc, NULL, adt_list_iter_last(list));

   char *a = STRDUP("A");
   char *b = STRDUP("B");
   char *c = STRDUP("C");

   adt_list_insert(list, a);
   adt_list_insert(list, b);
   adt_list_insert(list, c);

   CuAssertTrue(tc, !adt_list_is_empty(list));
   CuAssertPtrEquals(tc, a, adt_list_first(list));
   CuAssertPtrEquals(tc, c, adt_list_last(list));

   // Reverse iteration using adt_list_iter_prev
   adt_list_elem_t *it = adt_list_iter_last(list);
   CuAssertPtrNotNull(tc, it);
   CuAssertStrEquals(tc, "C", (char*)it->pItem);
   it = adt_list_iter_prev(it);
   CuAssertPtrNotNull(tc, it);
   CuAssertStrEquals(tc, "B", (char*)it->pItem);
   it = adt_list_iter_prev(it);
   CuAssertPtrNotNull(tc, it);
   CuAssertStrEquals(tc, "A", (char*)it->pItem);
   it = adt_list_iter_prev(it);
   CuAssertPtrEquals(tc, NULL, it);

   adt_list_delete(list);
}

static void test_adt_list_destructor_enable(CuTest* tc)
{
   adt_list_t *list = adt_list_new(vfree);
   adt_list_destructor_enable(list, false);

   char static_str[] = "static";
   adt_list_insert(list, static_str);
   CuAssertIntEquals(tc, 1, adt_list_length(list));

   // Clearing/deleting with destructor disabled should not call vfree on static_str
   adt_list_clear(list);
   CuAssertIntEquals(tc, 0, adt_list_length(list));

   adt_list_delete(list);
}

static void test_adt_list_vdelete(CuTest* tc)
{
   adt_list_t *outer = adt_list_new(adt_list_vdelete);
   adt_list_t *inner1 = adt_list_new(vfree);
   adt_list_t *inner2 = adt_list_new(vfree);

   adt_list_insert(inner1, STRDUP("first_1"));
   adt_list_insert(inner1, STRDUP("first_2"));

   adt_list_insert(inner2, STRDUP("second_1"));
   adt_list_insert(inner2, STRDUP("second_2"));

   adt_list_insert(outer, inner1);
   adt_list_insert(outer, inner2);
   CuAssertIntEquals(tc, 2, adt_list_length(outer));

   // adt_list_delete will invoke adt_list_vdelete on both inner1 and inner2
   adt_list_delete(outer);
}

