//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdbool.h>
#include <assert.h>
#include <malloc.h>
#include "CuTest.h"
#include "adt_set.h"
#ifdef MEM_LEAK_CHECK
# include "CMemLeak.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_adt_u32Set_create(CuTest* tc);
static void test_adt_u32Set_new_delete(CuTest* tc);
static void test_adt_u32Set_insert_in_empty_set(CuTest* tc);
static void test_adt_u32Set_insert_multiple_values(CuTest* tc);
static void test_adt_u32Set_insert_duplicates(CuTest* tc);
static void test_adt_u32Set_contains(CuTest* tc);
static void test_adt_u32Set_remove(CuTest* tc);
static void test_adt_u32Set_is_empty(CuTest* tc);

//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_adt_u32Set(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_u32Set_create);
   SUITE_ADD_TEST(suite, test_adt_u32Set_new_delete);
   SUITE_ADD_TEST(suite, test_adt_u32Set_insert_in_empty_set);
   SUITE_ADD_TEST(suite, test_adt_u32Set_insert_multiple_values);
   SUITE_ADD_TEST(suite, test_adt_u32Set_insert_duplicates);
   SUITE_ADD_TEST(suite, test_adt_u32Set_contains);
   SUITE_ADD_TEST(suite, test_adt_u32Set_remove);
   SUITE_ADD_TEST(suite, test_adt_u32Set_is_empty);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void test_adt_u32Set_create(CuTest* tc)
{
   adt_u32Set_t set;
   adt_u32Set_create(&set);
   CuAssertTrue(tc, adt_u32List_is_empty(&set.list));
   adt_u32Set_destroy(&set);
}

static void test_adt_u32Set_insert_in_empty_set(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32Set_create(&set);

   adt_u32Set_insert(&set, 10);
   CuAssertIntEquals(tc, 1, adt_u32Set_length(&set));

   //cleanup
   adt_u32Set_destroy(&set);
}

static void test_adt_u32Set_new_delete(CuTest* tc)
{
   adt_u32Set_t *set = adt_u32Set_new();
   CuAssertPtrNotNull(tc, set);
   adt_u32Set_delete(set);
}

static void test_adt_u32Set_insert_multiple_values(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32List_elem_t *iter;
   adt_u32Set_create(&set);

   //ordered insertion
   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 3);
   adt_u32Set_insert(&set, 4);
   adt_u32Set_insert(&set, 5);

   CuAssertIntEquals(tc, 5, adt_u32Set_length(&set));

   //ordered insertion (backwards)
   adt_u32Set_clear(&set);
   adt_u32Set_insert(&set, 5);
   adt_u32Set_insert(&set, 4);
   adt_u32Set_insert(&set, 3);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 1);

   CuAssertIntEquals(tc, 5, adt_u32Set_length(&set));
   //make sure list is sorted
   iter = adt_u32List_iter_first(&set.list);
   CuAssertUIntEquals(tc, 1, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 2, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 3, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 4, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 5, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertPtrEquals(tc, 0, iter);

   //unordered insertion
   adt_u32Set_clear(&set);
   adt_u32Set_insert(&set, 3);
   adt_u32Set_insert(&set, 5);
   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 4);
   adt_u32Set_insert(&set, 2);

   CuAssertIntEquals(tc, 5, adt_u32Set_length(&set));
   //make sure list is still sorted
   iter = adt_u32List_iter_first(&set.list);
   CuAssertUIntEquals(tc, 1, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 2, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 3, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 4, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 5, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertPtrEquals(tc, 0, iter);

   //cleanup
   adt_u32Set_destroy(&set);
}

static void test_adt_u32Set_insert_duplicates(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32List_elem_t *iter;
   adt_u32Set_create(&set);

   adt_u32Set_insert(&set, 3);
   adt_u32Set_insert(&set, 5);
   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 4);
   adt_u32Set_insert(&set, 5);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 4);
   adt_u32Set_insert(&set, 3);
   adt_u32Set_insert(&set, 2);
   CuAssertIntEquals(tc, 5, adt_u32Set_length(&set));
   iter = adt_u32List_iter_first(&set.list);
   CuAssertUIntEquals(tc, 1, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 2, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 3, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 4, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertUIntEquals(tc, 5, iter->item);
   iter = adt_u32List_iter_next(iter);
   CuAssertPtrEquals(tc, 0, iter);

   //cleanup
   adt_u32Set_destroy(&set);
}

static void test_adt_u32Set_contains(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32Set_create(&set);
   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 6);
   adt_u32Set_insert(&set, 5);
   adt_u32Set_insert(&set, 9);
   adt_u32Set_insert(&set, 11);
   adt_u32Set_insert(&set, 8);

   CuAssertTrue(tc, adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 3));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 4));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 5));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 6));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 7));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 8));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 9));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 10));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 11));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 12));

   //cleanup
   adt_u32Set_destroy(&set);

}

static void test_adt_u32Set_remove(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32Set_create(&set);
   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 3);

   CuAssertTrue(tc, adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 3));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 4));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 1));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 2));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 3));
   CuAssertTrue(tc, !adt_u32Set_remove(&set, 4));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 3));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 4));
   CuAssertIntEquals(tc, 0, adt_u32Set_length(&set));

   adt_u32Set_insert(&set, 1);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 3);
   CuAssertTrue(tc, adt_u32Set_remove(&set, 2));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 3));

   adt_u32Set_insert(&set, 0xFFFFFFFF);
   adt_u32Set_insert(&set, 2);
   adt_u32Set_insert(&set, 4);
   CuAssertTrue(tc, adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 3));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 4));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 0xFFFFFFFF));

   CuAssertTrue(tc, adt_u32Set_remove(&set, 1));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 3));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 4));
   CuAssertTrue(tc, adt_u32Set_remove(&set, 0xFFFFFFFF));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 1));
   CuAssertTrue(tc, adt_u32Set_contains(&set, 2));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 3));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 4));
   CuAssertTrue(tc, !adt_u32Set_contains(&set, 0xFFFFFFFF));


   //cleanup
   adt_u32Set_destroy(&set);

}

static void test_adt_u32Set_is_empty(CuTest* tc)
{
   //setup
   adt_u32Set_t set;
   adt_u32Set_create(&set);
   CuAssertTrue(tc, adt_u32Set_is_empty(&set));
   adt_u32Set_insert(&set, 1);
   CuAssertTrue(tc, !adt_u32Set_is_empty(&set));
}
