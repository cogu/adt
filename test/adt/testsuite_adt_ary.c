#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "adt_ary.h"
#include "CMemLeak.h"

void void_free(void* p){
	free(p);
}

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
	adt_ary_t *pArray = adt_ary_new(void_free);
	CuAssertPtrNotNull(tc, pArray);
	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray,strdup("The")));
	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray,strdup("quick")));
	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray,strdup("brown")));
	CuAssertIntEquals(tc, ADT_NO_ERROR, adt_ary_push(pArray,strdup("fox")));
	CuAssertPtrNotNull(tc,pArray->ppAlloc);
	CuAssertPtrEquals(tc, void_free,pArray->pDestructor);
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

static void test_adt_ary_shift_unshift(CuTest* tc)
{
	void **ppVal;
	char *pVal;
	char *tmp;
	adt_ary_t *pArray = adt_ary_new(void_free);

	CuAssertPtrNotNull(tc, pArray);
	CuAssertIntEquals(tc,0,adt_ary_length(pArray));
	CuAssertPtrEquals(tc, void_free,pArray->pDestructor);

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
   adt_ary_t *pArray = adt_ary_new(void_free);
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


CuSuite* testsuite_adt_ary(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_adt_ary_new);
	SUITE_ADD_TEST(suite, test_adt_ary_push_pop);
	SUITE_ADD_TEST(suite, test_adt_ary_shift_unshift);
	SUITE_ADD_TEST(suite, test_adt_ary_resize);
	SUITE_ADD_TEST(suite, test_adt_ary_push_unique);

	return suite;
}
