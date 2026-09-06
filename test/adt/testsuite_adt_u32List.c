/*****************************************************************************
* \file      testsuite_adt_u32List.c
* \author    Conny Gustafsson
* \date      2018-07-26
* \brief     Unit tests for adt_u32List_t
*
* Copyright (c) 2018-2026 Conny Gustafsson
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
static void test_adt_u32List_create(CuTest* tc);
static void test_adt_u32List_new(CuTest* tc);
static void test_adt_u32List_insert(CuTest* tc);
static void test_adt_u32List_erase(CuTest* tc);
static void test_adt_u32List_insert_before(CuTest* tc);
static void test_adt_u32List_insert_after(CuTest* tc);
static void test_adt_u32List_find(CuTest* tc);
static void test_adt_u32List_iter_prev(CuTest* tc);
static void test_adt_u32List_vdelete(CuTest* tc);
static void test_adt_u32List_errors(CuTest* tc);


/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
CuSuite* testsuite_adt_u32List(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_u32List_create);
   SUITE_ADD_TEST(suite, test_adt_u32List_new);
   SUITE_ADD_TEST(suite, test_adt_u32List_insert);
   SUITE_ADD_TEST(suite, test_adt_u32List_erase);
   SUITE_ADD_TEST(suite, test_adt_u32List_insert_before);
   SUITE_ADD_TEST(suite, test_adt_u32List_insert_after);
   SUITE_ADD_TEST(suite, test_adt_u32List_find);
   SUITE_ADD_TEST(suite, test_adt_u32List_iter_prev);
   SUITE_ADD_TEST(suite, test_adt_u32List_vdelete);
   SUITE_ADD_TEST(suite, test_adt_u32List_errors);

   return suite;
}

/***************** Private Function Definitions *******************/
static void test_adt_u32List_create(CuTest* tc)
{
   adt_u32List_t list;
   adt_u32List_create(&list);
   CuAssertPtrEquals(tc, NULL, list.pFirst);
   CuAssertPtrEquals(tc, NULL, list.pLast);

   CuAssertIntEquals(tc, 0, adt_u32List_length(&list));
}

static void test_adt_u32List_new(CuTest* tc)
{
   adt_u32List_t *list;
   list = adt_u32List_new();
   CuAssertPtrNotNull(tc, list);
   CuAssertPtrEquals(tc, NULL, list->pFirst);
   CuAssertPtrEquals(tc, NULL, list->pLast);

   adt_u32List_delete(list);
}

