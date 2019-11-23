//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stddef.h>
#include "adt_heap.h"
#include "CuTest.h"
#ifdef MEM_LEAK_CHECK
# include "CMemLeak.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_heap_elem(CuTest* tc);
static void test_min_heap_sortUp(CuTest* tc);
static void test_min_heap_sortDown(CuTest* tc);


//////////////////////////////////////////////////////////////////////////////
// LOCAL VARIABLES
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_adt_priorityHeap(void){
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_heap_elem);
   SUITE_ADD_TEST(suite, test_min_heap_sortUp);
   SUITE_ADD_TEST(suite, test_min_heap_sortDown);

   return suite;

}



//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void test_heap_elem(CuTest* tc){
   adt_heap_elem_t elem1;
   adt_heap_elem_t *elem2;
   int item1 = 10;
   int item2 = 20;

   //test init on stack
   adt_heap_elem_create(&elem1, &item1, 101);
   CuAssertPtrEquals(tc, &item1, elem1.pItem);
   CuAssertUIntEquals(tc, 101, elem1.u32Value);

   //test init from heap
   elem2 = adt_heap_elem_new(&item2, 102);
   CuAssertPtrNotNull(tc, elem2);
   CuAssertPtrEquals(tc, &item2, elem2->pItem);
   CuAssertUIntEquals(tc, 102, elem2->u32Value);
   adt_heap_elem_delete(elem2);
}

/**
 * test that the bubble up strategy works when pushing new items with lower values/priorities at the end
 */
static void test_min_heap_sortUp(CuTest* tc){
   adt_ary_t heap;
   adt_heap_elem_t *elem1;
   adt_heap_elem_t *elem2;
   adt_heap_elem_t *elem3;
   adt_heap_elem_t *elem4;
   adt_heap_elem_t *elem5;
   adt_heap_elem_t *elem6;
   adt_ary_create(&heap, adt_heap_elem_vdelete);
   CuAssertIntEquals(tc, 0, adt_ary_length(&heap));

   //add elem1
   elem1 = adt_heap_elem_new(NULL, 10);
   CuAssertPtrNotNull(tc, elem1);
   adt_ary_push(&heap, elem1);
   CuAssertIntEquals(tc, 1, adt_ary_length(&heap));
   adt_heap_sortUp(&heap, adt_ary_length(&heap)-1, ADT_MIN_HEAP); //this shall have no effect

   //add elem2
   elem2 = adt_heap_elem_new(NULL, 8);
   adt_ary_push(&heap, elem2);
   CuAssertIntEquals(tc, 2, adt_ary_length(&heap));
   adt_heap_sortUp(&heap, adt_ary_length(&heap)-1, ADT_MIN_HEAP);

   //expect elem2, elem1
   CuAssertPtrEquals(tc, (void*) elem2, adt_ary_value(&heap, 0));
   CuAssertPtrEquals(tc, (void*) elem1, adt_ary_value(&heap, 1));

   //add elem3
   elem3 = adt_heap_elem_new(NULL, 6);
   adt_ary_push(&heap, elem3);
   CuAssertIntEquals(tc, 3, adt_ary_length(&heap));
   adt_heap_sortUp(&heap, adt_ary_length(&heap)-1, ADT_MIN_HEAP);

   //expect elem3, elem1, elem2
   CuAssertPtrEquals(tc, (void*) elem3, adt_ary_value(&heap, 0));
   CuAssertPtrEquals(tc, (void*) elem1, adt_ary_value(&heap, 1));
   CuAssertPtrEquals(tc, (void*) elem2, adt_ary_value(&heap, 2));

   //add elem4
   elem4 = adt_heap_elem_new(NULL, 4);
   adt_ary_push(&heap, elem4);
   CuAssertIntEquals(tc, 4, adt_ary_length(&heap));
   adt_heap_sortUp(&heap, adt_ary_length(&heap)-1, ADT_MIN_HEAP);

   //expect elem4, elem3, elem2, elem1
   CuAssertPtrEquals(tc, (void*) elem4, adt_ary_value(&heap, 0));
   CuAssertPtrEquals(tc, (void*) elem3, adt_ary_value(&heap, 1));
   CuAssertPtrEquals(tc, (void*) elem2, adt_ary_value(&heap, 2));
   CuAssertPtrEquals(tc, (void*) elem1, adt_ary_value(&heap, 3));

   //add elem5
   elem5 = adt_heap_elem_new(NULL, 2);
   adt_ary_push(&heap, elem5);
   CuAssertIntEquals(tc, 5, adt_ary_length(&heap));
   adt_heap_sortUp(&heap, adt_ary_length(&heap)-1, ADT_MIN_HEAP);

   //expect elem5, elem4, elem2, elem1, elem3
   CuAssertPtrEquals(tc, (void*) elem5, adt_ary_value(&heap, 0));
   CuAssertPtrEquals(tc, (void*) elem4, adt_ary_value(&heap, 1));
   CuAssertPtrEquals(tc, (void*) elem2, adt_ary_value(&heap, 2));
   CuAssertPtrEquals(tc, (void*) elem1, adt_ary_value(&heap, 3));
   CuAssertPtrEquals(tc, (void*) elem3, adt_ary_value(&heap, 4));

   //add elem6
   elem6 = adt_heap_elem_new(NULL, 0);
   adt_ary_push(&heap, elem6);
   CuAssertIntEquals(tc, 6, adt_ary_length(&heap));
   adt_heap_sortUp(&heap, adt_ary_length(&heap)-1, ADT_MIN_HEAP);

   //expect elem6, elem4, elem5, elem1, elem3, elem2
   CuAssertPtrEquals(tc, (void*) elem6, adt_ary_value(&heap, 0));
   CuAssertPtrEquals(tc, (void*) elem4, adt_ary_value(&heap, 1));
   CuAssertPtrEquals(tc, (void*) elem5, adt_ary_value(&heap, 2));
   CuAssertPtrEquals(tc, (void*) elem1, adt_ary_value(&heap, 3));
   CuAssertPtrEquals(tc, (void*) elem3, adt_ary_value(&heap, 4));
   CuAssertPtrEquals(tc, (void*) elem2, adt_ary_value(&heap, 5));

   adt_ary_destroy(&heap);
}

