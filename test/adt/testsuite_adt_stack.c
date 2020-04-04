#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "adt_stack.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
void vfree(void* p);
#else
#define vfree free
#endif

#ifdef _MSC_VER
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

void test_adt_stack_new(CuTest* tc)
{
	adt_stack_t *pStack = adt_stack_new(NULL);
	CuAssertPtrNotNull(tc, pStack);
	CuAssertPtrEquals(tc, 0,pStack->ppAlloc);
	CuAssertPtrEquals(tc, 0,pStack->pDestructor);
	CuAssertIntEquals(tc,0,pStack->u32AllocLen);
	CuAssertIntEquals(tc,0,pStack->u32CurLen);
	adt_stack_delete(pStack);
}

void test_adt_stack_push(CuTest* tc)
{
	adt_stack_t *pStack = adt_stack_new(vfree);
	CuAssertPtrNotNull(tc, pStack);
	adt_stack_push(pStack,STRDUP("The"));
	adt_stack_push(pStack,STRDUP("quick"));
	adt_stack_push(pStack,STRDUP("brown"));
	adt_stack_push(pStack,STRDUP("fox"));
	CuAssertPtrNotNull(tc,pStack->ppAlloc);
	CuAssertPtrEquals(tc, vfree,pStack->pDestructor);
	CuAssertIntEquals(tc,8,pStack->u32AllocLen);
	CuAssertIntEquals(tc,4,pStack->u32CurLen);
	adt_stack_delete(pStack);
}

void test_adt_stack_top(CuTest* tc)
{
	char *pVal;
	adt_stack_t *pStack = adt_stack_new(NULL);

	CuAssertPtrNotNull(tc, pStack);
	CuAssertIntEquals(tc,0,adt_stack_size(pStack));
	CuAssertPtrEquals(tc, 0,adt_stack_top(pStack));


	adt_stack_push(pStack,STRDUP("The"));
	CuAssertIntEquals(tc,1,adt_stack_size(pStack));
	pVal = (char*) adt_stack_top(pStack);
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "The",pVal);
	free(pVal);

	adt_stack_push(pStack,STRDUP("quick"));
	CuAssertIntEquals(tc,2,adt_stack_size(pStack));
	pVal = (char*) adt_stack_top(pStack);
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "quick",pVal);
	free(pVal);

	adt_stack_push(pStack,STRDUP("brown"));
	CuAssertIntEquals(tc,3,adt_stack_size(pStack));
	pVal = (char*) adt_stack_top(pStack);
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "brown",pVal);
	free(pVal);

	adt_stack_push(pStack,STRDUP("fox"));
	CuAssertIntEquals(tc,4,adt_stack_size(pStack));
	pVal = (char*) adt_stack_top(pStack);
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "fox",pVal);
	free(pVal);

	adt_stack_delete(pStack);
}

void test_adt_stack_pop(CuTest* tc)
{
	char *pVal;
	adt_stack_t *pStack = adt_stack_new(NULL);

	CuAssertPtrNotNull(tc, pStack);
	CuAssertIntEquals(tc,0,adt_stack_size(pStack));
	CuAssertPtrEquals(tc, 0,adt_stack_top(pStack));

	adt_stack_push(pStack,STRDUP("The"));
	CuAssertIntEquals(tc,1,adt_stack_size(pStack));
	adt_stack_push(pStack,STRDUP("quick"));
	CuAssertIntEquals(tc,2,adt_stack_size(pStack));

	adt_stack_push(pStack,STRDUP("brown"));
	CuAssertIntEquals(tc,3,adt_stack_size(pStack));

	adt_stack_push(pStack,STRDUP("fox"));
	CuAssertIntEquals(tc,4,adt_stack_size(pStack));

	pVal = (char*) adt_stack_pop(pStack);
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "fox",pVal);
	CuAssertIntEquals(tc,3,adt_stack_size(pStack));
	free(pVal);

	pVal = (char*) adt_stack_pop(pStack);
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "brown",pVal);
	CuAssertIntEquals(tc,2,adt_stack_size(pStack));
	free(pVal);

	pVal = (char*) adt_stack_pop(pStack);
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "quick",pVal);
	CuAssertIntEquals(tc,1,adt_stack_size(pStack));
	free(pVal);

	pVal = (char*) adt_stack_pop(pStack);
	CuAssertPtrNotNull(tc, pVal);
	CuAssertStrEquals(tc, "The",pVal);
	CuAssertIntEquals(tc,0,adt_stack_size(pStack));
	free(pVal);

	CuAssertPtrEquals(tc, 0,adt_stack_pop(pStack));
	CuAssertPtrEquals(tc, 0,adt_stack_top(pStack));

	adt_stack_delete(pStack);
}



CuSuite* testsuite_adt_stack(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_adt_stack_new);
	SUITE_ADD_TEST(suite, test_adt_stack_push);
	SUITE_ADD_TEST(suite, test_adt_stack_top);
	SUITE_ADD_TEST(suite, test_adt_stack_pop);

	return suite;
}
