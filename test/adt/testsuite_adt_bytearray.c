#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "adt_bytearray.h"
#include "CMemLeak.h"

static void test_adt_bytearray_new(CuTest* tc)
{
   adt_bytearray_t *pArray = adt_bytearray_new(0);
   CuAssertPtrNotNull(tc, pArray);
   CuAssertPtrEquals(tc, 0,pArray->pData);
   CuAssertIntEquals(tc, 0,pArray->u32CurLen);
   CuAssertIntEquals(tc,0,pArray->u32AllocLen);
   CuAssertIntEquals(tc,ADT_BYTE_ARRAY_DEFAULT_GROW_SIZE,pArray->u32GrowSize);
   adt_bytearray_delete(pArray);
}

static void test_adt_bytearray_resize(CuTest* tc)
{
   int8_t result;
   adt_bytearray_t *pArray = adt_bytearray_new(4096);
   result = adt_bytearray_resize(pArray, 1132);
   CuAssertIntEquals(tc,0,result);
   CuAssertIntEquals(tc,1132,pArray->u32CurLen);
}

CuSuite* testsuite_adt_bytearray(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_bytearray_new);
   SUITE_ADD_TEST(suite, test_adt_bytearray_resize);

   return suite;
}
