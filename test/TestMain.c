#include <stdio.h>
#include "CuTest.h"
#include "CMemLeak.h"

#ifndef TEST_ADT_HASH_ENABLE
#define TEST_ADT_HASH_ENABLE 0
#endif

CuSuite* testsuite_adt_str(void);
CuSuite* testsuite_adt_ary(void);
CuSuite* testsuite_adt_hash(void);
CuSuite* testsuite_adt_stack(void);
CuSuite* testsuite_adt_u16Map(void);
CuSuite* testsuite_adt_list(void);
CuSuite* testsuite_adt_u32List(void);
CuSuite* testsuite_adt_bytearray(void);
CuSuite* testsuite_adt_priorityHeap(void);
CuSuite* testsuite_adt_u32Set(void);


void RunAllTests(void)
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, testsuite_adt_str());
	CuSuiteAddSuite(suite, testsuite_adt_ary());
#if TEST_ADT_HASH_ENABLE
	CuSuiteAddSuite(suite, testsuite_adt_hash()); //Note that this test takes several seconds to run, normally disabled
#endif
	CuSuiteAddSuite(suite, testsuite_adt_stack());
	CuSuiteAddSuite(suite, testsuite_adt_u16Map());
	CuSuiteAddSuite(suite, testsuite_adt_list());
	CuSuiteAddSuite(suite, testsuite_adt_u32List());
	CuSuiteAddSuite(suite, testsuite_adt_bytearray());
	CuSuiteAddSuite(suite, testsuite_adt_priorityHeap());
	CuSuiteAddSuite(suite, testsuite_adt_u32Set());



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
