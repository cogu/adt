#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "CuTest.h"
#include "adt_hash.h"
#include "CMemLeak.h"


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
	uint32_t u32KeyLen;
	uint32_t i = 0;
	do{
		pVal = adt_hash_iter_next(pHash,&pKey,&u32KeyLen);
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
	uint32_t u32KeyLen;
	uint32_t n = 0;
	do{
		pVal = adt_hash_iter_next(pHash,&pKey,&u32KeyLen);
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



CuSuite* testsuite_adt_hash(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_adt_hash_constructor);
	SUITE_ADD_TEST(suite, test_adt_hash_iterator);
	SUITE_ADD_TEST(suite, test_adt_hash_iterator2);
	return suite;
}
