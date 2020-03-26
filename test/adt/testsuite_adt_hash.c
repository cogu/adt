#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "CuTest.h"
#include "adt_hash.h"
#include "adt_ary.h"
#include "CMemLeak.h"

static void vfree(void *arg)
{
   free(arg);
}


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

void test_adt_hash_iterator2(CuTest* tc){
	adt_hash_t *pHash = adt_hash_new(NULL);
	CuAssertPtrNotNull(tc, pHash);
	clock_t start, end;
	double elapsed_time = 0.0;
	size_t len = 256;
	char *line = (char*) malloc(len);
	ssize_t read;
	int value = 42;
	int items = 0;
	int dup = 0;

	start = clock();
	FILE *fh = fopen("../../../test/3esl.txt","r");
	if (fh == 0)
	{
	   fh = fopen("../../../../test/3esl.txt","r");
	}
   if (fh == 0)
   {
      fh = fopen("../test/3esl.txt","r");
   }
	assert(fh != 0);
	do{
	   read = getline(&line,&len,fh);
	   if(read>1){
	      line[read-1]=0;
	      items++;
	      if(adt_hash_get(pHash,line)!=0){
	         printf("duplicate item: %s\n",line);
	         dup++;
	      }
	      adt_hash_set(pHash,line,&value);
	   }
	}while(read>=0);
	end = clock();
	elapsed_time = (double)(end - start) / (double)CLOCKS_PER_SEC;
	printf("added %d items in %.002fs\n",items,elapsed_time);
	fclose(fh);
	CuAssertIntEquals(tc,items,adt_hash_length(pHash));
	adt_hash_iter_init(pHash);
	void *pVal = 0;
	const char *pKey;

	uint32_t n = 0;
	do{
		pVal = adt_hash_iter_next(pHash,&pKey);
		if((int) n<items){
			CuAssertPtrNotNull(tc, pVal);
			n++;
		}
		else{
			CuAssertPtrEquals(tc,0,pVal);
		}
	}while(pVal);
	CuAssertIntEquals(tc,items,n);
	CuAssertIntEquals(tc,items,adt_hash_length(pHash));
	adt_hash_delete(pHash);
	free(line);
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

CuSuite* testsuite_adt_hash(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_adt_hash_constructor);
	SUITE_ADD_TEST(suite, test_adt_hash_iterator);
#if TEST_ADT_HASH_FULL //Note that this test takes several seconds to run, normally disabled
	SUITE_ADD_TEST(suite, test_adt_hash_iterator2);
#endif
	SUITE_ADD_TEST(suite, test_adt_hash_keys);
	SUITE_ADD_TEST(suite, test_adt_hash_values);
	SUITE_ADD_TEST(suite, test_adt_hash_remove);
	SUITE_ADD_TEST(suite, test_adt_hash_value);
	return suite;
}
