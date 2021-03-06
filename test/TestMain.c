#include <stdio.h>
#include "CuTest.h"
#ifdef MEM_LEAK_CHECK
# include "CMemLeak.h"
#endif

#ifndef TEST_ADT_HASH_ENABLE
#define TEST_ADT_HASH_ENABLE 0
#endif

//Enabling of deprecated functions
#ifndef TEST_ADT_U16MAP_ENABLE
#define TEST_ADT_U16MAP_ENABLE 0
#endif


CuSuite* testsuite_adt_str(void);
CuSuite* testsuite_adt_ary(void);
CuSuite* testsuite_adt_hash(void);
CuSuite* testsuite_adt_stack(void);
#if TEST_ADT_U16MAP_ENABLE
CuSuite* testsuite_adt_u16Map(void);
#endif
CuSuite* testsuite_adt_list(void);
CuSuite* testsuite_adt_u32List(void);
CuSuite* testsuite_adt_bytearray(void);
CuSuite* testsuite_adt_priorityHeap(void);
CuSuite* testsuite_adt_u32Set(void);
CuSuite* testsuite_adt_ringbuf(void);
CuSuite* testsuite_adt_bytes(void);

#ifdef MEM_LEAK_CHECK
void vfree(void* p)
{
   free(p);
}
#endif

void RunAllTests(void)
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, testsuite_adt_str());
	CuSuiteAddSuite(suite, testsuite_adt_ary());
	CuSuiteAddSuite(suite, testsuite_adt_hash());
	CuSuiteAddSuite(suite, testsuite_adt_stack());
#if TEST_ADT_U16MAP_ENABLE
	CuSuiteAddSuite(suite, testsuite_adt_u16Map());
#endif
	CuSuiteAddSuite(suite, testsuite_adt_list());
	CuSuiteAddSuite(suite, testsuite_adt_u32List());
	CuSuiteAddSuite(suite, testsuite_adt_bytearray());
	CuSuiteAddSuite(suite, testsuite_adt_priorityHeap());
	CuSuiteAddSuite(suite, testsuite_adt_u32Set());
	CuSuiteAddSuite(suite, testsuite_adt_ringbuf());
	CuSuiteAddSuite(suite, testsuite_adt_bytes());




	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
	CuSuiteDelete(suite);
	CuStringDelete(output);

}

int main(void)
{
	RunAllTests();
	return 0;
}
