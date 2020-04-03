/*****************************************************************************
* \file      testsuite_adt_str.c
* \author    Conny Gustafsson
* \date      2014-08-07
* \brief     Unit tests for adt_str
*
* Copyright (c) 2014-2019 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "adt_str.h"
#include "adt_bytes.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_adt_str_create(CuTest *tc);
static void test_adt_str_reserve(CuTest *tc);
static void test_adt_str_set_cstr(CuTest *tc);
static void test_adt_str_new_cstr(CuTest *tc);
static void test_adt_str_set_bstr(CuTest *tc);
static void test_adt_str_new_bstr(CuTest *tc);
static void test_adt_str_set(CuTest *tc);
static void test_adt_str_set_bstr_utf8(CuTest *tc);
static void test_adt_str_set_cstr_utf8(CuTest *tc);
static void test_adt_str_clone(CuTest *tc);
static void test_adt_str_append_cstr(CuTest *tc);
static void test_adt_str_append_cstr_utf8(CuTest *tc);
static void test_adt_str_append_bstr(CuTest *tc);
static void test_adt_str_append_bstr_utf8(CuTest *tc);
static void test_adt_str_append(CuTest *tc);
static void test_adt_str_push(CuTest *tc);
static void test_adt_str_pop(CuTest *tc);
static void test_adt_str_charAt(CuTest* tc);
static void test_adt_utf8_readCodePoint1(CuTest* tc);
static void test_adt_utf8_readCodePoint2(CuTest* tc);
static void test_adt_utf8_readCodePoint3(CuTest* tc);
static void test_adt_utf8_readCodePoint4(CuTest* tc);
static void test_adt_utf8_checkEncoding_ascii(CuTest* tc);
static void test_adt_utf8_checkEncoding_utf8(CuTest* tc);
static void test_adt_utf8_checkEncodingAndSize(CuTest* tc);
static void test_adt_str_equal_cstr(CuTest* tc);
static void test_adt_str_lt_utf8(CuTest* tc);
static void test_adt_str_new_byterray(CuTest* tc);
static void test_adt_str_bytearray(CuTest* tc);
static void test_adt_str_bytes(CuTest* tc);
static void test_adt_str_clear(CuTest* tc);
static void test_adt_adding_nulls_at_end_shall_not_be_part_of_length(CuTest* tc);
static void test_adt_str_append_bstr_containing_nulls_at_end(CuTest* tc);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_adt_str(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_str_create);
   SUITE_ADD_TEST(suite, test_adt_str_reserve);
   SUITE_ADD_TEST(suite, test_adt_str_set_cstr);
   SUITE_ADD_TEST(suite, test_adt_str_new_cstr);
   SUITE_ADD_TEST(suite, test_adt_str_set_bstr);
   SUITE_ADD_TEST(suite, test_adt_str_new_bstr);
   SUITE_ADD_TEST(suite, test_adt_str_set);
   SUITE_ADD_TEST(suite, test_adt_str_set_bstr_utf8);
   SUITE_ADD_TEST(suite, test_adt_str_set_cstr_utf8);
   SUITE_ADD_TEST(suite, test_adt_str_clone);
   SUITE_ADD_TEST(suite, test_adt_str_append_cstr);
   SUITE_ADD_TEST(suite, test_adt_str_append_cstr_utf8);
   SUITE_ADD_TEST(suite, test_adt_str_append_bstr);
   SUITE_ADD_TEST(suite, test_adt_str_append_bstr_utf8);
   SUITE_ADD_TEST(suite, test_adt_str_append);
   SUITE_ADD_TEST(suite, test_adt_str_push);
   SUITE_ADD_TEST(suite, test_adt_str_pop);
   SUITE_ADD_TEST(suite, test_adt_str_charAt);
   SUITE_ADD_TEST(suite, test_adt_utf8_readCodePoint1);
   SUITE_ADD_TEST(suite, test_adt_utf8_readCodePoint2);
   SUITE_ADD_TEST(suite, test_adt_utf8_readCodePoint3);
   SUITE_ADD_TEST(suite, test_adt_utf8_readCodePoint4);
   SUITE_ADD_TEST(suite, test_adt_utf8_checkEncoding_ascii);
   SUITE_ADD_TEST(suite, test_adt_utf8_checkEncoding_utf8);
   SUITE_ADD_TEST(suite, test_adt_utf8_checkEncodingAndSize);
   SUITE_ADD_TEST(suite, test_adt_str_equal_cstr);
   SUITE_ADD_TEST(suite, test_adt_str_lt_utf8);
   SUITE_ADD_TEST(suite, test_adt_str_new_byterray);
   SUITE_ADD_TEST(suite, test_adt_str_bytearray);
   SUITE_ADD_TEST(suite, test_adt_str_bytes);
   SUITE_ADD_TEST(suite, test_adt_str_clear);
   SUITE_ADD_TEST(suite, test_adt_adding_nulls_at_end_shall_not_be_part_of_length);
   SUITE_ADD_TEST(suite, test_adt_str_append_bstr_containing_nulls_at_end);


   return suite;
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void test_adt_str_create(CuTest *tc)
{
   adt_str_t str;
   adt_str_create(&str);
   CuAssertPtrEquals(tc, 0, str.pAlloc);
   CuAssertIntEquals(tc, 0, str.s32Cur);
   CuAssertIntEquals(tc, 0, str.s32Size);
}

static void test_adt_str_reserve(CuTest *tc)
{
   adt_str_t *str = adt_str_new();
   CuAssertPtrNotNull(tc, str);
   CuAssertIntEquals(tc, 0, str->s32Size);

   //negative length shall yield error
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_str_reserve(str, -1));
   CuAssertIntEquals(tc, 0, str->s32Size);

   //length 0 shall reserve 16 bytes
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_reserve(str, 0));
   CuAssertIntEquals(tc, 16, str->s32Size);

   //length 1 shall reserve 16 bytes
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_reserve(str, 1));
   CuAssertIntEquals(tc, 16, str->s32Size);

   //length 16 shall reserve 64 bytes
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_reserve(str, 16));
   CuAssertIntEquals(tc, 64, str->s32Size);

   //length 64 shall reserve 256 bytes
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_reserve(str, 64));
   CuAssertIntEquals(tc, 256, str->s32Size);

   //length 256 shall reserve 1024 bytes
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_reserve(str, 256));
   CuAssertIntEquals(tc, 1024, str->s32Size);

   adt_str_delete(str);
}

static void test_adt_str_set_cstr(CuTest *tc)
{
   const char *cstr1 = "Hello";
   const char *cstr2 = "World!";
   adt_str_t *str = adt_str_new();
   CuAssertPtrNotNull(tc, str);

   adt_str_set_cstr(str, cstr1);
   CuAssertIntEquals(tc, 5, adt_str_size(str));
   CuAssertIntEquals(tc, 0, memcmp(str->pAlloc, cstr1, str->s32Cur));

   adt_str_set_cstr(str, cstr2);
   CuAssertIntEquals(tc, 6, adt_str_size(str));
   CuAssertIntEquals(tc, 0, memcmp(str->pAlloc, cstr2, str->s32Cur));

   adt_str_delete(str);
}

static void test_adt_str_new_cstr(CuTest *tc)
{
   const char *cstr1 = "Hello";

   adt_str_t *str1 = adt_str_new_cstr(cstr1);

   CuAssertPtrNotNull(tc, str1);


   CuAssertIntEquals(tc, 5, adt_str_size(str1));
   CuAssertIntEquals(tc, 0, memcmp(str1->pAlloc, cstr1, str1->s32Cur));

   adt_str_delete(str1);
}

static void test_adt_str_set_bstr(CuTest *tc)
{
   char buf[20];
   const char *a, *b, *c, *d;
   adt_str_t *str1 = adt_str_new();
   adt_str_t *str2 = adt_str_new();
   CuAssertPtrNotNull(tc, str1);
   CuAssertPtrNotNull(tc, str2);

   strcpy(buf, "Hello World!");
   a=buf, b=buf+5;
   c=buf+6, d=buf+12;

   adt_str_set_bstr(str1, (const uint8_t*) a, (const uint8_t*) b);
   adt_str_set_bstr(str2, (const uint8_t*) c, (const uint8_t*) d);
   CuAssertIntEquals(tc, 5, str1->s32Cur);
   CuAssertIntEquals(tc, 6, str2->s32Cur);
   CuAssertIntEquals(tc, 0, memcmp(str1->pAlloc, a, 5));
   CuAssertIntEquals(tc, 0, memcmp(str2->pAlloc, c, 6));
   adt_str_delete(str1);
   adt_str_delete(str2);
}

static void test_adt_str_new_bstr(CuTest *tc)
{
   char buf[20];
   const char *a, *b, *c, *d;
   adt_str_t *str1, *str2;

   strcpy(buf, "Hello World!");
   a=buf, b=buf+5;
   c=buf+6, d=buf+12;

   str1 = adt_str_new_bstr((const uint8_t*) a, (const uint8_t*) b);
   str2 = adt_str_new_bstr((const uint8_t*) c, (const uint8_t*) d);
   CuAssertPtrNotNull(tc, str1);
   CuAssertPtrNotNull(tc, str2);
   CuAssertIntEquals(tc, 5, str1->s32Cur);
   CuAssertIntEquals(tc, 6, str2->s32Cur);
   CuAssertIntEquals(tc, 0, memcmp(str1->pAlloc, a, 5));
   CuAssertIntEquals(tc, 0, memcmp(str2->pAlloc, c, 6));
   adt_str_delete(str1);
   adt_str_delete(str2);
}

static void test_adt_str_set(CuTest *tc)
{
   char buf[20];
   const char *a, *b;
   adt_str_t *str1 = adt_str_new();
   adt_str_t *str2 = adt_str_new();
   CuAssertPtrNotNull(tc, str1);
   CuAssertPtrNotNull(tc, str2);

   strcpy(buf, "Hello World!");
   a=buf, b=buf+strlen(buf);

   adt_str_set_bstr(str1, (const uint8_t*) a, (const uint8_t*) b);
   adt_str_set(str2, str1);
   CuAssertIntEquals(tc, 12, str1->s32Cur);
   CuAssertIntEquals(tc, 12, str2->s32Cur);
   CuAssertIntEquals(tc, 0, memcmp(str1->pAlloc, &buf[0], str1->s32Cur));
   CuAssertIntEquals(tc, 0, memcmp(str2->pAlloc, &buf[0], str2->s32Cur));
   CuAssertTrue(tc, str1->pAlloc != str2->pAlloc);
   adt_str_delete(str1);
   adt_str_delete(str2);
}

static void test_adt_str_set_bstr_utf8(CuTest *tc)
{
   const char *a, *b;
   const char *test1 = "Test \343\202\206";
   const char *test2 = "Test2";
   adt_str_t *str = adt_str_new();

   CuAssertIntEquals(tc, 0, str->s32Cur);
   CuAssertUIntEquals(tc, ADT_STR_ENCODING_ASCII, adt_str_getEncoding(str));

   a=test1, b=a+strlen(test1);
   adt_str_set_bstr(str, (const uint8_t*) a, (const uint8_t*) b);

   CuAssertIntEquals(tc, 8, str->s32Cur);
   CuAssertUIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_str_getEncoding(str));

   a=test2, b=a+strlen(test2);
   adt_str_set_bstr(str, (const uint8_t*) a, (const uint8_t*) b);

   CuAssertIntEquals(tc, 5, str->s32Cur);
   //overwriting with ASCII string switches back to ASCII encoding
   CuAssertUIntEquals(tc, ADT_STR_ENCODING_ASCII, adt_str_getEncoding(str));


   adt_str_delete(str);
}

static void test_adt_str_set_cstr_utf8(CuTest *tc)
{
   const char *test1 = "Test \343\202\206";
   const char *test2 = "Test2";
   adt_str_t *str = adt_str_new();

   CuAssertIntEquals(tc, 0, str->s32Cur);
   CuAssertUIntEquals(tc, ADT_STR_ENCODING_ASCII, adt_str_getEncoding(str));

   adt_str_set_cstr(str, test1);

   CuAssertIntEquals(tc, 8, str->s32Cur);
   CuAssertUIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_str_getEncoding(str));


   adt_str_set_cstr(str, test2);

   CuAssertIntEquals(tc, 5, str->s32Cur);
   //overwriting with ASCII string switches back to ASCII encoding
   CuAssertUIntEquals(tc, ADT_STR_ENCODING_ASCII, adt_str_getEncoding(str));


   adt_str_delete(str);
}


static void test_adt_str_clone(CuTest *tc)
{
   char buf[20];
   const char *a, *b;
   adt_str_t *str1 = adt_str_new();
   adt_str_t *str2;
   CuAssertPtrNotNull(tc, str1);

   strcpy(buf, "Hello World!");
   a=buf, b=buf+strlen(buf);

   adt_str_set_bstr(str1, (const uint8_t*) a, (const uint8_t*) b);
   str2 = adt_str_clone(str1);
   CuAssertPtrNotNull(tc, str2);
   CuAssertIntEquals(tc, 12, str1->s32Cur);
   CuAssertIntEquals(tc, 12, str2->s32Cur);
   CuAssertIntEquals(tc, 0, memcmp(str1->pAlloc, &buf[0], str1->s32Cur));
   CuAssertIntEquals(tc, 0, memcmp(str2->pAlloc, &buf[0], str2->s32Cur));
   CuAssertTrue(tc, str1->pAlloc != str2->pAlloc);
   adt_str_delete(str1);
   adt_str_delete(str2);
}

static void test_adt_str_append_cstr(CuTest *tc)
{
   adt_str_t *str = adt_str_new();
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_append_cstr(str, "Hello"));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_append_cstr(str, " "));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_append_cstr(str, "World!"));
   CuAssertIntEquals(tc, 12, adt_str_size(str));
   CuAssertStrEquals(tc, "Hello World!", adt_str_cstr(str));
   adt_str_delete(str);
}

static void test_adt_str_append_bstr(CuTest *tc)
{
   char buf[20];
   const char *a, *b, *c, *d;
   adt_str_t *str;

   strcpy(buf, "Hello World!");
   a=buf, b=buf+5;
   c=buf+5, d=buf+12;
   str = adt_str_new();
   CuAssertPtrNotNull(tc, str);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_append_bstr(str, (const uint8_t*) a, (const uint8_t*) b));
   CuAssertIntEquals(tc, 5, adt_str_size(str));
   CuAssertStrEquals(tc, "Hello", adt_str_cstr(str));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_append_bstr(str, (const uint8_t*) c, (const uint8_t*) d));
   CuAssertIntEquals(tc, 12, adt_str_size(str));
   CuAssertStrEquals(tc, "Hello World!", adt_str_cstr(str));
   adt_str_delete(str);
}

static void test_adt_str_append(CuTest *tc)
{
   adt_str_t *str1;
   adt_str_t *str2;
   adt_str_t *str3;

   str1 = adt_str_new_cstr("Hello");
   str2 = adt_str_new_cstr(" ");
   str3 = adt_str_new_cstr("World!");
   CuAssertPtrNotNull(tc, str1);
   CuAssertPtrNotNull(tc, str2);
   CuAssertPtrNotNull(tc, str3);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_append(str1, str2));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_append(str1, str3));
   CuAssertIntEquals(tc, 12, adt_str_size(str1));
   CuAssertIntEquals(tc, 12, adt_str_length(str1));
   CuAssertStrEquals(tc, "Hello World!", adt_str_cstr(str1));
   adt_str_delete(str3);
   adt_str_delete(str2);
   adt_str_delete(str1);
}

static void test_adt_str_append_cstr_utf8(CuTest *tc)
{
   const char *test1 = "\327\220\327\206"; //\xD7\x90 \xD7\x86
   const char *test2 = "\327\252"; //\xD7\xAA
   adt_str_t *str = adt_str_new();

   adt_str_append_cstr(str, test1);
   CuAssertIntEquals(tc, 4, adt_str_size(str));
   CuAssertIntEquals(tc, 2, adt_str_length(str));
   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_str_getEncoding(str));

   adt_str_append_cstr(str, test2);
   CuAssertIntEquals(tc, 6, adt_str_size(str));
   CuAssertIntEquals(tc, 3, adt_str_length(str));
   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_str_getEncoding(str));

   adt_str_delete(str);
}

static void test_adt_str_append_bstr_utf8(CuTest *tc)
{
   const char *test1 = "\327\220\327\206"; //\xD7\x90 \xD7\x86
   const char *test2 = "\327\252"; //\xD7\xAA
   const char *a, *b;
   adt_str_t *str = adt_str_new();


   a = test1, b = a+strlen(test1);
   adt_str_append_bstr(str, (const uint8_t*) a, (const uint8_t*) b);
   CuAssertIntEquals(tc, 4, adt_str_size(str));
   CuAssertIntEquals(tc, 2, adt_str_length(str));
   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_str_getEncoding(str));


   a = test2, b = a+strlen(test2);
   adt_str_append_bstr(str, (const uint8_t*) a, (const uint8_t*) b);
   CuAssertIntEquals(tc, 6, adt_str_size(str));
   CuAssertIntEquals(tc, 3, adt_str_length(str));
   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_str_getEncoding(str));

   adt_str_delete(str);

}

static void test_adt_str_push(CuTest* tc){
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

static void test_adt_str_pop(CuTest* tc){
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


static void test_adt_str_charAt(CuTest* tc){
	adt_str_t *str = adt_str_new_cstr("Hello");
	CuAssertPtrNotNull(tc, str);
	CuAssertIntEquals(tc,'H',adt_str_charAt(str,0));
	CuAssertIntEquals(tc,'e',adt_str_charAt(str,1));
	CuAssertIntEquals(tc,'l',adt_str_charAt(str,2));
	CuAssertIntEquals(tc,'l',adt_str_charAt(str,3));
	CuAssertIntEquals(tc,'o',adt_str_charAt(str,4));
	CuAssertIntEquals(tc,-1,adt_str_charAt(str,5));
	CuAssertIntEquals(tc,'o',adt_str_charAt(str,-1));
	CuAssertIntEquals(tc,'l',adt_str_charAt(str,-2));
	CuAssertIntEquals(tc,'l',adt_str_charAt(str,-3));
	CuAssertIntEquals(tc,'e',adt_str_charAt(str,-4));
	CuAssertIntEquals(tc,'H',adt_str_charAt(str,-5));
	CuAssertIntEquals(tc,-1,adt_str_charAt(str,-6));
	adt_str_delete(str);
}

static void test_adt_utf8_readCodePoint1(CuTest* tc)
{
   uint8_t str[1];
   const int32_t expectedSize = 1;
   int codePoint;

   str[0] = 0; //lowest codepoint using 1-byte encoding
   CuAssertIntEquals(tc, expectedSize, adt_utf8_readCodePoint(&str[0], expectedSize, &codePoint));
   CuAssertIntEquals(tc, 0, codePoint);

   str[0] = 127; //highest codepoint using 1-byte encoding
   CuAssertIntEquals(tc, expectedSize, adt_utf8_readCodePoint(&str[0], expectedSize, &codePoint));
   CuAssertIntEquals(tc, 127, codePoint);

}


static void test_adt_utf8_readCodePoint2(CuTest* tc)
{
   uint8_t str[2];
   const int32_t expectedSize = 2;
   int codePoint;

   str[0] = 0xC2, str[1] = 0x80; //lowest codepoint using 2-byte encoding
   CuAssertIntEquals(tc, expectedSize, adt_utf8_readCodePoint(&str[0], expectedSize, &codePoint));
   CuAssertIntEquals(tc, 128, codePoint);

   str[0] = 0xDF, str[1] = 0xbf; //highest codepoint using 2-byte encoding
   CuAssertIntEquals(tc, expectedSize, adt_utf8_readCodePoint(&str[0], expectedSize, &codePoint));
   CuAssertIntEquals(tc, 2047, codePoint);

}

static void test_adt_utf8_readCodePoint3(CuTest* tc)
{
   uint8_t str[3];
   const int32_t expectedSize = 3;
   int codePoint;

   str[0] = 0xe0, str[1] = 0xA0, str[2]  = 0x80; //lowest codepoint using 3-byte encoding
   CuAssertIntEquals(tc, expectedSize, adt_utf8_readCodePoint(&str[0], expectedSize, &codePoint));
   CuAssertIntEquals(tc, 2048, codePoint);

   str[0] = 0xef, str[1] = 0xbf, str[2]  = 0xbf; //highest codepoint using 3-byte encoding
   CuAssertIntEquals(tc, expectedSize, adt_utf8_readCodePoint(&str[0], expectedSize, &codePoint));
   CuAssertIntEquals(tc, 65535, codePoint);

}

static void test_adt_utf8_readCodePoint4(CuTest* tc)
{
   uint8_t str[4];
   const int32_t expectedSize = 4;
   int codePoint;

   str[0] = 0xf0, str[1] = 0x90, str[2]  = 0x80, str[3] = 0x80; //lowest codepoint using 4-byte encoding
   CuAssertIntEquals(tc, expectedSize, adt_utf8_readCodePoint(&str[0], expectedSize, &codePoint));
   CuAssertIntEquals(tc, 65536, codePoint);

   str[0] = 0xf4, str[1] = 0x8f, str[2]  = 0xbf, str[3] = 0xbf; //highest valid codepoint in unicode
   CuAssertIntEquals(tc, expectedSize, adt_utf8_readCodePoint(&str[0], expectedSize, &codePoint));
   CuAssertIntEquals(tc, 1114111, codePoint);

}

static void test_adt_utf8_checkEncoding_ascii(CuTest* tc)
{
   const char *test1 = "abc123";
   const char *test2 = "Hello World";
   const char *test3 = "Hello\r\nWorld\b";
   int32_t size;
   const uint8_t *test;
   int32_t strLen;

   test = (const uint8_t*) test1, strLen = (int32_t) strlen( (const char*) test);
   CuAssertIntEquals(tc, 6, strLen);
   CuAssertIntEquals(tc, ADT_STR_ENCODING_ASCII, adt_utf8_checkEncodingAndSize(test, strLen, &size));
   CuAssertIntEquals(tc, strLen, size);

   test = (const uint8_t*) test2, strLen = (int32_t) strlen( (const char*) test);
   CuAssertIntEquals(tc, 11, strLen);
   CuAssertIntEquals(tc, ADT_STR_ENCODING_ASCII, adt_utf8_checkEncodingAndSize(test, strLen, &size));
   CuAssertIntEquals(tc, strLen, size);

   test = (const uint8_t*) test3, strLen = (int32_t) strlen( (const char*) test);
   CuAssertIntEquals(tc, 13, strLen);
   CuAssertIntEquals(tc, ADT_STR_ENCODING_ASCII, adt_utf8_checkEncodingAndSize(test, strLen, &size));
   CuAssertIntEquals(tc, strLen, size);

}

static void test_adt_utf8_checkEncoding_utf8(CuTest* tc)
{
   int32_t size;
   const uint8_t test1[2] = {0302, 0241};
   const uint8_t test2[3] = {0343, 0202, 0206};
   const uint8_t test3[4] = {0360, 0237, 0245, 0214};

   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_utf8_checkEncodingAndSize(test1, sizeof(test1), &size));

   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_utf8_checkEncodingAndSize(test2, sizeof(test2), &size));

   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_utf8_checkEncodingAndSize(test3, sizeof(test3), &size));
}

static void test_adt_utf8_checkEncodingAndSize(CuTest* tc)
{
   const char *test1 = "Hello World";
   const char *test2 = "Test \343\202\206";
   int32_t size;
   CuAssertIntEquals(tc, ADT_STR_ENCODING_ASCII, adt_utf8_checkEncodingAndSize( (const uint8_t*) test1, 0, &size));
   CuAssertIntEquals(tc, 11, size);

   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_utf8_checkEncodingAndSize( (const uint8_t*) test2,  0, &size));
   CuAssertIntEquals(tc, 8, size);

}

static void test_adt_str_equal_cstr(CuTest* tc)
{
   const char *test1 = "Hello World\n";

   adt_str_t *str = adt_str_new();

   adt_str_set_cstr(str, test1);
   CuAssertTrue(tc, adt_str_equal_cstr(str, test1));

   adt_str_delete(str);

}

static void test_adt_str_lt_utf8(CuTest* tc)
{
   const char *left_cstr = "\113\303\266\160\145\156\150\141\155\156"; //Köpenhamn
   const char *right_cstr = "\113\303\266\160\151\156\147"; //Köping
   adt_str_t *left = adt_str_new_cstr(left_cstr);
   adt_str_t *right = adt_str_new_cstr(right_cstr);
   CuAssertPtrNotNull(tc, left);
   CuAssertPtrNotNull(tc, right);
   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_str_getEncoding(left));
   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_str_getEncoding(right));
   CuAssertIntEquals(tc, 9, adt_str_length(left));
   CuAssertIntEquals(tc, 6, adt_str_length(right));
   CuAssertTrue(tc, adt_str_lt(left, right)); //letter e is less than letter i in fourth code point

   adt_str_delete(left);
   adt_str_delete(right);
}

static void test_adt_str_new_byterray(CuTest* tc)
{
   const char *cstr = "\113\303\266\160\145\156\150\141\155\156"; //Köpenhamn
   adt_bytearray_t *tmpArray;
   adt_str_t *str;

   tmpArray = adt_bytearray_make_cstr(cstr, ADT_BYTE_ARRAY_NO_GROWTH);

   CuAssertPtrNotNull(tc, tmpArray);

   str = adt_str_new_bytearray(tmpArray);
   CuAssertPtrNotNull(tc, str);
   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_str_getEncoding(str));
   CuAssertIntEquals(tc, 9, adt_str_length(str));


   adt_str_delete(str);
   adt_bytearray_delete(tmpArray);
}

static void test_adt_str_bytearray(CuTest* tc)
{
   const char *cstr = "Test";
   adt_str_t *str;
   adt_bytearray_t *data;
   str = adt_str_new_cstr(cstr);
   CuAssertPtrNotNull(tc, str);

   data = adt_str_bytearray(str);
   CuAssertPtrNotNull(tc, data);
   CuAssertUIntEquals(tc, 4, adt_bytearray_length(data));
   CuAssertIntEquals(tc, 0, memcmp(cstr, adt_bytearray_data(data), 4));
   adt_bytearray_delete(data);
   adt_str_delete(str);
}

static void test_adt_str_bytes(CuTest* tc)
{
   const char *cstr = "Test";
   adt_str_t *str;
   adt_bytes_t *bytes;
   str = adt_str_new_cstr(cstr);
   CuAssertPtrNotNull(tc, str);

   bytes = adt_str_bytes(str);
   CuAssertPtrNotNull(tc, bytes);
   CuAssertUIntEquals(tc, 4, adt_bytes_length(bytes));
   CuAssertIntEquals(tc, 0, memcmp(cstr, adt_bytes_constData(bytes), 4));
   adt_bytes_delete(bytes);
   adt_str_delete(str);
}

static void test_adt_str_clear(CuTest* tc)
{
   const char *test1 = "\327\220\327\206\327\252"; //\xD7\x90 \xD7\x86 \xD7\xAA
   const char *test2 = "Hello";
   adt_str_t *str = adt_str_new();

   adt_str_append_cstr(str, test1);
   CuAssertIntEquals(tc, 6, adt_str_size(str));
   CuAssertIntEquals(tc, 3, adt_str_length(str));
   CuAssertIntEquals(tc, ADT_STR_ENCODING_UTF8, adt_str_getEncoding(str));
   adt_str_clear(str);
   CuAssertIntEquals(tc, ADT_STR_ENCODING_ASCII, adt_str_getEncoding(str));
   adt_str_append_cstr(str, test2);
   CuAssertIntEquals(tc, 5, adt_str_size(str));
   CuAssertIntEquals(tc, 5, adt_str_length(str));
   CuAssertIntEquals(tc, ADT_STR_ENCODING_ASCII, adt_str_getEncoding(str));

   adt_str_delete(str);
}

static void test_adt_adding_nulls_at_end_shall_not_be_part_of_length(CuTest* tc)
{
   const uint8_t test1[6] = {'a', 'b', 'c', 0, 0, 0};
   const uint8_t test2[3] = {0, 0, 0};
   const uint8_t test3[3] = {'a', 0, 0};
   adt_str_t *str = adt_str_new();
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_set_bstr(str, test1, test1+sizeof(test1)));
   CuAssertIntEquals(tc, 3, adt_str_length(str));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_set_bstr(str, test2, test1+sizeof(test2)));
   CuAssertIntEquals(tc, 0, adt_str_length(str));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_set_bstr(str, test3, test1+sizeof(test3)));
   CuAssertIntEquals(tc, 1, adt_str_length(str));
   adt_str_delete(str);
}

static void test_adt_str_append_bstr_containing_nulls_at_end(CuTest* tc)
{
   const uint8_t test1[6] = {'a', 'b', 'c', 0, 0, 0};
   const uint8_t test2[3] = {0, 0, 0};
   const uint8_t test3[3] = {'a', 0, 0};
   adt_str_t *str = adt_str_new();
   CuAssertPtrNotNull(tc, str);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_append_bstr(str, test1, test1+sizeof(test1)));
   CuAssertIntEquals(tc, 3, adt_str_length(str));
   adt_str_clear(str);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_append_bstr(str, test2, test1+sizeof(test2)));
   CuAssertIntEquals(tc, 0, adt_str_length(str));
   adt_str_clear(str);
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_str_append_bstr(str, test3, test1+sizeof(test3)));
   CuAssertIntEquals(tc, 1, adt_str_length(str));
   adt_str_delete(str);
}
