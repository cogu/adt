#include <stdio.h>
#include "CuTest.h"
#include "CMemLeak.h"

CuSuite* testsuite_adt_str(void);
CuSuite* testsuite_adt_ary(void);
CuSuite* testsuite_adt_hash(void);
CuSuite* testsuite_adt_stack(void);
CuSuite* testsuite_adt_u16Map(void);

void RunAllTests(void)
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, testsuite_adt_str());
	CuSuiteAddSuite(suite, testsuite_adt_ary());
	CuSuiteAddSuite(suite, testsuite_adt_hash());
	CuSuiteAddSuite(suite, testsuite_adt_stack());
	CuSuiteAddSuite(suite, testsuite_adt_u16Map());


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
