#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "adt_list.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

/**************** Private Function Declarations *******************/
static void test_adt_list_new(CuTest* tc);
static void test_adt_list_insert_remove(CuTest* tc);
static void test_adt_list_insert_unique(CuTest* tc);
static void test_adt_list_insert_erase(CuTest* tc);
static void test_adt_list_clear(CuTest* tc);
static void vfree(void *arg);

/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
CuSuite* testsuite_adt_list(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_list_new);
   SUITE_ADD_TEST(suite, test_adt_list_insert_remove);
   SUITE_ADD_TEST(suite, test_adt_list_insert_unique);
   SUITE_ADD_TEST(suite, test_adt_list_insert_erase);
   SUITE_ADD_TEST(suite, test_adt_list_clear);



   return suite;
}

/***************** Private Function Definitions *******************/
static void test_adt_list_new(CuTest* tc)
{
   adt_list_t *list;
   list = adt_list_new(vfree);
   CuAssertPtrNotNull(tc,list);
   adt_list_delete(list);
}

static void test_adt_list_insert_remove(CuTest* tc)
{
   adt_list_t *list;
   list = adt_list_new(NULL);
   char *hello=strdup("hello");
   char *world=strdup("world");
   CuAssertPtrNotNull(tc,list);
   adt_list_insert(list, hello);
   adt_list_insert(list, world);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   adt_list_remove(list, hello);
   CuAssertIntEquals(tc, 1, adt_list_length(list));
   adt_list_remove(list, hello);
   CuAssertIntEquals(tc, 1, adt_list_length(list));
   adt_list_remove(list, world);
   CuAssertIntEquals(tc, 0, adt_list_length(list));
   adt_list_delete(list);
   free(hello);
   free(world);
}

static void test_adt_list_insert_unique(CuTest* tc)
{
   adt_list_t *list;
   list = adt_list_new(vfree);
   char *hello=strdup("hello");
   char *world=strdup("world");
   CuAssertPtrNotNull(tc,list);
   adt_list_insert_unique(list, hello);
   adt_list_insert_unique(list, world);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   adt_list_insert_unique(list, hello);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   adt_list_insert_unique(list, world);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   adt_list_delete(list);
}

static void test_adt_list_insert_erase(CuTest* tc)
{
   adt_list_t *list;
   adt_list_elem_t *iter;
   list = adt_list_new(NULL);
   char *hello=strdup("hello");
   char *world=strdup("world");
   CuAssertPtrNotNull(tc,list);
   adt_list_insert(list, hello);
   adt_list_insert(list, world);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   iter = adt_list_iter_last(list);
   CuAssertPtrNotNull(tc, iter);
   CuAssertPtrEquals(tc, world, iter->pItem);
   adt_list_erase(list, iter);
   CuAssertIntEquals(tc, 1, adt_list_length(list));
   iter = adt_list_iter_last(list);
   CuAssertPtrNotNull(tc, iter);
   CuAssertPtrEquals(tc, hello, iter->pItem);
   adt_list_erase(list, iter);
   CuAssertIntEquals(tc, 0, adt_list_length(list));
   adt_list_delete(list);
   free(hello);
   free(world);
}


static void test_adt_list_clear(CuTest* tc)
{
   adt_list_t *list;
   list = adt_list_new(vfree);
   char *hello=strdup("hello");
   char *world=strdup("world");
   char *foo=strdup("foo");
   CuAssertPtrNotNull(tc,list);
   adt_list_insert(list, hello);
   adt_list_insert(list, world);
   CuAssertIntEquals(tc, 2, adt_list_length(list));
   adt_list_clear(list);
   CuAssertIntEquals(tc, 0, adt_list_length(list));
   adt_list_insert(list, foo);
   CuAssertIntEquals(tc, 1, adt_list_length(list));
   adt_list_delete(list);
}

static void vfree(void *arg)
{
   free(arg);
}
