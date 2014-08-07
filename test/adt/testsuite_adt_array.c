#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "adt_array.h"
#include "CMemLeak.h"

void void_free(void* p){
	free(p);
}

void test_adt_array_new(CuTest* tc)
{
	adt_array_t *pArray = adt_array_new(NULL);
	CuAssertPtrNotNull(tc, pArray);
	CuAssertPtrEquals(tc, 0,pArray->ppAlloc);
	CuAssertPtrEquals(tc, 0,pArray->pDestructor);
	CuAssertIntEquals(tc,0,pArray->u32AllocLen);
	CuAssertIntEquals(tc,0,pArray->u32CurLen);
	adt_array_delete(pArray);
}

void test_adt_array_push_pop(CuTest* tc)
{
	char *pVal;
	adt_array_t *pArray = adt_array_new(void_free);
	CuAssertPtrNotNull(tc, pArray);
	adt_array_push(pArray,strdup("The"));
	adt_array_push(pArray,strdup("quick"));
	adt_array_push(pArray,strdup("brown"));
	adt_array_push(pArray,strdup("fox"));
	CuAssertPtrNotNull(tc,pArray->ppAlloc);
	CuAssertPtrEquals(tc, void_free,pArray->pDestructor);
	CuAssertIntEquals(tc,4,pArray->u32AllocLen);
	CuAssertIntEquals(tc,4,pArray->u32CurLen);
	CuAssertPtrNotNull(tc,(pVal = (char*) adt_array_pop(pArray)));
	CuAssertStrEquals(tc, "fox",pVal);
	free(pVal);
	CuAssertIntEquals(tc,3,adt_array_length(pArray));
	CuAssertPtrNotNull(tc,(pVal = (char*) adt_array_pop(pArray)));
	CuAssertStrEquals(tc, "brown",pVal);
	free(pVal);
	CuAssertIntEquals(tc,2,adt_array_length(pArray));
	CuAssertPtrNotNull(tc,(pVal = (char*) adt_array_pop(pArray)));
	CuAssertStrEquals(tc, "quick",pVal);
	free(pVal);
	CuAssertIntEquals(tc,1,adt_array_length(pArray));
	CuAssertPtrNotNull(tc,(pVal = (char*) adt_array_pop(pArray)));
	CuAssertStrEquals(tc, "The",pVal);
	free(pVal);
	CuAssertIntEquals(tc,0,adt_array_length(pArray));
	adt_array_delete(pArray);
}

void test_adt_array_shift_unshift(CuTest* tc)
{
	void **ppVal;
	char *pVal;
	adt_array_t *pArray = adt_array_new(void_free);

	CuAssertPtrNotNull(tc, pArray);
	CuAssertIntEquals(tc,0,adt_array_length(pArray));


	adt_array_unshift(pArray,strdup("The"));
	CuAssertIntEquals(tc,1,adt_array_length(pArray));
	CuAssertPtrNotNull(tc, (ppVal = adt_array_get(pArray,0)));
	pVal = (char*) *ppVal;
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "The",pVal);

	adt_array_unshift(pArray,strdup("quick"));
	CuAssertIntEquals(tc,2,adt_array_length(pArray));
	CuAssertPtrNotNull(tc, (ppVal = adt_array_get(pArray,0)));
	pVal = (char*) *ppVal;
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "quick",pVal);

	adt_array_unshift(pArray,strdup("brown"));
	CuAssertIntEquals(tc,3,adt_array_length(pArray));
	CuAssertPtrNotNull(tc, (ppVal = adt_array_get(pArray,0)));
	pVal = (char*) *ppVal;
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "brown",pVal);

	adt_array_unshift(pArray,strdup("fox"));
	CuAssertIntEquals(tc,4,adt_array_length(pArray));
	CuAssertPtrNotNull(tc, (ppVal = adt_array_get(pArray,0)));
	pVal = (char*) *ppVal;
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "fox",pVal);

	adt_array_delete(pArray);
}


CuSuite* testsuite_adt_array(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_adt_array_new);
	SUITE_ADD_TEST(suite, test_adt_array_push_pop);
	SUITE_ADD_TEST(suite, test_adt_array_shift_unshift);

	return suite;
}
