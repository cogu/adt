#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "test_common.h"
#include "adt_stack.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

void test_adt_stack_new(CuTest* tc)
{
	adt_stack_t *pStack = adt_stack_new(NULL);
	CuAssertPtrNotNull(tc, pStack);
	CuAssertPtrEquals(tc, NULL, pStack->ppAlloc);
	CuAssertPtrEquals(tc, NULL, pStack->pDestructor);
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
	CuAssertPtrEquals(tc, NULL, adt_stack_top(pStack));


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
	CuAssertPtrEquals(tc, NULL, adt_stack_top(pStack));

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

	CuAssertPtrEquals(tc, NULL, adt_stack_pop(pStack));
	CuAssertPtrEquals(tc, NULL, adt_stack_top(pStack));

	adt_stack_delete(pStack);
}

void test_adt_stack_reserve(CuTest* tc)
{
	adt_stack_t *pStack = adt_stack_new(NULL);
	CuAssertPtrNotNull(tc, pStack);
	adt_stack_reserve(pStack, 32);
	CuAssertIntEquals(tc, 32, pStack->u32AllocLen);
	CuAssertIntEquals(tc, 0, adt_stack_size(pStack));
	adt_stack_delete(pStack);
}

void test_adt_stack_resize(CuTest* tc)
{
	adt_stack_t *pStack = adt_stack_new(vfree);
	CuAssertPtrNotNull(tc, pStack);

	adt_stack_push(pStack, STRDUP("first"));
	adt_stack_push(pStack, STRDUP("second"));
	adt_stack_push(pStack, STRDUP("third"));
	CuAssertIntEquals(tc, 3, adt_stack_size(pStack));

	// Shrink from 3 to 1 (items "second" and "third" will be cleaned up via vfree)
	adt_stack_resize(pStack, 1);
	CuAssertIntEquals(tc, 1, adt_stack_size(pStack));
	CuAssertIntEquals(tc, 1, pStack->u32AllocLen);
	CuAssertStrEquals(tc, "first", (char*) adt_stack_top(pStack));

	adt_stack_delete(pStack);
}

CuSuite* testsuite_adt_stack(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_adt_stack_new);
	SUITE_ADD_TEST(suite, test_adt_stack_push);
	SUITE_ADD_TEST(suite, test_adt_stack_top);
	SUITE_ADD_TEST(suite, test_adt_stack_pop);
	SUITE_ADD_TEST(suite, test_adt_stack_reserve);
	SUITE_ADD_TEST(suite, test_adt_stack_resize);

	return suite;
}
