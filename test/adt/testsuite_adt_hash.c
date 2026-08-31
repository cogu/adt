#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "test_common.h"
#include "adt_hash.h"
#include "adt_ary.h"

void test_adt_hash_constructor(CuTest* tc)
{
	adt_hash_t *hash = adt_hash_new(NULL);
	CuAssertPtrNotNull(tc, hash);
	adt_hash_delete(hash);
}

void test_adt_hash_iterator(CuTest* tc){
	adt_hash_t *pHash = adt_hash_new(NULL);
	CuAssertPtrNotNull(tc, pHash);
	int value = 42;

	adt_hash_set(pHash,"The",&value);
	adt_hash_set(pHash,"quick",&value);
	adt_hash_set(pHash,"brown",&value);
	adt_hash_set(pHash,"fox",&value);
	adt_hash_iter_init(pHash);
	void *pVal = 0;
	const char *pKey;
	uint32_t i = 0;
	do{
		pVal = adt_hash_iter_next(pHash,&pKey);
		switch(i++){
		case 0:
			CuAssertPtrNotNull(tc, pVal);
			CuAssertStrEquals(tc,"The",pKey);
			break;
		case 1:
			CuAssertPtrNotNull(tc, pVal);
			CuAssertStrEquals(tc,"quick",pKey);
			break;
		case 2:
			CuAssertPtrNotNull(tc, pVal);
			CuAssertStrEquals(tc,"brown",pKey);
			break;
		case 3:
			CuAssertPtrNotNull(tc, pVal);
			CuAssertStrEquals(tc,"fox",pKey);
			break;
		case 4:
			CuAssertPtrEquals(tc,0,pVal);
			break;
		}
	}while(pVal);
	CuAssertIntEquals(tc,4,adt_hash_length(pHash));
	adt_hash_delete(pHash);
}

void test_adt_hash_keys(CuTest* tc)
{
   int val1 = 1;
   int val2 = 2;
   int val3 = 3;
   int val4 = 4;
   adt_hash_t *pHash = adt_hash_new(NULL);
   adt_ary_t *pKeys = adt_ary_new(vfree);

   CuAssertPtrNotNull(tc, pHash);
   CuAssertPtrNotNull(tc, pKeys);

   adt_hash_set(pHash,"The",&val1);
   adt_hash_set(pHash,"quick",&val2);
   adt_hash_set(pHash,"brown",&val3);
   adt_hash_set(pHash,"fox",&val4);

   CuAssertIntEquals(tc, 0, adt_ary_length(pKeys));
   adt_hash_keys(pHash, pKeys);
   CuAssertIntEquals(tc, 4, adt_ary_length(pKeys));
   CuAssertStrEquals(tc, "The", (const char*) adt_ary_value(pKeys, 0));
   CuAssertStrEquals(tc, "quick", (const char*) adt_ary_value(pKeys, 1));
   CuAssertStrEquals(tc, "brown", (const char*) adt_ary_value(pKeys, 2));
   CuAssertStrEquals(tc, "fox", (const char*) adt_ary_value(pKeys, 3));

   adt_hash_delete(pHash);
   adt_ary_delete(pKeys);

}

void test_adt_hash_values(CuTest* tc)
{
   int val1 = 1;
   int val2 = 2;
   int val3 = 3;
   int val4 = 4;
   adt_hash_t *pHash = adt_hash_new(NULL);
   adt_ary_t *pValues = adt_ary_new(NULL);

   CuAssertPtrNotNull(tc, pHash);
   CuAssertPtrNotNull(tc, pValues);

   adt_hash_set(pHash,"The",&val1);
   adt_hash_set(pHash,"quick",&val2);
   adt_hash_set(pHash,"brown",&val3);
   adt_hash_set(pHash,"fox",&val4);

   CuAssertIntEquals(tc, 0, adt_ary_length(pValues));
   adt_hash_values(pHash, pValues);
   CuAssertIntEquals(tc, 4, adt_ary_length(pValues));

   CuAssertPtrEquals(tc, &val1, adt_ary_value(pValues, 0));
   CuAssertPtrEquals(tc, &val2, adt_ary_value(pValues, 1));
   CuAssertPtrEquals(tc, &val3, adt_ary_value(pValues, 2));
   CuAssertPtrEquals(tc, &val4, adt_ary_value(pValues, 3));

   adt_hash_delete(pHash);
   adt_ary_delete(pValues);

}