static void test_min_heap_sortDown(CuTest* tc){
   adt_ary_t heap;
   adt_heap_elem_t *elem1;
   adt_heap_elem_t *elem2;
   adt_heap_elem_t *elem3;
   adt_heap_elem_t *elem4;

   adt_ary_create(&heap, adt_heap_elem_vdelete);
   CuAssertIntEquals(tc, 0, adt_ary_length(&heap));

   //add elem1
   elem1 = adt_heap_elem_new(NULL, 0);
   CuAssertPtrNotNull(tc, elem1);
   adt_ary_unshift(&heap, elem1);
   CuAssertIntEquals(tc, 1, adt_ary_length(&heap));
   adt_heap_sortDown(&heap, 0, ADT_MIN_HEAP); //this shall have no effect

   //add elem2
   elem2 = adt_heap_elem_new(NULL, 2);
   adt_ary_unshift(&heap, elem2);
   CuAssertIntEquals(tc, 2, adt_ary_length(&heap));
   adt_heap_sortDown(&heap, 0, ADT_MIN_HEAP);

   //expect elem1, elem2
   CuAssertPtrEquals(tc, (void*) elem1, adt_ary_value(&heap, 0));
   CuAssertPtrEquals(tc, (void*) elem2, adt_ary_value(&heap, 1));

   //add elem3
   elem3 = adt_heap_elem_new(NULL, 4);
   adt_ary_unshift(&heap, elem3);
   CuAssertIntEquals(tc, 3, adt_ary_length(&heap));
   adt_heap_sortDown(&heap, 0, ADT_MIN_HEAP);

   //expect elem1, elem3, elem2
   CuAssertPtrEquals(tc, (void*) elem1, adt_ary_value(&heap, 0));
   CuAssertPtrEquals(tc, (void*) elem3, adt_ary_value(&heap, 1));
   CuAssertPtrEquals(tc, (void*) elem2, adt_ary_value(&heap, 2));

   //add elem4
   elem4 = adt_heap_elem_new(NULL, 6);
   adt_ary_unshift(&heap, elem4);
   CuAssertIntEquals(tc, 4, adt_ary_length(&heap));
   adt_heap_sortDown(&heap, 0, ADT_MIN_HEAP);

   //expect elem1, elem2, elem3, elem4
   CuAssertPtrEquals(tc, (void*) elem1, adt_ary_value(&heap, 0));
   CuAssertPtrEquals(tc, (void*) elem2, adt_ary_value(&heap, 1));
   CuAssertPtrEquals(tc, (void*) elem3, adt_ary_value(&heap, 2));
   CuAssertPtrEquals(tc, (void*) elem4, adt_ary_value(&heap, 3));

   adt_ary_destroy(&heap);
}
