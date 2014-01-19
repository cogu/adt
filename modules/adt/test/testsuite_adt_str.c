#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "adt_str.h"
#include "CMemLeak.h"

void test_adt_str_constructor(CuTest* tc)
{
	adt_str_t *str = adt_str_new();
	CuAssertPtrNotNull(tc, str);
	CuAssertStrEquals(tc,"",adt_str_cstr(str));
	adt_str_delete(str);
}

void test_adt_str_push(CuTest* tc){
	adt_str_t *str = adt_str_new();
	CuAssertPtrNotNull(tc, str);
	int i;
	int len = 0;
	for(i = 'A'; i <= 'Z'; i++){
		len++;
		adt_str_push(str,i);
		CuAssertIntEquals(tc,len,strlen(adt_str_cstr(str)));
		CuAssertIntEquals(tc,len,adt_str_length(str));
	}

	CuAssertStrEquals(tc,"ABCDEFGHIJKLMNOPQRSTUVWXYZ",adt_str_cstr(str));

	adt_str_delete(str);
}

void test_adt_str_pop(CuTest* tc){
	adt_str_t *str = adt_str_new();
	CuAssertPtrNotNull(tc, str);
	int i;
	int len = 26;
	adt_str_set_cstr(str,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	for(i = 'Z'; i >= 'A'; i--){
		char c;
		len--;
		c = (char) adt_str_pop(str);
		CuAssertTrue(tc,len>=0);
		CuAssertIntEquals(tc,len,strlen(adt_str_cstr(str)));
		CuAssertIntEquals(tc,len,adt_str_length(str));
		CuAssertIntEquals(tc,i,c);
	}
	CuAssertTrue(tc,len==0);
	CuAssertStrEquals(tc,"",adt_str_cstr(str));
	adt_str_delete(str);
}

void test_adt_str_cdup(CuTest* tc){
	adt_str_t *str = adt_str_dup_cstr("Hello World");
	CuAssertPtrNotNull(tc, str);
	CuAssertIntEquals(tc,11,adt_str_length(str));
	CuAssertStrEquals(tc,"Hello World",adt_str_cstr(str));
	adt_str_delete(str);
}

void test_adt_str_cappend(CuTest* tc){
	adt_str_t *str = adt_str_dup_cstr("Hello");
	CuAssertPtrNotNull(tc, str);
	CuAssertIntEquals(tc,5,adt_str_length(str));
	CuAssertStrEquals(tc,"Hello",adt_str_cstr(str));
	adt_str_append_cstr(str," World");
	CuAssertIntEquals(tc,11,adt_str_length(str));
	CuAssertStrEquals(tc,"Hello World",adt_str_cstr(str));
	adt_str_delete(str);
}

void test_adt_str_copy_range(CuTest* tc){
	char buf[80];
	strcpy(buf,"Hello World!");
	const char *a=buf,*b=buf+5;
	const char *c=buf+6,*d=buf+12;
	adt_str_t *str1 = adt_str_new();
	adt_str_t *str2 = adt_str_new();
	CuAssertPtrNotNull(tc,str1);
	CuAssertPtrNotNull(tc,str2);
	adt_str_copy_range(str1,a,b);
	adt_str_copy_range(str2,c,d);
	CuAssertStrEquals(tc,"Hello",adt_str_cstr(str1));
	CuAssertStrEquals(tc,"World!",adt_str_cstr(str2));
	adt_str_delete(str1);
	adt_str_delete(str2);
}

void test_adt_str_get_char(CuTest* tc){
	adt_str_t *str = adt_str_dup_cstr("Hello");
	CuAssertPtrNotNull(tc, str);
	CuAssertIntEquals(tc,'H',adt_str_get_char(str,0));
	CuAssertIntEquals(tc,'e',adt_str_get_char(str,1));
	CuAssertIntEquals(tc,'l',adt_str_get_char(str,2));
	CuAssertIntEquals(tc,'l',adt_str_get_char(str,3));
	CuAssertIntEquals(tc,'o',adt_str_get_char(str,4));
	CuAssertIntEquals(tc,-1,adt_str_get_char(str,5));
	CuAssertIntEquals(tc,'o',adt_str_get_char(str,-1));
	CuAssertIntEquals(tc,'l',adt_str_get_char(str,-2));
	CuAssertIntEquals(tc,'l',adt_str_get_char(str,-3));
	CuAssertIntEquals(tc,'e',adt_str_get_char(str,-4));
	CuAssertIntEquals(tc,'H',adt_str_get_char(str,-5));
	CuAssertIntEquals(tc,-1,adt_str_get_char(str,-6));
	adt_str_delete(str);
}


CuSuite* testsuite_adt_str(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_adt_str_constructor);
	SUITE_ADD_TEST(suite, test_adt_str_push);
	SUITE_ADD_TEST(suite, test_adt_str_pop);
	SUITE_ADD_TEST(suite, test_adt_str_cdup);
	SUITE_ADD_TEST(suite, test_adt_str_cappend);
	SUITE_ADD_TEST(suite, test_adt_str_copy_range);
	SUITE_ADD_TEST(suite, test_adt_str_get_char);


	return suite;
}