void test_adt_hash_remove(CuTest* tc)
{
   int *val1 = malloc(sizeof(int));
   int *val2 = malloc(sizeof(int));
   int *val3 = malloc(sizeof(int));
   adt_hash_t *pHash = adt_hash_new(vfree);

   CuAssertPtrNotNull(tc, pHash);

   adt_hash_set(pHash,"V1",val1);
   adt_hash_set(pHash,"V2",val2);
   adt_hash_set(pHash,"V3",val3);

   CuAssertIntEquals(tc, 3, adt_hash_length(pHash));
   CuAssertPtrEquals(tc, val2, adt_hash_remove(pHash, "V2"));
   CuAssertPtrEquals(tc, val3, adt_hash_remove(pHash, "V3"));
   CuAssertPtrEquals(tc, val1, adt_hash_remove(pHash, "V1"));
   CuAssertIntEquals(tc, 0, adt_hash_length(pHash));

   adt_hash_delete(pHash);
   free(val1);
   free(val2);
   free(val3);

}

void test_adt_hash_value(CuTest* tc)
{
   int *val1 = malloc(sizeof(int));
   int *val2 = malloc(sizeof(int));
   int *val3 = malloc(sizeof(int));

   adt_hash_t *pHash = adt_hash_new(vfree);

   CuAssertPtrNotNull(tc, pHash);

   adt_hash_set(pHash,"V1",val1);
   adt_hash_set(pHash,"V2",val2);
   adt_hash_set(pHash,"V3",val3);

   CuAssertIntEquals(tc, 3, adt_hash_length(pHash));
   CuAssertPtrEquals(tc, val1, adt_hash_value(pHash, "V1"));
   CuAssertPtrEquals(tc, val2, adt_hash_value(pHash, "V2"));
   CuAssertPtrEquals(tc, val3, adt_hash_value(pHash, "V3"));

   adt_hash_delete(pHash);
}

void test_adt_hash_exists(CuTest* tc)
{
   adt_hash_t *pHash = adt_hash_new(NULL);
   int val = 123;
   CuAssertTrue(tc, !adt_hash_exists(pHash, "nonexistent"));

   adt_hash_set(pHash, "key1", &val);
   CuAssertTrue(tc, adt_hash_exists(pHash, "key1"));
   CuAssertTrue(tc, !adt_hash_exists(pHash, "key2"));

   adt_hash_delete(pHash);
}

void test_adt_hash_set_overwrite_destructor(CuTest* tc)
{
   adt_hash_t *pHash = adt_hash_new(vfree);
   CuAssertPtrNotNull(tc, pHash);

   adt_hash_set(pHash, "my_key", STRDUP("first_value"));
   CuAssertIntEquals(tc, 1, adt_hash_length(pHash));
   CuAssertStrEquals(tc, "first_value", (char*) adt_hash_value(pHash, "my_key"));

   // Overwriting should trigger vfree on "first_value"
   adt_hash_set(pHash, "my_key", STRDUP("second_value"));
   CuAssertIntEquals(tc, 1, adt_hash_length(pHash));
   CuAssertStrEquals(tc, "second_value", (char*) adt_hash_value(pHash, "my_key"));

   adt_hash_delete(pHash);
}

CuSuite* testsuite_adt_hash(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_adt_hash_constructor);
	SUITE_ADD_TEST(suite, test_adt_hash_iterator);
	SUITE_ADD_TEST(suite, test_adt_hash_keys);
	SUITE_ADD_TEST(suite, test_adt_hash_values);
	SUITE_ADD_TEST(suite, test_adt_hash_remove);
	SUITE_ADD_TEST(suite, test_adt_hash_value);
	SUITE_ADD_TEST(suite, test_adt_hash_exists);
	SUITE_ADD_TEST(suite, test_adt_hash_set_overwrite_destructor);
	return suite;
}
