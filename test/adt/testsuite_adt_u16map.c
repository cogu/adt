/*****************************************************************************
* \file      testsuite_adt_u16map.c
* \author    Conny Gustafsson
* \date      2016-11-20
* \brief     Unit tests for adt_u16Map_t
*
* Copyright (c) 2016-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "test_common.h"
#include "adt_map.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

#define NUM_ELEM 20
#define NUM_ELEM_LARGE 2000

void test_adt_u16Map_insert(CuTest* tc){
   adt_u16MapElem_t elem[10];
   adt_u16Map_t map;
   adt_u16Map_t *pList = &map;
   adt_u16MapElem_t *it;
   adt_u16Map_create(pList, elem, 10, NULL);
   adt_u16Map_insert(pList,4,0);
   adt_u16Map_insert(pList,3,0);
   adt_u16Map_insert(pList,8,0);
   adt_u16Map_insert(pList,7,0);
   adt_u16Map_insert(pList,1,0);
   adt_u16Map_insert(pList,6,0);
   adt_u16Map_insert(pList,2,0);
   adt_u16Map_insert(pList,9,0);
   adt_u16Map_insert(pList,5,0);
   adt_u16Map_insert(pList,0,0);
   CuAssertIntEquals(tc,10,adt_u16Map_size(pList));
   adt_u16Map_insert(pList,10,0);
   CuAssertIntEquals(tc,10,adt_u16Map_size(pList));
   it = adt_u16Map_iter_init(pList,0);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,0,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,1,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,2,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,4,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,6,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,7,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,8,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,9,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrEquals(tc, NULL, it);
}

void test_adt_u16Map_duplicates(CuTest* tc){
   adt_u16MapElem_t elem[10];
   adt_u16Map_t map;
   adt_u16Map_t *pList = &map;
   adt_u16MapElem_t *it;
   adt_u16Map_create(pList, elem, 10, NULL);
   adt_u16Map_insert(pList,4,(void*) 1);
   adt_u16Map_insert(pList,3,(void*) 2);
   adt_u16Map_insert(pList,5,(void*) 3);
   adt_u16Map_insert(pList,4,(void*) 4);
   adt_u16Map_insert(pList,4,(void*) 5);
   adt_u16Map_insert(pList,5,(void*) 6);
   adt_u16Map_insert(pList,3,(void*) 7);
   adt_u16Map_insert(pList,3,(void*) 8);
   adt_u16Map_insert(pList,1,(void*) 9);
   adt_u16Map_insert(pList,1,(void*) 10);
   CuAssertIntEquals(tc,10,adt_u16Map_size(pList));
   adt_u16Map_insert(pList,10,0);
   CuAssertIntEquals(tc,10,adt_u16Map_size(pList));
   it = adt_u16Map_iter_init(pList,0);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,1,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,1,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,4,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,4,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,4,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrEquals(tc, NULL, it);
}

void test_adt_u16Map_find(CuTest* tc){
   adt_u16MapElem_t elem[10];
   adt_u16Map_t map;
   adt_u16Map_t *pList = &map;
   adt_u16MapElem_t *it;
   adt_u16Map_create(pList, elem, 10, NULL);
   adt_u16Map_insert(pList,4,(void*) 1);
   adt_u16Map_insert(pList,3,(void*) 2);
   adt_u16Map_insert(pList,5,(void*) 3);
   adt_u16Map_insert(pList,4,(void*) 4);
   adt_u16Map_insert(pList,4,(void*) 5);
   adt_u16Map_insert(pList,5,(void*) 6);
   adt_u16Map_insert(pList,3,(void*) 7);
   adt_u16Map_insert(pList,3,(void*) 8);
   adt_u16Map_insert(pList,1,(void*) 9);
   adt_u16Map_insert(pList,1,(void*) 10);
   CuAssertIntEquals(tc,10,adt_u16Map_size(pList));
   it = adt_u16Map_find(pList,0);
   CuAssertPtrEquals(tc, NULL, it);
   it = adt_u16Map_find(pList,1);
   CuAssertPtrNotNull(tc,it);
   it = adt_u16Map_iter_init(pList,it);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,1,it->key);
   CuAssertPtrEquals(tc,(void*)9,it->val);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,1,it->key);
   CuAssertPtrEquals(tc,(void*)10,it->val);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   CuAssertPtrEquals(tc,(void*)2,it->val);

   it = adt_u16Map_find(pList,5);
   CuAssertPtrNotNull(tc,it);
   it = adt_u16Map_iter_init(pList,it);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   CuAssertPtrEquals(tc,(void*)3,it->val);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   CuAssertPtrEquals(tc,(void*)6,it->val);
   it = adt_u16Map_iter_next(pList);
   CuAssertPtrEquals(tc, NULL, it);
}

void test_adt_u16Map_find_exact(CuTest* tc){
   adt_u16MapElem_t elem[10];
   adt_u16Map_t map;
   adt_u16Map_t *pList = &map;
   adt_u16MapElem_t *it;
   adt_u16Map_create(pList, elem, 10, NULL);
   adt_u16Map_insert(pList,4,(void*) 1);
   adt_u16Map_insert(pList,3,(void*) 2);
   adt_u16Map_insert(pList,5,(void*) 3);
   adt_u16Map_insert(pList,4,(void*) 4);
   adt_u16Map_insert(pList,4,(void*) 5);
   adt_u16Map_insert(pList,5,(void*) 6);
   adt_u16Map_insert(pList,3,(void*) 7);
   adt_u16Map_insert(pList,3,(void*) 8);
   adt_u16Map_insert(pList,1,(void*) 9);
   adt_u16Map_insert(pList,1,(void*) 10);
   CuAssertIntEquals(tc,10,adt_u16Map_size(pList));
   it = adt_u16Map_find_exact(pList,1,(void*) 1);
   CuAssertPtrEquals(tc, NULL, it);
   it = adt_u16Map_find_exact(pList,4,(void*) 7);
   CuAssertPtrEquals(tc, NULL, it);
   //last item of key 4
   it = adt_u16Map_find_exact(pList,4,(void*) 5);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,4,it->key);
   CuAssertPtrEquals(tc,(void*) 5,it->val);
   //first item of key 5
   it = adt_u16Map_find_exact(pList,5,(void*) 3);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   CuAssertPtrEquals(tc,(void*) 3,it->val);
   //mit item of key 3
   it = adt_u16Map_find_exact(pList,3,(void*) 7);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   CuAssertPtrEquals(tc,(void*) 7,it->val);
}

void test_adt_u16Map_sort(CuTest* tc){
   adt_u16MapElem_t data[NUM_ELEM];
   adt_u16Map_t map;

   adt_u16Map_create(&map, &data[0], NUM_ELEM, NULL);
   adt_u16Map_insert(&map,5,0);
   adt_u16Map_insert(&map,87,0);
   adt_u16Map_insert(&map,1245,0);
   adt_u16Map_insert(&map,2,0);
   adt_u16Map_insert(&map,11,0);
   adt_u16Map_insert(&map,1,0);
   adt_u16Map_insert(&map,600,0);
   adt_u16MapElem_t *elem;
   uint16_t i = 0;
   adt_u16Map_iter_init(&map,NULL);
   uint16_t expected[] = {1,2,5,11,87,600,1245};
   while((elem = adt_u16Map_iter_next(&map)) != 0){
      CuAssertPtrNotNull(tc,elem);
      CuAssertIntEquals(tc,expected[i++],elem->key);
   }
}

static uint32_t test_prng(uint32_t *state)
{
   *state = (*state * 1664525U) + 1013904223U;
   return *state;
}

void test_adt_u16Map_find_rand_set(CuTest* tc){
   adt_u16MapElem_t *data;
   adt_u16Map_t map;
   adt_u16MapElem_t *elem;
   uint32_t prng_state = 42U;
   data = malloc(sizeof(adt_u16MapElem_t)*NUM_ELEM_LARGE);
   CuAssertPtrNotNull(tc,data);

   adt_u16Map_create(&map, data, NUM_ELEM_LARGE, NULL);
   for(uint32_t i = 0; i < NUM_ELEM_LARGE - 1; i++){
      uint16_t key = (uint16_t)((test_prng(&prng_state) % 100U) + 1U);
      adt_u16Map_insert(&map, key, &data[i]);
   }

   adt_u16Map_insert(&map, 82, NULL);
   elem = adt_u16Map_find(&map, 82);
   CuAssertPtrNotNull(tc, elem);
   CuAssertIntEquals(tc, 82, elem->key);
   elem = adt_u16Map_find(&map, 101);
   CuAssertPtrEquals(tc, NULL, elem);
   free(data);
}

void test_adt_u16Map_move(CuTest* tc){
   adt_u16MapElem_t elem1[10];
   adt_u16MapElem_t elem2[10];
   adt_u16Map_t map1;
   adt_u16Map_t map2;
   adt_u16Map_t *pList1 = &map1;
   adt_u16Map_t *pList2 = &map2;

   adt_u16Map_create(pList1, elem1, 10, NULL);
   adt_u16Map_create(pList2, elem2, 10, NULL);
   adt_u16Map_insert(pList1,4,(void*) 1);
   adt_u16Map_insert(pList1,3,(void*) 2);
   adt_u16Map_insert(pList1,5,(void*) 3);
   adt_u16Map_insert(pList1,4,(void*) 4);
   adt_u16Map_insert(pList1,4,(void*) 5);
   adt_u16Map_insert(pList1,5,(void*) 6);
   adt_u16Map_insert(pList1,3,(void*) 7);
   adt_u16Map_insert(pList1,3,(void*) 8);
   adt_u16Map_insert(pList1,1,(void*) 9);
   adt_u16Map_insert(pList1,1,(void*) 10);
   CuAssertIntEquals(tc,10,adt_u16Map_size(pList1));
   CuAssertIntEquals(tc,0,adt_u16Map_size(pList2));
   adt_u16Map_move_elem(pList2,pList1,4);
   CuAssertIntEquals(tc,3,adt_u16Map_size(pList2));
   CuAssertIntEquals(tc,7,adt_u16Map_size(pList1));
}

#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
void test_adt_u16Map_new_delete(CuTest* tc){
   adt_u16Map_t *map = adt_u16Map_new(10, NULL);
   CuAssertPtrNotNull(tc, map);
   adt_u16Map_insert(map, 42, (void*) "Answer");
   CuAssertUIntEquals(tc, 1, adt_u16Map_size(map));
   adt_u16MapElem_t *elem = adt_u16Map_find(map, 42);
   CuAssertPtrNotNull(tc, elem);
   CuAssertPtrEquals(tc, (void*) "Answer", elem->val);
   adt_u16Map_delete(map);
}

void test_adt_u16Map_largeCapacity(CuTest* tc){
   adt_u16Map_t *map = adt_u16Map_new(65536, NULL);
   CuAssertPtrNotNull(tc, map);
   CuAssertUIntEquals(tc, 0, adt_u16Map_size(map));

   adt_u16Map_insert(map, 0, (void*) 1);
   adt_u16Map_insert(map, 65535, (void*) 2);
   CuAssertUIntEquals(tc, 2, adt_u16Map_size(map));

   adt_u16MapElem_t *elem0 = adt_u16Map_find(map, 0);
   CuAssertPtrNotNull(tc, elem0);
   CuAssertIntEquals(tc, 0, elem0->key);
   CuAssertPtrEquals(tc, (void*) 1, elem0->val);

   adt_u16MapElem_t *elemMax = adt_u16Map_find(map, 65535);
   CuAssertPtrNotNull(tc, elemMax);
   CuAssertIntEquals(tc, 65535, elemMax->key);
   CuAssertPtrEquals(tc, (void*) 2, elemMax->val);

   adt_u16Map_delete(map);
}
#endif

void test_adt_u16Map_remove_val(CuTest* tc){
   adt_u16MapElem_t elems[10];
   adt_u16Map_t map;
   adt_u16Map_create(&map, elems, 10, NULL);
   CuAssertUIntEquals(tc, 0, adt_u16Map_size(&map));

   adt_u16Map_insert(&map, 10, (void*) "A");
   adt_u16Map_insert(&map, 20, (void*) "B");
   adt_u16Map_insert(&map, 30, (void*) "A");
   adt_u16Map_insert(&map, 40, (void*) "C");
   adt_u16Map_insert(&map, 50, (void*) "A");
   CuAssertUIntEquals(tc, 5, adt_u16Map_size(&map));

   // Remove all entries with val == "A"
   adt_u16Map_remove_val(&map, (void*) "A");
   CuAssertUIntEquals(tc, 2, adt_u16Map_size(&map));

   // Remaining elements should be 20 ("B") and 40 ("C")
   adt_u16MapElem_t *elem20 = adt_u16Map_find(&map, 20);
   CuAssertPtrNotNull(tc, elem20);
   CuAssertPtrEquals(tc, (void*) "B", elem20->val);

   adt_u16MapElem_t *elem40 = adt_u16Map_find(&map, 40);
   CuAssertPtrNotNull(tc, elem40);
   CuAssertPtrEquals(tc, (void*) "C", elem40->val);

   CuAssertPtrEquals(tc, NULL, adt_u16Map_find(&map, 10));
   CuAssertPtrEquals(tc, NULL, adt_u16Map_find(&map, 30));
   CuAssertPtrEquals(tc, NULL, adt_u16Map_find(&map, 50));

   // Removing non-existent value should be a no-op
   adt_u16Map_remove_val(&map, (void*) "Z");
   CuAssertUIntEquals(tc, 2, adt_u16Map_size(&map));

   adt_u16Map_destroy(&map);
}

void test_adt_u16Map_move_large(CuTest* tc){
   const uint32_t count = 50;
   adt_u16MapElem_t elems1[100];
   adt_u16MapElem_t elems2[100];
   adt_u16Map_t map1_obj;
   adt_u16Map_t map2_obj;
   adt_u16Map_t *map1 = &map1_obj;
   adt_u16Map_t *map2 = &map2_obj;
   adt_u16Map_create(map1, elems1, 100, NULL);
   adt_u16Map_create(map2, elems2, 100, NULL);

   int val1 = 1;
   int val2 = 2;
   int values[50];

   adt_u16Map_insert(map1, 10, &val1);
   for (uint32_t i = 0; i < count; i++) {
      values[i] = (int)(i + 100);
      adt_u16Map_insert(map1, 50, &values[i]);
   }
   adt_u16Map_insert(map1, 90, &val2);
   CuAssertUIntEquals(tc, count + 2, adt_u16Map_size(map1));

   uint32_t moved = adt_u16Map_move_elem(map2, map1, 50);
   CuAssertUIntEquals(tc, count, moved);
   CuAssertUIntEquals(tc, 2, adt_u16Map_size(map1));
   CuAssertUIntEquals(tc, count, adt_u16Map_size(map2));

   // Verify map1 has key 10 and 90 remaining
   CuAssertPtrNotNull(tc, adt_u16Map_find(map1, 10));
   CuAssertPtrNotNull(tc, adt_u16Map_find(map1, 90));
   CuAssertPtrEquals(tc, NULL, adt_u16Map_find(map1, 50));

   // Verify map2 has all 50 items of key 50 in order
   adt_u16MapElem_t *it = adt_u16Map_iter_init(map2, NULL);
   for (uint32_t i = 0; i < count; i++) {
      CuAssertPtrNotNull(tc, it);
      CuAssertIntEquals(tc, 50, it->key);
      CuAssertPtrEquals(tc, &values[i], it->val);
      it = adt_u16Map_iter_next(map2);
   }
   CuAssertPtrEquals(tc, NULL, it);

   adt_u16Map_destroy(map1);
   adt_u16Map_destroy(map2);
}

void test_adt_u16Map_errors(CuTest* tc){
   adt_u16MapElem_t elem[2];
   adt_u16Map_t map;
   adt_u16Map_create(&map, elem, 2, NULL);

   // NULL self checks
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_u16Map_insert(NULL, 1, NULL));

   // Normal inserts return ADT_NO_ERROR
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16Map_insert(&map, 10, (void*) 1));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16Map_insert(&map, 20, (void*) 2));

   // Duplicate key+val returns ADT_NO_ERROR
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_u16Map_insert(&map, 10, (void*) 1));

   // Map is full (max_num_elem == 2): overflow error
   CuAssertIntEquals(tc, ADT_OVERFLOW_ERROR, adt_u16Map_insert(&map, 30, (void*) 3));

   adt_u16Map_destroy(&map);
}

CuSuite* testsuite_adt_u16Map(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_u16Map_insert);
   SUITE_ADD_TEST(suite, test_adt_u16Map_duplicates);
   SUITE_ADD_TEST(suite, test_adt_u16Map_find);
   SUITE_ADD_TEST(suite, test_adt_u16Map_find_exact);
   SUITE_ADD_TEST(suite, test_adt_u16Map_move);
   SUITE_ADD_TEST(suite, test_adt_u16Map_find_rand_set);
   SUITE_ADD_TEST(suite, test_adt_u16Map_remove_val);
   SUITE_ADD_TEST(suite, test_adt_u16Map_move_large);
#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
   SUITE_ADD_TEST(suite, test_adt_u16Map_new_delete);
   SUITE_ADD_TEST(suite, test_adt_u16Map_largeCapacity);
#endif
   SUITE_ADD_TEST(suite, test_adt_u16Map_errors);

   return suite;
}
