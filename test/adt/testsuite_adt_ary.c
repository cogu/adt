/*****************************************************************************
* \file      testsuite_adt_ary.c
* \author    Conny Gustafsson
* \date      2016-11-20
* \brief     Unit tests for adt_ary_t
*
* Copyright (c) 2016-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_ary.h"
#include "adt_str.h"
#include "test_common.h"
#include <assert.h>
#include <limits.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_adt_ary_new(CuTest *tc);
static void test_adt_ary_make(CuTest *tc);
static void test_adt_ary_push_pop(CuTest *tc);
static void test_adt_ary_push_nullptr(CuTest *tc);
static void test_adt_ary_unshift(CuTest *tc);
static void test_adt_ary_shift(CuTest *tc);
static void test_adt_ary_resize(CuTest *tc);
static void test_adt_ary_resize_shrink_destructor(CuTest *tc);
static void test_adt_ary_push_unique(CuTest *tc);
static void test_adt_ary_remove(CuTest *tc);
static void test_adt_ary_splice_to_empty(CuTest *tc);
static void test_adt_ary_splice_at_begin(CuTest *tc);
static void test_adt_ary_splice_at_end(CuTest *tc);
static void test_adt_ary_splice_at_middle(CuTest *tc);
static void test_adt_ary_sort_empty_list(CuTest *tc);
static void test_adt_ary_sort_array_with_one_item(CuTest *tc);
static void test_adt_ary_sort_array_with_seven_items(CuTest *tc);
static void test_adt_ary_reverse_sort_array_with_seven_items(CuTest *tc);
static void test_adt_ary_sort_strings_array_with_four_items(CuTest *tc);
static void test_adt_ary_sort_with_duplicates(CuTest *tc);
static void test_adt_ary_sort_all_identical(CuTest *tc);
static void test_adt_ary_sort_already_sorted(CuTest *tc);
static void test_adt_ary_sort_reversed_input(CuTest *tc);
static void test_adt_ary_sort_large_array(CuTest *tc);
static void test_adt_ary_sort_compare_error(CuTest *tc);
static void test_adt_ary_index_of(CuTest *tc);
static void test_adt_ary_destructor_enable(CuTest *tc);
static void test_adt_ary_vdelete(CuTest *tc);
static void test_adt_ary_destructor_query(CuTest *tc);
static void test_adt_ary_push_growth(CuTest *tc);
static void test_adt_ary_push_shift_compaction(CuTest *tc);
static void test_adt_ary_reserve(CuTest *tc);
static void test_adt_ary_unshift_growth(CuTest *tc);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////
static int m_numbers[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite *testsuite_adt_ary(void) {
  CuSuite *suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, test_adt_ary_new);
  SUITE_ADD_TEST(suite, test_adt_ary_make);
  SUITE_ADD_TEST(suite, test_adt_ary_push_pop);
  SUITE_ADD_TEST(suite, test_adt_ary_push_nullptr);
  SUITE_ADD_TEST(suite, test_adt_ary_splice_to_empty);
  SUITE_ADD_TEST(suite, test_adt_ary_splice_at_begin);
  SUITE_ADD_TEST(suite, test_adt_ary_splice_at_end);
  SUITE_ADD_TEST(suite, test_adt_ary_splice_at_middle);
  SUITE_ADD_TEST(suite, test_adt_ary_unshift);
  SUITE_ADD_TEST(suite, test_adt_ary_shift);
  SUITE_ADD_TEST(suite, test_adt_ary_resize);
  SUITE_ADD_TEST(suite, test_adt_ary_resize_shrink_destructor);
  SUITE_ADD_TEST(suite, test_adt_ary_push_unique);
  SUITE_ADD_TEST(suite, test_adt_ary_remove);
  SUITE_ADD_TEST(suite, test_adt_ary_sort_empty_list);
  SUITE_ADD_TEST(suite, test_adt_ary_sort_array_with_one_item);
  SUITE_ADD_TEST(suite, test_adt_ary_sort_array_with_seven_items);
  SUITE_ADD_TEST(suite, test_adt_ary_reverse_sort_array_with_seven_items);
  SUITE_ADD_TEST(suite, test_adt_ary_sort_strings_array_with_four_items);
  SUITE_ADD_TEST(suite, test_adt_ary_sort_with_duplicates);
  SUITE_ADD_TEST(suite, test_adt_ary_sort_all_identical);
  SUITE_ADD_TEST(suite, test_adt_ary_sort_already_sorted);
  SUITE_ADD_TEST(suite, test_adt_ary_sort_reversed_input);
  SUITE_ADD_TEST(suite, test_adt_ary_sort_large_array);
  SUITE_ADD_TEST(suite, test_adt_ary_sort_compare_error);
  SUITE_ADD_TEST(suite, test_adt_ary_index_of);
  SUITE_ADD_TEST(suite, test_adt_ary_destructor_enable);
  SUITE_ADD_TEST(suite, test_adt_ary_vdelete);
  SUITE_ADD_TEST(suite, test_adt_ary_destructor_query);
  SUITE_ADD_TEST(suite, test_adt_ary_push_growth);
  SUITE_ADD_TEST(suite, test_adt_ary_push_shift_compaction);
  SUITE_ADD_TEST(suite, test_adt_ary_reserve);
  SUITE_ADD_TEST(suite, test_adt_ary_unshift_growth);

  return suite;
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void test_adt_ary_new(CuTest *tc) {
  adt_ary_t *pArray = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, pArray);
  CuAssertPtrEquals(tc, NULL, pArray->ppAlloc);
  CuAssertFnPtrEquals(tc, NULL, pArray->pDestructor);
  CuAssertIntEquals(tc, 0, pArray->s32AllocLen);
  CuAssertIntEquals(tc, 0, pArray->s32CurLen);
  adt_ary_delete(pArray);
}

static void test_adt_ary_push_pop(CuTest *tc) {
  char *pVal;
  adt_ary_t *pArray = adt_ary_new(vfree);
  CuAssertPtrNotNull(tc, pArray);
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, STRDUP("The")));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, STRDUP("quick")));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, STRDUP("brown")));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, STRDUP("fox")));
  CuAssertPtrNotNull(tc, pArray->ppAlloc);
  CuAssertFnPtrEquals(tc, vfree, pArray->pDestructor);
  CuAssertIntEquals(tc, 4, pArray->s32AllocLen);
  CuAssertIntEquals(tc, 4, pArray->s32CurLen);
  CuAssertPtrNotNull(tc, (pVal = (char *)adt_ary_pop(pArray)));
  CuAssertStrEquals(tc, "fox", pVal);
  free(pVal);
  CuAssertIntEquals(tc, 3, adt_ary_length(pArray));
  CuAssertPtrNotNull(tc, (pVal = (char *)adt_ary_pop(pArray)));
  CuAssertStrEquals(tc, "brown", pVal);
  free(pVal);
  CuAssertIntEquals(tc, 2, adt_ary_length(pArray));
  CuAssertPtrNotNull(tc, (pVal = (char *)adt_ary_pop(pArray)));
  CuAssertStrEquals(tc, "quick", pVal);
  free(pVal);
  CuAssertIntEquals(tc, 1, adt_ary_length(pArray));
  CuAssertPtrNotNull(tc, (pVal = (char *)adt_ary_pop(pArray)));
  CuAssertStrEquals(tc, "The", pVal);
  free(pVal);
  CuAssertIntEquals(tc, 0, adt_ary_length(pArray));
  adt_ary_delete(pArray);
}

static void test_adt_ary_push_nullptr(CuTest *tc) {
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, NULL));
  CuAssertIntEquals(tc, 1, adt_ary_length(array));
  adt_ary_delete(array);
}

static void test_adt_ary_unshift(CuTest *tc) {
  void **ppVal;
  char *pVal;
  char *tmp;
  adt_ary_t *pArray = adt_ary_new(vfree);

  CuAssertPtrNotNull(tc, pArray);
  CuAssertIntEquals(tc, 0, adt_ary_length(pArray));
  CuAssertFnPtrEquals(tc, vfree, pArray->pDestructor);

  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_unshift(pArray, tmp = STRDUP("The")));
  CuAssertIntEquals(tc, 1, adt_ary_length(pArray));
  CuAssertPtrNotNull(tc, (ppVal = adt_ary_get(pArray, 0)));
  pVal = (char *)*ppVal;
  CuAssertPtrNotNull(tc, pVal);
  CuAssertStrEquals(tc, "The", pVal);

  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_unshift(pArray, tmp = STRDUP("quick")));
  CuAssertIntEquals(tc, 2, adt_ary_length(pArray));
  CuAssertPtrNotNull(tc, (ppVal = adt_ary_get(pArray, 0)));
  pVal = (char *)*ppVal;
  CuAssertPtrNotNull(tc, pVal);
  CuAssertStrEquals(tc, "quick", pVal);

  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_unshift(pArray, tmp = STRDUP("brown")));
  CuAssertIntEquals(tc, 3, adt_ary_length(pArray));
  CuAssertPtrNotNull(tc, (ppVal = adt_ary_get(pArray, 0)));
  pVal = (char *)*ppVal;
  CuAssertPtrNotNull(tc, pVal);
  CuAssertStrEquals(tc, "brown", pVal);

  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_unshift(pArray, tmp = STRDUP("fox")));
  CuAssertIntEquals(tc, 4, adt_ary_length(pArray));
  CuAssertPtrNotNull(tc, (ppVal = adt_ary_get(pArray, 0)));
  pVal = (char *)*ppVal;
  CuAssertPtrNotNull(tc, pVal);
  CuAssertStrEquals(tc, "fox", pVal);

  adt_ary_delete(pArray);
}

static void test_adt_ary_shift(CuTest *tc) {
  adt_ary_t *pArray = adt_ary_new(vfree);
  int *n1, *n2, *n3, *pVal;
  n1 = (int *)malloc(sizeof(int));
  n2 = (int *)malloc(sizeof(int));
  n3 = (int *)malloc(sizeof(int));
  *n1 = 1;
  *n1 = 2;
  *n1 = 3;
  CuAssertPtrNotNull(tc, pArray);
  CuAssertIntEquals(tc, 0, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, n1));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, n2));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, n3));
  CuAssertIntEquals(tc, 3, adt_ary_length(pArray));
  pVal = adt_ary_shift(pArray);
  CuAssertPtrEquals(tc, n1, pVal);
  CuAssertIntEquals(tc, 2, adt_ary_length(pArray));
  pVal = adt_ary_shift(pArray);
  CuAssertPtrEquals(tc, n2, pVal);
  CuAssertIntEquals(tc, 1, adt_ary_length(pArray));
  pVal = adt_ary_shift(pArray);
  CuAssertPtrEquals(tc, n3, pVal);
  CuAssertIntEquals(tc, 0, adt_ary_length(pArray));

  free(n1);
  free(n2);
  free(n3);
  adt_ary_delete(pArray);
}

static void test_adt_ary_resize(CuTest *tc) {
  int one = 1;
  int two = 2;
  int three = 3;
  int four = 4;
  int five = 4;
  int *p;
  adt_ary_t *a = adt_ary_new(NULL);
  adt_ary_push(a, &one);
  adt_ary_push(a, &two);
  adt_ary_push(a, &three);
  adt_ary_push(a, &four);
  adt_ary_push(a, &five);
  CuAssertIntEquals(tc, 5, adt_ary_length(a));
  adt_ary_resize(a, 3);
  CuAssertIntEquals(tc, 3, adt_ary_length(a));
  p = (int *)*adt_ary_get(a, 0);
  CuAssertPtrEquals(tc, &one, p);
  p = (int *)*adt_ary_get(a, 1);
  CuAssertPtrEquals(tc, &two, p);
  p = (int *)*adt_ary_get(a, 2);
  CuAssertPtrEquals(tc, &three, p);
  adt_ary_resize(a, 0);
  CuAssertIntEquals(tc, 0, adt_ary_length(a));
  adt_ary_push(a, &one);
  adt_ary_push(a, &two);
  adt_ary_push(a, &three);
  adt_ary_push(a, &four);
  adt_ary_push(a, &five);
  CuAssertIntEquals(tc, 5, adt_ary_length(a));
  adt_ary_resize(a, 5);
  CuAssertIntEquals(tc, 5, adt_ary_length(a));
  adt_ary_delete(a);
}

static void test_adt_ary_push_unique(CuTest *tc) {
  int *a;
  int *b;
  int *c;
  adt_ary_t *pArray = adt_ary_new(vfree);
  a = (int *)malloc(sizeof(int));
  b = (int *)malloc(sizeof(int));
  c = (int *)malloc(sizeof(int));
  CuAssertIntEquals(tc, 0, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push_unique(pArray, a));
  CuAssertIntEquals(tc, 1, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push_unique(pArray, a));
  CuAssertIntEquals(tc, 1, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push_unique(pArray, b));
  CuAssertIntEquals(tc, 2, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push_unique(pArray, b));
  CuAssertIntEquals(tc, 2, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push_unique(pArray, c));
  CuAssertIntEquals(tc, 3, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push_unique(pArray, c));
  CuAssertIntEquals(tc, 3, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push_unique(pArray, c));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push_unique(pArray, b));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push_unique(pArray, a));
  CuAssertIntEquals(tc, 3, adt_ary_length(pArray));
  adt_ary_delete(pArray);
}

static void test_adt_ary_remove(CuTest *tc) {
  int *a;
  int *b;
  int *c;
  adt_ary_t *pArray = adt_ary_new(vfree);
  a = (int *)malloc(sizeof(int));
  b = (int *)malloc(sizeof(int));
  c = (int *)malloc(sizeof(int));
  CuAssertIntEquals(tc, 0, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, a));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, b));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, c));
  CuAssertIntEquals(tc, 3, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_remove(pArray, a));
  CuAssertIntEquals(tc, 2, adt_ary_length(pArray));
  CuAssertPtrEquals(tc, b, adt_ary_value(pArray, 0));
  CuAssertPtrEquals(tc, c, adt_ary_value(pArray, 1));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_remove(pArray, c));
  CuAssertIntEquals(tc, 1, adt_ary_length(pArray));
  CuAssertPtrEquals(tc, b, adt_ary_value(pArray, 0));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_remove(pArray, b));
  CuAssertIntEquals(tc, 0, adt_ary_length(pArray));
  CuAssertIntEquals(tc, 4, pArray->s32AllocLen);
  adt_ary_delete(pArray);
}

static void test_adt_ary_splice_to_empty(CuTest *tc) {

  adt_ary_t *pArray = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, pArray);
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[0]));
  CuAssertIntEquals(tc, 1, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_splice(pArray, 0, 1));
  CuAssertIntEquals(tc, 0, adt_ary_length(pArray));
  adt_ary_delete(pArray);
}

static void test_adt_ary_splice_at_begin(CuTest *tc) {

  adt_ary_t *pArray = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, pArray);
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[0]));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[1]));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[2]));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[3]));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[4]));
  CuAssertIntEquals(tc, 5, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_splice(pArray, 0, 1));
  CuAssertIntEquals(tc, 4, adt_ary_length(pArray));
  CuAssertPtrEquals(tc, &m_numbers[1], adt_ary_value(pArray, 0));
  CuAssertPtrEquals(tc, &m_numbers[2], adt_ary_value(pArray, 1));
  CuAssertPtrEquals(tc, &m_numbers[3], adt_ary_value(pArray, 2));
  CuAssertPtrEquals(tc, &m_numbers[4], adt_ary_value(pArray, 3));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_splice(pArray, 0, 2));
  CuAssertIntEquals(tc, 2, adt_ary_length(pArray));
  CuAssertPtrEquals(tc, &m_numbers[3], adt_ary_value(pArray, 0));
  CuAssertPtrEquals(tc, &m_numbers[4], adt_ary_value(pArray, 1));
  adt_ary_delete(pArray);
}

static void test_adt_ary_splice_at_end(CuTest *tc) {

  adt_ary_t *pArray = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, pArray);
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[0]));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[1]));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[2]));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[3]));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[4]));
  CuAssertIntEquals(tc, 5, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_splice(pArray, 4, 1));
  CuAssertIntEquals(tc, 4, adt_ary_length(pArray));
  CuAssertPtrEquals(tc, &m_numbers[0], adt_ary_value(pArray, 0));
  CuAssertPtrEquals(tc, &m_numbers[1], adt_ary_value(pArray, 1));
  CuAssertPtrEquals(tc, &m_numbers[2], adt_ary_value(pArray, 2));
  CuAssertPtrEquals(tc, &m_numbers[3], adt_ary_value(pArray, 3));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_splice(pArray, 2, 2));
  CuAssertIntEquals(tc, 2, adt_ary_length(pArray));
  CuAssertPtrEquals(tc, &m_numbers[0], adt_ary_value(pArray, 0));
  CuAssertPtrEquals(tc, &m_numbers[1], adt_ary_value(pArray, 1));
  adt_ary_delete(pArray);
}

static void test_adt_ary_splice_at_middle(CuTest *tc) {
  int *n1, *n2, *n3;
  adt_ary_t *pArray = adt_ary_new(vfree);
  n1 = (int *)malloc(sizeof(int));
  n2 = (int *)malloc(sizeof(int));
  n3 = (int *)malloc(sizeof(int));
  *n1 = 1;
  *n1 = 2;
  *n1 = 3;
  CuAssertPtrNotNull(tc, pArray);
  CuAssertIntEquals(tc, 0, adt_ary_length(pArray));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, n1));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, n2));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, n3));
  CuAssertIntEquals(tc, 3, adt_ary_length(pArray));

  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_splice(pArray, 1, 1));
  CuAssertIntEquals(tc, 2, adt_ary_length(pArray));
  CuAssertPtrEquals(tc, n1, adt_ary_value(pArray, 0));
  CuAssertPtrEquals(tc, n3, adt_ary_value(pArray, 1));

  adt_ary_delete(pArray);
}

static void test_adt_ary_sort_empty_list(CuTest *tc) {
  adt_ary_t *array = adt_ary_new(NULL);

  CuAssertPtrNotNull(tc, array);
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, false));

  adt_ary_delete(array);
}

static void test_adt_ary_sort_array_with_one_item(CuTest *tc) {
  int32_t value = 5;
  adt_ary_t *array = adt_ary_new(NULL);

  CuAssertPtrNotNull(tc, array);
  adt_ary_push(array, &value);
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, false));
  CuAssertIntEquals(tc, 5, *((int32_t *)adt_ary_value(array, 0)));
  adt_ary_delete(array);
}

static void test_adt_ary_sort_array_with_seven_items(CuTest *tc) {
  int32_t i;
  int32_t values[] = {7, 8, 5, 2, 4, 6, 3};
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);
  for (i = 0; i < 7; i++) {
    adt_ary_push(array, &values[i]);
  }
  CuAssertIntEquals(tc, 7, adt_ary_length(array));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, false));
  CuAssertIntEquals(tc, 2, *((int32_t *)adt_ary_value(array, 0)));
  CuAssertIntEquals(tc, 3, *((int32_t *)adt_ary_value(array, 1)));
  CuAssertIntEquals(tc, 4, *((int32_t *)adt_ary_value(array, 2)));
  CuAssertIntEquals(tc, 5, *((int32_t *)adt_ary_value(array, 3)));
  CuAssertIntEquals(tc, 6, *((int32_t *)adt_ary_value(array, 4)));
  CuAssertIntEquals(tc, 7, *((int32_t *)adt_ary_value(array, 5)));
  CuAssertIntEquals(tc, 8, *((int32_t *)adt_ary_value(array, 6)));
  adt_ary_delete(array);
}

static void test_adt_ary_reverse_sort_array_with_seven_items(CuTest *tc) {
  int32_t i;
  int32_t values[] = {7, 8, 5, 2, 4, 6, 3};
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);
  for (i = 0; i < 7; i++) {
    adt_ary_push(array, &values[i]);
  }
  CuAssertIntEquals(tc, 7, adt_ary_length(array));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, true));
  CuAssertIntEquals(tc, 8, *((int32_t *)adt_ary_value(array, 0)));
  CuAssertIntEquals(tc, 7, *((int32_t *)adt_ary_value(array, 1)));
  CuAssertIntEquals(tc, 6, *((int32_t *)adt_ary_value(array, 2)));
  CuAssertIntEquals(tc, 5, *((int32_t *)adt_ary_value(array, 3)));
  CuAssertIntEquals(tc, 4, *((int32_t *)adt_ary_value(array, 4)));
  CuAssertIntEquals(tc, 3, *((int32_t *)adt_ary_value(array, 5)));
  CuAssertIntEquals(tc, 2, *((int32_t *)adt_ary_value(array, 6)));
  adt_ary_delete(array);
}

static void test_adt_ary_sort_strings_array_with_four_items(CuTest *tc) {
  const char *green = "green";
  const char *purple = "purple";
  const char *blue = "blue";
  const char *black = "black";

  adt_ary_t *array = adt_ary_new(adt_str_vdelete);
  CuAssertPtrNotNull(tc, array);
  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_push(array, adt_str_new_cstr(green)));
  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_push(array, adt_str_new_cstr(purple)));
  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_push(array, adt_str_new_cstr(blue)));
  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_push(array, adt_str_new_cstr(black)));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_str_vlt, false));
  CuAssertStrEquals(tc, "black",
                    adt_str_cstr((adt_str_t *)adt_ary_value(array, 0)));
  CuAssertStrEquals(tc, "blue",
                    adt_str_cstr((adt_str_t *)adt_ary_value(array, 1)));
  CuAssertStrEquals(tc, "green",
                    adt_str_cstr((adt_str_t *)adt_ary_value(array, 2)));
  CuAssertStrEquals(tc, "purple",
                    adt_str_cstr((adt_str_t *)adt_ary_value(array, 3)));
  adt_ary_delete(array);
}

static void test_adt_ary_sort_with_duplicates(CuTest *tc) {
  int32_t values[] = {5, 1, 3, 5, 2, 1, 4, 3};
  int32_t count = sizeof(values) / sizeof(values[0]);
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);

  for (int32_t i = 0; i < count; i++) {
    adt_ary_push(array, &values[i]);
  }
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, false));
  CuAssertIntEquals(tc, 1, *((int32_t *)adt_ary_value(array, 0)));
  CuAssertIntEquals(tc, 1, *((int32_t *)adt_ary_value(array, 1)));
  CuAssertIntEquals(tc, 2, *((int32_t *)adt_ary_value(array, 2)));
  CuAssertIntEquals(tc, 3, *((int32_t *)adt_ary_value(array, 3)));
  CuAssertIntEquals(tc, 3, *((int32_t *)adt_ary_value(array, 4)));
  CuAssertIntEquals(tc, 4, *((int32_t *)adt_ary_value(array, 5)));
  CuAssertIntEquals(tc, 5, *((int32_t *)adt_ary_value(array, 6)));
  CuAssertIntEquals(tc, 5, *((int32_t *)adt_ary_value(array, 7)));

  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, true));
  CuAssertIntEquals(tc, 5, *((int32_t *)adt_ary_value(array, 0)));
  CuAssertIntEquals(tc, 5, *((int32_t *)adt_ary_value(array, 1)));
  CuAssertIntEquals(tc, 4, *((int32_t *)adt_ary_value(array, 2)));
  CuAssertIntEquals(tc, 3, *((int32_t *)adt_ary_value(array, 3)));
  CuAssertIntEquals(tc, 3, *((int32_t *)adt_ary_value(array, 4)));
  CuAssertIntEquals(tc, 2, *((int32_t *)adt_ary_value(array, 5)));
  CuAssertIntEquals(tc, 1, *((int32_t *)adt_ary_value(array, 6)));
  CuAssertIntEquals(tc, 1, *((int32_t *)adt_ary_value(array, 7)));

  adt_ary_delete(array);
}

static void test_adt_ary_sort_all_identical(CuTest *tc) {
  int32_t val = 42;
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);

  for (int32_t i = 0; i < 20; i++) {
    adt_ary_push(array, &val);
  }
  CuAssertIntEquals(tc, 20, adt_ary_length(array));
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, false));
  for (int32_t i = 0; i < 20; i++) {
    CuAssertIntEquals(tc, 42, *((int32_t *)adt_ary_value(array, i)));
  }

  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, true));
  for (int32_t i = 0; i < 20; i++) {
    CuAssertIntEquals(tc, 42, *((int32_t *)adt_ary_value(array, i)));
  }

  adt_ary_delete(array);
}

static void test_adt_ary_sort_already_sorted(CuTest *tc) {
  int32_t values[] = {10, 20, 30, 40, 50, 60, 70, 80};
  int32_t count = sizeof(values) / sizeof(values[0]);
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);

  for (int32_t i = 0; i < count; i++) {
    adt_ary_push(array, &values[i]);
  }
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, false));
  for (int32_t i = 0; i < count; i++) {
    CuAssertIntEquals(tc, values[i], *((int32_t *)adt_ary_value(array, i)));
  }

  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, true));
  for (int32_t i = 0; i < count; i++) {
    CuAssertIntEquals(tc, values[count - 1 - i], *((int32_t *)adt_ary_value(array, i)));
  }

  adt_ary_delete(array);
}

static void test_adt_ary_sort_reversed_input(CuTest *tc) {
  int32_t values[] = {80, 70, 60, 50, 40, 30, 20, 10};
  int32_t count = sizeof(values) / sizeof(values[0]);
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);

  for (int32_t i = 0; i < count; i++) {
    adt_ary_push(array, &values[i]);
  }
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, false));
  for (int32_t i = 0; i < count; i++) {
    CuAssertIntEquals(tc, (i + 1) * 10, *((int32_t *)adt_ary_value(array, i)));
  }

  adt_ary_delete(array);
}

static void test_adt_ary_sort_large_array(CuTest *tc) {
  const int32_t count = 250;
  int32_t values[250];
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);

  // Pseudo-random distribution
  uint32_t seed = 12345;
  for (int32_t i = 0; i < count; i++) {
    seed = seed * 1103515245 + 12345;
    values[i] = (int32_t)(seed % 1000);
    adt_ary_push(array, &values[i]);
  }
  CuAssertIntEquals(tc, count, adt_ary_length(array));

  // Ascending sort
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, false));
  for (int32_t i = 0; i < count - 1; i++) {
    int32_t a = *((int32_t *)adt_ary_value(array, i));
    int32_t b = *((int32_t *)adt_ary_value(array, i + 1));
    CuAssertTrue(tc, a <= b);
  }

  // Descending sort
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, true));
  for (int32_t i = 0; i < count - 1; i++) {
    int32_t a = *((int32_t *)adt_ary_value(array, i));
    int32_t b = *((int32_t *)adt_ary_value(array, i + 1));
    CuAssertTrue(tc, a >= b);
  }

  adt_ary_delete(array);
}

static int mock_failing_comparator(const void *a, const void *b) {
  (void)a;
  (void)b;
  return -1;
}

static void test_adt_ary_sort_compare_error(CuTest *tc) {
  int32_t values[] = {3, 1, 2};
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);

  for (int32_t i = 0; i < 3; i++) {
    adt_ary_push(array, &values[i]);
  }
  CuAssertIntEquals(tc, ADT_OBJECT_COMPARE_ERROR, adt_ary_sort(array, mock_failing_comparator, false));

  adt_ary_delete(array);
}

static void test_adt_ary_index_of(CuTest *tc) {
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);
  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_push(array, &m_numbers[0])); // index 0
  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_push(array, &m_numbers[1])); // index 1
  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_push(array, &m_numbers[2])); // index 2
  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_push(array, &m_numbers[3])); // index 3
  CuAssertIntEquals(tc, ADT_NO_ERROR,
                    adt_ary_push(array, &m_numbers[4])); // index 4
  CuAssertIntEquals(tc, 5, adt_ary_length(array));

  CuAssertIntEquals(tc, 0, adt_ary_index_of(array, &m_numbers[0]));
  CuAssertIntEquals(tc, 1, adt_ary_index_of(array, &m_numbers[1]));
  CuAssertIntEquals(tc, 2, adt_ary_index_of(array, &m_numbers[2]));
  CuAssertIntEquals(tc, 3, adt_ary_index_of(array, &m_numbers[3]));
  CuAssertIntEquals(tc, 4, adt_ary_index_of(array, &m_numbers[4]));
  CuAssertIntEquals(tc, -1, adt_ary_index_of(array, &m_numbers[5]));
  CuAssertIntEquals(tc, -1, adt_ary_index_of(array, &m_numbers[6]));

  adt_ary_delete(array);
}

static void test_adt_ary_make(CuTest *tc) {
  CuAssertPtrEquals(tc, NULL, adt_ary_make(NULL, 5, NULL));
  CuAssertPtrEquals(tc, NULL,
                    adt_ary_make((void **)m_numbers, INT32_MAX, NULL));

  void *items[3];
  items[0] = STRDUP("apple");
  items[1] = STRDUP("banana");
  items[2] = STRDUP("cherry");

  adt_ary_t *array = adt_ary_make(items, 3, vfree);
  CuAssertPtrNotNull(tc, array);
  CuAssertIntEquals(tc, 3, adt_ary_length(array));
  CuAssertStrEquals(tc, "apple", (char *)adt_ary_value(array, 0));
  CuAssertStrEquals(tc, "banana", (char *)adt_ary_value(array, 1));
  CuAssertStrEquals(tc, "cherry", (char *)adt_ary_value(array, 2));

  adt_ary_delete(array);
}

static void test_adt_ary_resize_shrink_destructor(CuTest *tc) {
  adt_ary_t *array = adt_ary_new(vfree);
  CuAssertPtrNotNull(tc, array);

  adt_ary_push(array, STRDUP("one"));
  adt_ary_push(array, STRDUP("two"));
  adt_ary_push(array, STRDUP("three"));
  adt_ary_push(array, STRDUP("four"));
  CuAssertIntEquals(tc, 4, adt_ary_length(array));

  // Shrink array from 4 to 2 (items "three" and "four" will be destroyed via
  // vfree)
  adt_ary_resize(array, 2);
  CuAssertIntEquals(tc, 2, adt_ary_length(array));
  CuAssertStrEquals(tc, "one", (char *)adt_ary_value(array, 0));
  CuAssertStrEquals(tc, "two", (char *)adt_ary_value(array, 1));

  adt_ary_delete(array);
}

static void test_adt_ary_destructor_enable(CuTest *tc) {
  adt_ary_t *array = adt_ary_new(vfree);
  adt_ary_destructor_enable(array, false);

  char static_item[] = "static";
  adt_ary_push(array, static_item);
  CuAssertIntEquals(tc, 1, adt_ary_length(array));

  adt_ary_delete(array);
}

static void test_adt_ary_vdelete(CuTest *tc) {
  adt_ary_t *outer = adt_ary_new(adt_ary_vdelete);
  adt_ary_t *inner1 = adt_ary_new(vfree);
  adt_ary_t *inner2 = adt_ary_new(vfree);

  adt_ary_push(inner1, STRDUP("inner1_item"));
  adt_ary_push(inner2, STRDUP("inner2_item"));

  adt_ary_push(outer, inner1);
  adt_ary_push(outer, inner2);
  CuAssertIntEquals(tc, 2, adt_ary_length(outer));

  // adt_ary_delete will invoke adt_ary_vdelete on inner1 and inner2
  adt_ary_delete(outer);
}

static void test_adt_ary_destructor_query(CuTest *tc) {
  // NULL array checks
  CuAssertTrue(tc, !adt_ary_has_destructor(NULL));
  CuAssertTrue(tc, !adt_ary_destructor_is_enabled(NULL));

  // Array with NULL destructor
  adt_ary_t *array_no_destructor = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array_no_destructor);
  CuAssertTrue(tc, !adt_ary_has_destructor(array_no_destructor));
  CuAssertTrue(tc, !adt_ary_destructor_is_enabled(array_no_destructor));

  // Even if enable is called, without a destructor callback it remains not enabled
  adt_ary_destructor_enable(array_no_destructor, true);
  CuAssertTrue(tc, !adt_ary_destructor_is_enabled(array_no_destructor));
  adt_ary_delete(array_no_destructor);

  // Array with destructor callback
  adt_ary_t *array_with_destructor = adt_ary_new(vfree);
  CuAssertPtrNotNull(tc, array_with_destructor);
  CuAssertTrue(tc, adt_ary_has_destructor(array_with_destructor));
  CuAssertTrue(tc, adt_ary_destructor_is_enabled(array_with_destructor));

  // Disable destructor
  adt_ary_destructor_enable(array_with_destructor, false);
  CuAssertTrue(tc, adt_ary_has_destructor(array_with_destructor));
  CuAssertTrue(tc, !adt_ary_destructor_is_enabled(array_with_destructor));

  // Re-enable destructor
  adt_ary_destructor_enable(array_with_destructor, true);
  CuAssertTrue(tc, adt_ary_has_destructor(array_with_destructor));
  CuAssertTrue(tc, adt_ary_destructor_is_enabled(array_with_destructor));

  adt_ary_delete(array_with_destructor);
}

static void test_adt_ary_push_growth(CuTest *tc) {
  const int count = 1000;
  int values[1000];
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);

  for (int i = 0; i < count; i++) {
    values[i] = i;
    CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, &values[i]));
  }
  CuAssertIntEquals(tc, count, adt_ary_length(array));
  CuAssertTrue(tc, array->s32AllocLen >= count);
  // With min capacity 4 and doubling, 1000 elements results in capacity 1024
  CuAssertIntEquals(tc, 1024, array->s32AllocLen);

  for (int i = 0; i < count; i++) {
    CuAssertIntEquals(tc, i, *((int *)adt_ary_value(array, i)));
  }

  adt_ary_delete(array);
}

static void test_adt_ary_push_shift_compaction(CuTest *tc) {
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);

  for (int i = 0; i < 4; i++) {
    CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, &m_numbers[i]));
  }
  CuAssertIntEquals(tc, 4, array->s32AllocLen);
  CuAssertIntEquals(tc, 4, adt_ary_length(array));

  // Shift 2 elements off the front
  void *elem0 = adt_ary_shift(array);
  void *elem1 = adt_ary_shift(array);
  CuAssertPtrEquals(tc, &m_numbers[0], elem0);
  CuAssertPtrEquals(tc, &m_numbers[1], elem1);
  CuAssertIntEquals(tc, 2, adt_ary_length(array));
  CuAssertIntEquals(tc, 4, array->s32AllocLen);

  // Pushing one element triggers shift compaction because back is full, but front has 2 free slots
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, &m_numbers[4]));
  CuAssertIntEquals(tc, 3, adt_ary_length(array));
  // Capacity remains 4 (no reallocation!)
  CuAssertIntEquals(tc, 4, array->s32AllocLen);
  CuAssertPtrEquals(tc, array->ppAlloc, array->pFirst);

  CuAssertPtrEquals(tc, &m_numbers[2], adt_ary_value(array, 0));
  CuAssertPtrEquals(tc, &m_numbers[3], adt_ary_value(array, 1));
  CuAssertPtrEquals(tc, &m_numbers[4], adt_ary_value(array, 2));

  // Push another element to fill capacity 4
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, &m_numbers[5]));
  CuAssertIntEquals(tc, 4, array->s32AllocLen);

  // Push 5th element: now buffer is truly full, grows to 8
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, &m_numbers[6]));
  CuAssertIntEquals(tc, 5, adt_ary_length(array));
  CuAssertIntEquals(tc, 8, array->s32AllocLen);

  adt_ary_delete(array);
}

static void test_adt_ary_reserve(CuTest *tc) {
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);

  CuAssertIntEquals(tc, 0, array->s32AllocLen);
  CuAssertIntEquals(tc, 0, adt_ary_length(array));

  // Preallocate capacity
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_reserve(array, 100));
  CuAssertIntEquals(tc, 100, array->s32AllocLen);
  CuAssertIntEquals(tc, 0, adt_ary_length(array));

  // Reserving smaller or equal capacity is a no-op
  CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_reserve(array, 50));
  CuAssertIntEquals(tc, 100, array->s32AllocLen);

  // Push elements into reserved capacity
  for (int i = 0; i < 50; i++) {
    CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, &m_numbers[i % 10]));
  }
  CuAssertIntEquals(tc, 50, adt_ary_length(array));
  CuAssertIntEquals(tc, 100, array->s32AllocLen);

  // Invalid arguments
  CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_ary_reserve(NULL, 10));
  CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_ary_reserve(array, -1));

  adt_ary_delete(array);
}

static void test_adt_ary_unshift_growth(CuTest *tc) {
  adt_ary_t *array = adt_ary_new(NULL);
  CuAssertPtrNotNull(tc, array);

  for (int i = 0; i < 10; i++) {
    CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_unshift(array, &m_numbers[i]));
  }
  CuAssertIntEquals(tc, 10, adt_ary_length(array));
  CuAssertTrue(tc, array->s32AllocLen >= 10);
  CuAssertIntEquals(tc, 16, array->s32AllocLen);

  // Elements should be in reverse order
  for (int i = 0; i < 10; i++) {
    CuAssertPtrEquals(tc, &m_numbers[9 - i], adt_ary_value(array, i));
  }

  adt_ary_delete(array);
}
