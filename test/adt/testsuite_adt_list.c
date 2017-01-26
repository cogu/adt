#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "adt_list.h"
#include "CMemLeak.h"

/**************** Private Function Declarations *******************/
static void test_adt_list_new(CuTest* tc);
static void test_adt_ary_insert_remove(CuTest* tc);
static void vfree(void *arg);

/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
CuSuite* testsuite_adt_list(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_list_new);
   SUITE_ADD_TEST(suite, test_adt_ary_insert_remove);

   return suite;
}

/***************** Private Function Definitions *******************/
void test_adt_list_new(CuTest* tc)
{
   adt_list_t *list;
   list = adt_list_new(vfree);
   CuAssertPtrNotNull(tc,list);
   adt_list_delete(list);
}

void test_adt_ary_insert_remove(CuTest* tc)
{
   adt_list_t *list;
   list = adt_list_new(vfree);
   char *hello=strdup("hello");
   char *world=strdup("world");
   CuAssertPtrNotNull(tc,list);
   adt_list_insert(list,hello);
   adt_list_insert(list,world);
   adt_list_delete(list);
}

static void vfree(void *arg)
{
   free(arg);
}
