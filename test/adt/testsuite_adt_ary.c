/*****************************************************************************
* \file      testsuite_adt_ary.c
* \author    Conny Gustafsson
* \date      2016-11-20
* \brief     Unit tests for adt_ary_t
*
* Copyright (c) 2016-2020 Conny Gustafsson
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
#include <malloc.h>
#include "CuTest.h"
#include "adt_ary.h"
#include "adt_str.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
void vfree(void* p);
#else
#define vfree free
#endif



//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_adt_ary_new(CuTest* tc);
static void test_adt_ary_push_pop(CuTest* tc);
static void test_adt_ary_unshift(CuTest* tc);
static void test_adt_ary_shift(CuTest* tc);
static void test_adt_ary_resize(CuTest* tc);
static void test_adt_ary_push_unique(CuTest* tc);
static void test_adt_ary_remove(CuTest* tc);
static void test_adt_ary_splice_to_empty(CuTest* tc);
static void test_adt_ary_splice_at_begin(CuTest* tc);
static void test_adt_ary_splice_at_end(CuTest* tc);
static void test_adt_ary_splice_at_middle(CuTest* tc);
static void test_adt_ary_sort_empty_list(CuTest* tc);
static void test_adt_ary_sort_array_with_one_item(CuTest* tc);
static void test_adt_ary_sort_array_with_seven_items(CuTest* tc);
static void test_adt_ary_reverse_sort_array_with_seven_items(CuTest* tc);
static void test_adt_ary_sort_strings_array_with_four_items(CuTest* tc);
static void test_adt_ary_indexOf(CuTest* tc);



//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////
static int m_numbers[10] = {0,1,2,3,4,5,6,7,8,9};
//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_adt_ary(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_ary_new);
   SUITE_ADD_TEST(suite, test_adt_ary_push_pop);
   SUITE_ADD_TEST(suite, test_adt_ary_splice_to_empty);
   SUITE_ADD_TEST(suite, test_adt_ary_splice_at_begin);
   SUITE_ADD_TEST(suite, test_adt_ary_splice_at_end);
   SUITE_ADD_TEST(suite, test_adt_ary_splice_at_middle);
   SUITE_ADD_TEST(suite, test_adt_ary_unshift);
   SUITE_ADD_TEST(suite, test_adt_ary_shift);
   SUITE_ADD_TEST(suite, test_adt_ary_resize);
   SUITE_ADD_TEST(suite, test_adt_ary_push_unique);
   SUITE_ADD_TEST(suite, test_adt_ary_remove);
   SUITE_ADD_TEST(suite, test_adt_ary_sort_empty_list);
   SUITE_ADD_TEST(suite, test_adt_ary_sort_array_with_one_item);
   SUITE_ADD_TEST(suite, test_adt_ary_sort_array_with_seven_items);
   SUITE_ADD_TEST(suite, test_adt_ary_reverse_sort_array_with_seven_items);
   SUITE_ADD_TEST(suite, test_adt_ary_sort_strings_array_with_four_items);
   SUITE_ADD_TEST(suite, test_adt_ary_indexOf);

   return suite;
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void test_adt_ary_new(CuTest* tc)
{
	adt_ary_t *pArray = adt_ary_new(NULL);
	CuAssertPtrNotNull(tc, pArray);
	CuAssertPtrEquals(tc, 0,pArray->ppAlloc);
	CuAssertPtrEquals(tc, 0,pArray->pDestructor);
	CuAssertIntEquals(tc,0,pArray->s32AllocLen);
	CuAssertIntEquals(tc,0,pArray->s32CurLen);
	adt_ary_delete(pArray);
}

static void test_adt_ary_push_pop(CuTest* tc)
{
	char *pVal;
	adt_ary_t *pArray = adt_ary_new(vfree);
	CuAssertPtrNotNull(tc, pArray);
	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray,strdup("The")));
	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray,strdup("quick")));
	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray,strdup("brown")));
	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray,strdup("fox")));
	CuAssertPtrNotNull(tc,pArray->ppAlloc);
	CuAssertPtrEquals(tc, vfree,pArray->pDestructor);
	CuAssertIntEquals(tc,4,pArray->s32AllocLen);
	CuAssertIntEquals(tc,4,pArray->s32CurLen);
	CuAssertPtrNotNull(tc,(pVal = (char*) adt_ary_pop(pArray)));
	CuAssertStrEquals(tc, "fox",pVal);
	free(pVal);
	CuAssertIntEquals(tc,3,adt_ary_length(pArray));
	CuAssertPtrNotNull(tc,(pVal = (char*) adt_ary_pop(pArray)));
	CuAssertStrEquals(tc, "brown",pVal);
	free(pVal);
	CuAssertIntEquals(tc,2,adt_ary_length(pArray));
	CuAssertPtrNotNull(tc,(pVal = (char*) adt_ary_pop(pArray)));
	CuAssertStrEquals(tc, "quick",pVal);
	free(pVal);
	CuAssertIntEquals(tc,1,adt_ary_length(pArray));
	CuAssertPtrNotNull(tc,(pVal = (char*) adt_ary_pop(pArray)));
	CuAssertStrEquals(tc, "The",pVal);
	free(pVal);
	CuAssertIntEquals(tc,0,adt_ary_length(pArray));
	adt_ary_delete(pArray);
}

static void test_adt_ary_unshift(CuTest* tc)
{
	void **ppVal;
	char *pVal;
	char *tmp;
	adt_ary_t *pArray = adt_ary_new(vfree);

	CuAssertPtrNotNull(tc, pArray);
	CuAssertIntEquals(tc,0,adt_ary_length(pArray));
	CuAssertPtrEquals(tc, vfree,pArray->pDestructor);

	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_unshift(pArray,tmp=strdup("The")));
	CuAssertIntEquals(tc,1,adt_ary_length(pArray));
	CuAssertPtrNotNull(tc, (ppVal = adt_ary_get(pArray,0)));
	pVal = (char*) *ppVal;
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "The",pVal);

	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_unshift(pArray,tmp=strdup("quick")));
	CuAssertIntEquals(tc,2,adt_ary_length(pArray));
	CuAssertPtrNotNull(tc, (ppVal = adt_ary_get(pArray,0)));
	pVal = (char*) *ppVal;
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "quick",pVal);

	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_unshift(pArray,tmp=strdup("brown")));
	CuAssertIntEquals(tc,3,adt_ary_length(pArray));
	CuAssertPtrNotNull(tc, (ppVal = adt_ary_get(pArray,0)));
	pVal = (char*) *ppVal;
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "brown",pVal);

	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_unshift(pArray,tmp=strdup("fox")));
	CuAssertIntEquals(tc,4,adt_ary_length(pArray));
	CuAssertPtrNotNull(tc, (ppVal = adt_ary_get(pArray,0)));
	pVal = (char*) *ppVal;
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "fox",pVal);

	adt_ary_delete(pArray);
}

static void test_adt_ary_shift(CuTest* tc)
{
   adt_ary_t *pArray = adt_ary_new(vfree);
   int *n1, *n2, *n3, *pVal;
   n1 = (int*) malloc(sizeof(int));
   n2 = (int*) malloc(sizeof(int));
   n3 = (int*) malloc(sizeof(int));
   *n1=1;
   *n1=2;
   *n1=3;
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

static void test_adt_ary_resize(CuTest* tc)
{
   int one = 1;
   int two = 2;
   int three = 3;
   int four = 4;
   int five = 4;
   int *p;
   adt_ary_t *a = adt_ary_new(0);
   adt_ary_push(a,&one);
   adt_ary_push(a,&two);
   adt_ary_push(a,&three);
   adt_ary_push(a,&four);
   adt_ary_push(a,&five);
   CuAssertIntEquals(tc,5,adt_ary_length(a));
   adt_ary_resize(a,3);
   CuAssertIntEquals(tc,3,adt_ary_length(a));
   p = (int*) *adt_ary_get(a,0);
   CuAssertPtrEquals(tc,&one,p);
   p = (int*) *adt_ary_get(a,1);
   CuAssertPtrEquals(tc,&two,p);
   p = (int*) *adt_ary_get(a,2);
   CuAssertPtrEquals(tc,&three,p);
   adt_ary_resize(a,0);
   CuAssertIntEquals(tc,0,adt_ary_length(a));
   adt_ary_push(a,&one);
   adt_ary_push(a,&two);
   adt_ary_push(a,&three);
   adt_ary_push(a,&four);
   adt_ary_push(a,&five);
   CuAssertIntEquals(tc,5,adt_ary_length(a));
   adt_ary_resize(a,5);
   CuAssertIntEquals(tc,5,adt_ary_length(a));
   adt_ary_delete(a);
}

static void test_adt_ary_push_unique(CuTest* tc)
{
   int *a;
   int *b;
   int *c;
   adt_ary_t *pArray = adt_ary_new(vfree);
   a = (int*) malloc(sizeof(int));
   b = (int*) malloc(sizeof(int));
   c = (int*) malloc(sizeof(int));
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

static void test_adt_ary_remove(CuTest* tc)
{
   int *a;
   int *b;
   int *c;
   adt_ary_t *pArray = adt_ary_new(vfree);
   a = (int*) malloc(sizeof(int));
   b = (int*) malloc(sizeof(int));
   c = (int*) malloc(sizeof(int));
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
   CuAssertIntEquals(tc, 3, pArray->s32AllocLen);
   adt_ary_delete(pArray);
}

static void test_adt_ary_splice_to_empty(CuTest* tc)
{

   adt_ary_t *pArray = adt_ary_new(NULL);
   CuAssertPtrNotNull(tc, pArray);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray, &m_numbers[0]));
   CuAssertIntEquals(tc, 1, adt_ary_length(pArray));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_splice(pArray, 0, 1));
   CuAssertIntEquals(tc, 0, adt_ary_length(pArray));
   adt_ary_delete(pArray);
}

static void test_adt_ary_splice_at_begin(CuTest* tc)
{

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

static void test_adt_ary_splice_at_end(CuTest* tc)
{

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

static void test_adt_ary_splice_at_middle(CuTest* tc)
{
   int *n1, *n2, *n3;
   adt_ary_t *pArray = adt_ary_new(vfree);
   n1 = (int*) malloc(sizeof(int));
   n2 = (int*) malloc(sizeof(int));
   n3 = (int*) malloc(sizeof(int));
   *n1=1;
   *n1=2;
   *n1=3;
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


static void test_adt_ary_sort_empty_list(CuTest* tc)
{
   adt_ary_t *array = adt_ary_new(NULL);

   CuAssertPtrNotNull(tc, array);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, false));

   adt_ary_delete(array);
}

static void test_adt_ary_sort_array_with_one_item(CuTest* tc)
{
   int32_t value = 5;
   adt_ary_t *array = adt_ary_new(NULL);

   CuAssertPtrNotNull(tc, array);
   adt_ary_push(array, &value);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, false));
   CuAssertIntEquals(tc, 5, *((int32_t*) adt_ary_value(array, 0)));
   adt_ary_delete(array);

}

static void test_adt_ary_sort_array_with_seven_items(CuTest* tc)
{
   int32_t i;
   int32_t values[] = {7, 8, 5, 2, 4, 6, 3};
   adt_ary_t *array = adt_ary_new(NULL);
   CuAssertPtrNotNull(tc, array);
   for(i=0; i<7; i++)
   {
      adt_ary_push(array, &values[i]);
   }
   CuAssertIntEquals(tc, 7, adt_ary_length(array));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, false));
   CuAssertIntEquals(tc, 2, *((int32_t*) adt_ary_value(array, 0)));
   CuAssertIntEquals(tc, 3, *((int32_t*) adt_ary_value(array, 1)));
   CuAssertIntEquals(tc, 4, *((int32_t*) adt_ary_value(array, 2)));
   CuAssertIntEquals(tc, 5, *((int32_t*) adt_ary_value(array, 3)));
   CuAssertIntEquals(tc, 6, *((int32_t*) adt_ary_value(array, 4)));
   CuAssertIntEquals(tc, 7, *((int32_t*) adt_ary_value(array, 5)));
   CuAssertIntEquals(tc, 8, *((int32_t*) adt_ary_value(array, 6)));
   adt_ary_delete(array);

}

static void test_adt_ary_reverse_sort_array_with_seven_items(CuTest* tc)
{
   int32_t i;
   int32_t values[] = {7, 8, 5, 2, 4, 6, 3};
   adt_ary_t *array = adt_ary_new(NULL);
   CuAssertPtrNotNull(tc, array);
   for(i=0; i<7; i++)
   {
      adt_ary_push(array, &values[i]);
   }
   CuAssertIntEquals(tc, 7, adt_ary_length(array));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_i32_vlt, true));
   CuAssertIntEquals(tc, 8, *((int32_t*) adt_ary_value(array, 0)));
   CuAssertIntEquals(tc, 7, *((int32_t*) adt_ary_value(array, 1)));
   CuAssertIntEquals(tc, 6, *((int32_t*) adt_ary_value(array, 2)));
   CuAssertIntEquals(tc, 5, *((int32_t*) adt_ary_value(array, 3)));
   CuAssertIntEquals(tc, 4, *((int32_t*) adt_ary_value(array, 4)));
   CuAssertIntEquals(tc, 3, *((int32_t*) adt_ary_value(array, 5)));
   CuAssertIntEquals(tc, 2, *((int32_t*) adt_ary_value(array, 6)));
   adt_ary_delete(array);

}

static void test_adt_ary_sort_strings_array_with_four_items(CuTest* tc)
{
   const char *green = "green";
   const char *purple = "purple";
   const char *blue = "blue";
   const char *black = "black";

   adt_ary_t *array = adt_ary_new(adt_str_vdelete);
   CuAssertPtrNotNull(tc, array);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, adt_str_new_cstr(green)));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, adt_str_new_cstr(purple)));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, adt_str_new_cstr(blue)));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, adt_str_new_cstr(black)));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_sort(array, adt_str_vlt, false));
   CuAssertStrEquals(tc, "black", adt_str_cstr((adt_str_t*) adt_ary_value(array, 0)));
   CuAssertStrEquals(tc, "blue", adt_str_cstr((adt_str_t*) adt_ary_value(array, 1)));
   CuAssertStrEquals(tc, "green", adt_str_cstr((adt_str_t*) adt_ary_value(array, 2)));
   CuAssertStrEquals(tc, "purple", adt_str_cstr((adt_str_t*) adt_ary_value(array, 3)));
   adt_ary_delete(array);
}

static void test_adt_ary_indexOf(CuTest* tc)
{
   adt_ary_t *array = adt_ary_new(NULL);
   CuAssertPtrNotNull(tc, array);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, &m_numbers[0])); //index 0
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, &m_numbers[1])); //index 1
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, &m_numbers[2])); //index 2
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, &m_numbers[3])); //index 3
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(array, &m_numbers[4])); //index 4
   CuAssertIntEquals(tc, 5, adt_ary_length(array));

   CuAssertIntEquals(tc, 0, adt_ary_indexOf(array, &m_numbers[0]));
   CuAssertIntEquals(tc, 1, adt_ary_indexOf(array, &m_numbers[1]));
   CuAssertIntEquals(tc, 2, adt_ary_indexOf(array, &m_numbers[2]));
   CuAssertIntEquals(tc, 3, adt_ary_indexOf(array, &m_numbers[3]));
   CuAssertIntEquals(tc, 4, adt_ary_indexOf(array, &m_numbers[4]));
   CuAssertIntEquals(tc, -1, adt_ary_indexOf(array, &m_numbers[5]));
   CuAssertIntEquals(tc, -1, adt_ary_indexOf(array, &m_numbers[6]));

   adt_ary_delete(array);
}