static void test_adt_u32List_insert(CuTest* tc)
{
   adt_u32List_t *list;
   adt_u32List_elem_t *iter;
   list = adt_u32List_new();
   adt_u32List_insert(list, 1);
   adt_u32List_insert(list, 2);
   adt_u32List_insert(list, 3);
   CuAssertIntEquals(tc, 3, adt_u32List_length(list));
   iter = adt_u32List_iter_first(list);
   CuAssertPtrNotNull(tc, iter);
   CuAssertUIntEquals(tc, 1, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertPtrNotNull(tc, iter);
   CuAssertUIntEquals(tc, 2, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertPtrNotNull(tc, iter);
   CuAssertUIntEquals(tc, 3, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertPtrEquals(tc, NULL, iter);
   adt_u32List_delete(list);
}

static void test_adt_u32List_erase(CuTest* tc)
{
   adt_u32List_t *list;
   adt_u32List_elem_t *iter1, *iter2;
   list = adt_u32List_new();
   //erase on an empty list
   CuAssertIntEquals(tc, 0, adt_u32List_length(list));
   adt_u32List_erase(list, 0);
   CuAssertIntEquals(tc, 0, adt_u32List_length(list));

   //erase on a list with one element
   adt_u32List_clear(list);
   adt_u32List_insert(list, 1);
   CuAssertTrue(tc, !adt_u32List_is_empty(list));
   adt_u32List_erase(list, adt_u32List_iter_first(list));
   CuAssertTrue(tc, adt_u32List_is_empty(list));

   //erase first on a list with two elements
   adt_u32List_clear(list);
   adt_u32List_insert(list, 1);
   adt_u32List_insert(list, 2);
   adt_u32List_erase(list, adt_u32List_iter_first(list));
   CuAssertIntEquals(tc, 1, adt_u32List_length(list));
   CuAssertTrue(tc, !adt_u32List_is_empty(list));
   iter1 = adt_u32List_iter_first(list);
   iter2 = adt_u32List_iter_last(list);
   CuAssertUIntEquals(tc, 2, iter1->item);
   CuAssertPtrEquals(tc, iter1, iter2);

   //erase last on a list with two elements
   adt_u32List_clear(list);
   adt_u32List_insert(list, 1);
   adt_u32List_insert(list, 2);
   adt_u32List_erase(list, adt_u32List_iter_last(list));
   CuAssertIntEquals(tc, 1, adt_u32List_length(list));
   iter1 = adt_u32List_iter_first(list);
   iter2 = adt_u32List_iter_last(list);
   CuAssertUIntEquals(tc, 1, iter1->item);
   CuAssertPtrEquals(tc, iter1, iter2);

   //erase middle on a list with three elements
   adt_u32List_clear(list);
   adt_u32List_insert(list, 1);
   adt_u32List_insert(list, 2);
   adt_u32List_insert(list, 3);
   iter1 = adt_u32List_iter_first(list);
   iter1 = adt_u32List_iter_next(iter1);
   CuAssertUIntEquals(tc, 2, iter1->item);
   adt_u32List_erase(list, iter1);
   CuAssertIntEquals(tc, 2, adt_u32List_length(list));
   iter1 = adt_u32List_iter_first(list);
   iter2 = adt_u32List_iter_last(list);
   CuAssertUIntEquals(tc, 1, iter1->item);
   CuAssertUIntEquals(tc, 3, iter2->item);

   //erase first items until empty
   adt_u32List_clear(list);
   adt_u32List_insert(list, 1);
   adt_u32List_insert(list, 2);
   adt_u32List_insert(list, 3);
   CuAssertTrue(tc, !adt_u32List_is_empty(list));
   iter1 = adt_u32List_iter_first(list);
   CuAssertUIntEquals(tc, 1, iter1->item);
   adt_u32List_erase(list, iter1);
   iter1 = adt_u32List_iter_first(list);
   CuAssertUIntEquals(tc, 2, iter1->item);
   adt_u32List_erase(list, iter1);
   iter1 = adt_u32List_iter_first(list);
   CuAssertUIntEquals(tc, 3, iter1->item);
   adt_u32List_erase(list, iter1);
   CuAssertTrue(tc, adt_u32List_is_empty(list));

   //erase last items until empty
   adt_u32List_clear(list);
   adt_u32List_insert(list, 1);
   adt_u32List_insert(list, 2);
   adt_u32List_insert(list, 3);
   CuAssertTrue(tc, !adt_u32List_is_empty(list));
   iter1 = adt_u32List_iter_last(list);
   CuAssertUIntEquals(tc, 3, iter1->item);
   adt_u32List_erase(list, iter1);
   iter1 = adt_u32List_iter_last(list);
   CuAssertUIntEquals(tc, 2, iter1->item);
   adt_u32List_erase(list, iter1);
   iter1 = adt_u32List_iter_last(list);
   CuAssertUIntEquals(tc, 1, iter1->item);
   adt_u32List_erase(list, iter1);
   CuAssertTrue(tc, adt_u32List_is_empty(list));

   //cleanup
   adt_u32List_delete(list);
}

static void test_adt_u32List_insert_before(CuTest* tc)
{
   adt_u32List_t *list;
   adt_u32List_elem_t *iter;
   list = adt_u32List_new();

   //insert before first item
   adt_u32List_clear(list);
   adt_u32List_insert(list, 1);
   adt_u32List_insert(list, 2);

   adt_u32List_insert_before(list, adt_u32List_iter_first(list), 99);
   iter = adt_u32List_iter_first(list);
   CuAssertUIntEquals(tc, 99, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 1, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 2, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertPtrEquals(tc, NULL, iter);

   //insert before last item
   adt_u32List_clear(list);
   adt_u32List_insert(list, 1);
   adt_u32List_insert(list, 2);

   adt_u32List_insert_before(list, adt_u32List_iter_last(list), 99);
   iter = adt_u32List_iter_first(list);
   CuAssertUIntEquals(tc, 1, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 99, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 2, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertPtrEquals(tc, NULL, iter);

   //cleanup
   adt_u32List_delete(list);
}

static void test_adt_u32List_insert_after(CuTest* tc)
{
   adt_u32List_t *list;
   adt_u32List_elem_t *iter;
   list = adt_u32List_new();

   //insert after first item
   adt_u32List_clear(list);
   adt_u32List_insert(list, 1);
   adt_u32List_insert(list, 2);

   adt_u32List_insert_after(list, adt_u32List_iter_first(list), 99);
   iter = adt_u32List_iter_first(list);
   CuAssertUIntEquals(tc, 1, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 99, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 2, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertPtrEquals(tc, NULL, iter);

   //insert after last item
   adt_u32List_clear(list);
   adt_u32List_insert(list, 1);
   adt_u32List_insert(list, 2);

   adt_u32List_insert_after(list, adt_u32List_iter_last(list), 99);
   iter = adt_u32List_iter_first(list);
   CuAssertUIntEquals(tc, 1, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 2, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 99, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertPtrEquals(tc, NULL, iter);

   //cleanup
   adt_u32List_delete(list);
}

static void test_adt_u32List_find(CuTest* tc)
{
   adt_u32List_t *list;
   adt_u32List_elem_t *iter;
   list = adt_u32List_new();
   adt_u32List_insert(list, 1);
   adt_u32List_insert(list, 2);
   adt_u32List_insert(list, 3);
   CuAssertIntEquals(tc, 3, adt_u32List_length(list));
   iter = adt_u32List_find(list, 3);
   CuAssertPtrNotNull(tc, iter);
   CuAssertUIntEquals(tc, 3, iter->item);
   iter = adt_u32List_find(list, 2);
   CuAssertPtrNotNull(tc, iter);
   CuAssertUIntEquals(tc, 2, iter->item);
   iter = adt_u32List_find(list, 1);
   CuAssertPtrNotNull(tc, iter);
   CuAssertUIntEquals(tc, 1, iter->item);
   adt_u32List_delete(list);
}

static void test_adt_u32List_iter_prev(CuTest* tc)
{
   adt_u32List_t *list = adt_u32List_new();
   adt_u32List_insert(list, 10);
   adt_u32List_insert(list, 20);
   adt_u32List_insert(list, 30);

   adt_u32List_elem_t *it = adt_u32List_iter_last(list);
   CuAssertPtrNotNull(tc, it);
   CuAssertUIntEquals(tc, 30, it->item);
   it = adt_u32List_iter_prev(it);
   CuAssertPtrNotNull(tc, it);
   CuAssertUIntEquals(tc, 20, it->item);
   it = adt_u32List_iter_prev(it);
   CuAssertPtrNotNull(tc, it);
   CuAssertUIntEquals(tc, 10, it->item);
   it = adt_u32List_iter_prev(it);
   CuAssertPtrEquals(tc, NULL, it);

   adt_u32List_delete(list);
}

static void test_adt_u32List_vdelete(CuTest* tc)
{
   adt_list_t *outer = adt_list_new(adt_u32List_vdelete);
   adt_u32List_t *inner1 = adt_u32List_new();
   adt_u32List_t *inner2 = adt_u32List_new();

   adt_u32List_insert(inner1, 10);
   adt_u32List_insert(inner1, 20);
   adt_u32List_insert(inner1, 30);

   adt_u32List_insert(inner2, 40);
   adt_u32List_insert(inner2, 50);
   adt_u32List_insert(inner2, 60);

   adt_list_insert(outer, inner1);
   adt_list_insert(outer, inner2);
   CuAssertIntEquals(tc, 2, adt_list_length(outer));

   // adt_list_delete will invoke adt_u32List_vdelete on both inner1 and inner2
   adt_list_delete(outer);
}

static void test_adt_u32List_errors(CuTest* tc)
{
   adt_u32List_t *list = adt_u32List_new();
   CuAssertPtrNotNull(tc, list);

   // NULL self checks
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_u32List_insert(NULL, 10));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_u32List_insert_before(NULL, NULL, 10));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_u32List_insert_after(NULL, NULL, 10));

   // NULL iter checks for before/after
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_u32List_insert_before(list, NULL, 10));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_u32List_insert_after(list, NULL, 10));

   // Successful insertions return ADT_NO_ERROR
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32List_insert(list, 10));
   adt_u32List_elem_t *elem = adt_u32List_find(list, 10);
   CuAssertPtrNotNull(tc, elem);

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32List_insert_before(list, elem, 5));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u32List_insert_after(list, elem, 15));

   adt_u32List_delete(list);
}

