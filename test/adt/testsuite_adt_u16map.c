#include <string.h>
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "CuTest.h"
#include "adt_u16Map.h"

#define NUM_ELEM 20
#define NUM_ELEM_LARGE 2000

void test_adt_u16Map_insert(CuTest* tc){
   adt_u16MapElem_t elem[10];
   adt_u16Map_t map;
   adt_u16Map_t *pList = &map;
   adt_u16MapElem_t *it;
   adt_u16Map_create(pList,elem,10,0);
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
   it = adt_u16Map_iterInit(pList,0);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,0,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,1,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,2,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,4,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,6,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,7,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,8,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,9,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrEquals(tc,0,it);
}

void test_adt_u16Map_duplicates(CuTest* tc){
   adt_u16MapElem_t elem[10];
   adt_u16Map_t map;
   adt_u16Map_t *pList = &map;
   adt_u16MapElem_t *it;
   adt_u16Map_create(pList,elem,10,0);
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
   it = adt_u16Map_iterInit(pList,0);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,1,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,1,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,4,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,4,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,4,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrEquals(tc,0,it);
}

void test_adt_u16Map_find(CuTest* tc){
   adt_u16MapElem_t elem[10];
   adt_u16Map_t map;
   adt_u16Map_t *pList = &map;
   adt_u16MapElem_t *it;
   adt_u16Map_create(pList,elem,10,0);
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
   CuAssertPtrEquals(tc,0,it);
   it = adt_u16Map_find(pList,1);
   CuAssertPtrNotNull(tc,it);
   it = adt_u16Map_iterInit(pList,it);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,1,it->key);
   CuAssertPtrEquals(tc,(void*)9,it->val);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,1,it->key);
   CuAssertPtrEquals(tc,(void*)10,it->val);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   CuAssertPtrEquals(tc,(void*)2,it->val);

   it = adt_u16Map_find(pList,5);
   CuAssertPtrNotNull(tc,it);
   it = adt_u16Map_iterInit(pList,it);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   CuAssertPtrEquals(tc,(void*)3,it->val);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   CuAssertPtrEquals(tc,(void*)6,it->val);
   it = adt_u16Map_iterNext(pList);
   CuAssertPtrEquals(tc,0,it);
}

void test_adt_u16Map_findExact(CuTest* tc){
   adt_u16MapElem_t elem[10];
   adt_u16Map_t map;
   adt_u16Map_t *pList = &map;
   adt_u16MapElem_t *it;
   adt_u16Map_create(pList,elem,10,0);
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
   it = adt_u16Map_findExact(pList,1,(void*) 1);
   CuAssertPtrEquals(tc,0,0);
   it = adt_u16Map_findExact(pList,4,(void*) 7);
   CuAssertPtrEquals(tc,0,0);
   //last item of key 4
   it = adt_u16Map_findExact(pList,4,(void*) 5);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,4,it->key);
   CuAssertPtrEquals(tc,(void*) 5,it->val);
   //first item of key 5
   it = adt_u16Map_findExact(pList,5,(void*) 3);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,5,it->key);
   CuAssertPtrEquals(tc,(void*) 3,it->val);
   //mit item of key 3
   it = adt_u16Map_findExact(pList,3,(void*) 7);
   CuAssertPtrNotNull(tc,it);
   CuAssertIntEquals(tc,3,it->key);
   CuAssertPtrEquals(tc,(void*) 7,it->val);
}

void test_adt_u16Map_sort(CuTest* tc){
   adt_u16MapElem_t data[NUM_ELEM];
   adt_u16Map_t map;

   adt_u16Map_create(&map,&data[0],NUM_ELEM,0);
   adt_u16Map_insert(&map,5,0);
   adt_u16Map_insert(&map,87,0);
   adt_u16Map_insert(&map,1245,0);
   adt_u16Map_insert(&map,2,0);
   adt_u16Map_insert(&map,11,0);
   adt_u16Map_insert(&map,1,0);
   adt_u16Map_insert(&map,600,0);
   adt_u16MapElem_t *elem;
   uint16_t i = 0;
   adt_u16Map_iterInit(&map,NULL);
   uint16_t expected[] = {1,2,5,11,87,600,1245};
   while((elem = adt_u16Map_iterNext(&map)) != 0){
      CuAssertPtrNotNull(tc,elem);
      CuAssertIntEquals(tc,expected[i++],elem->key);
   }
}

void test_adt_u16Map_find_rand_set(CuTest* tc){
   adt_u16MapElem_t *data;
   adt_u16Map_t map;
   adt_u16MapElem_t *elem;
   long i;
   srand (time(NULL));
   data = malloc(sizeof(adt_u16MapElem_t)*NUM_ELEM_LARGE);
   CuAssertPtrNotNull(tc,data);

   adt_u16Map_create(&map,data,NUM_ELEM_LARGE,0);
   for(i=0;i<NUM_ELEM_LARGE-1;i++){
      uint16_t key = (uint16_t)(rand() % 100 + 1);
      adt_u16Map_insert(&map,key,(void*) i);
   }

   adt_u16Map_insert(&map,82,0);
   elem = adt_u16Map_find(&map,82);
   CuAssertPtrNotNull(tc,elem);
   CuAssertIntEquals(tc,82,elem->key);
   elem = adt_u16Map_find(&map,101);
   CuAssertPtrEquals(tc,NULL,elem);
}

void test_adt_u16Map_move(CuTest* tc){
   adt_u16MapElem_t elem1[10];
   adt_u16MapElem_t elem2[10];
   adt_u16Map_t map1;
   adt_u16Map_t map2;
   adt_u16Map_t *pList1 = &map1;
   adt_u16Map_t *pList2 = &map2;

   adt_u16Map_create(pList1,elem1,10,0);
   adt_u16Map_create(pList2,elem2,10,0);
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
   adt_u16Map_moveElem(pList2,pList1,4);
   CuAssertIntEquals(tc,3,adt_u16Map_size(pList2));
   CuAssertIntEquals(tc,7,adt_u16Map_size(pList1));
}

CuSuite* testsuite_adt_u16Map(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_u16Map_insert);
   SUITE_ADD_TEST(suite, test_adt_u16Map_duplicates);
   SUITE_ADD_TEST(suite, test_adt_u16Map_find);
   SUITE_ADD_TEST(suite, test_adt_u16Map_findExact);
   SUITE_ADD_TEST(suite, test_adt_u16Map_move);
   SUITE_ADD_TEST(suite, test_adt_u16Map_find_rand_set);

   return suite;
}
