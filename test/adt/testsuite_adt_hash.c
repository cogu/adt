/*****************************************************************************
* \file      testsuite_adt_hash.c
* \author    Conny Gustafsson
* \date      2013-08-06
* \brief     Unit tests for adt_hash_t
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "test_common.h"
#include "adt_hash.h"
#include "adt_ary.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

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
   uint32_t i = 0;
   do{
      pVal = adt_hash_iter_next(pHash,&pKey);
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
         CuAssertPtrEquals(tc, NULL, pVal);
         break;
      }
   }while(pVal);
   CuAssertIntEquals(tc,4,adt_hash_length(pHash));
   adt_hash_delete(pHash);
}

void test_adt_hash_keys(CuTest* tc)
{
   int val1 = 1;
   int val2 = 2;
   int val3 = 3;
   int val4 = 4;
   adt_hash_t *pHash = adt_hash_new(NULL);
   adt_ary_t *pKeys = adt_ary_new(NULL);

   CuAssertPtrNotNull(tc, pHash);
   CuAssertPtrNotNull(tc, pKeys);

   adt_hash_set(pHash,"The",&val1);
   adt_hash_set(pHash,"quick",&val2);
   adt_hash_set(pHash,"brown",&val3);
   adt_hash_set(pHash,"fox",&val4);

   CuAssertIntEquals(tc, 0, adt_ary_length(pKeys));
   CuAssertIntEquals(tc, 4, adt_hash_keys(pHash, pKeys));
   CuAssertIntEquals(tc, 4, adt_ary_length(pKeys));
   CuAssertStrEquals(tc, "The", (const char*) adt_ary_value(pKeys, 0));
   CuAssertStrEquals(tc, "quick", (const char*) adt_ary_value(pKeys, 1));
   CuAssertStrEquals(tc, "brown", (const char*) adt_ary_value(pKeys, 2));
   CuAssertStrEquals(tc, "fox", (const char*) adt_ary_value(pKeys, 3));

   adt_hash_delete(pHash);
   adt_ary_delete(pKeys);

}

void test_adt_hash_values(CuTest* tc)
{
   int val1 = 1;
   int val2 = 2;
   int val3 = 3;
   int val4 = 4;
   adt_hash_t *pHash = adt_hash_new(NULL);
   adt_ary_t *pValues = adt_ary_new(NULL);

   CuAssertPtrNotNull(tc, pHash);
   CuAssertPtrNotNull(tc, pValues);

   adt_hash_set(pHash,"The",&val1);
   adt_hash_set(pHash,"quick",&val2);
   adt_hash_set(pHash,"brown",&val3);
   adt_hash_set(pHash,"fox",&val4);

   CuAssertIntEquals(tc, 0, adt_ary_length(pValues));
   CuAssertIntEquals(tc, 4, adt_hash_values(pHash, pValues));
   CuAssertIntEquals(tc, 4, adt_ary_length(pValues));

   CuAssertPtrEquals(tc, &val1, adt_ary_value(pValues, 0));
   CuAssertPtrEquals(tc, &val2, adt_ary_value(pValues, 1));
   CuAssertPtrEquals(tc, &val3, adt_ary_value(pValues, 2));
   CuAssertPtrEquals(tc, &val4, adt_ary_value(pValues, 3));

   adt_hash_delete(pHash);
   adt_ary_delete(pValues);

}

void test_adt_hash_remove(CuTest* tc)
{
   int *val1 = malloc(sizeof(int));
   int *val2 = malloc(sizeof(int));
   int *val3 = malloc(sizeof(int));
   adt_hash_t *pHash = adt_hash_new(vfree);

   CuAssertPtrNotNull(tc, pHash);

   adt_hash_set(pHash,"V1",val1);
   adt_hash_set(pHash,"V2",val2);
   adt_hash_set(pHash,"V3",val3);

   CuAssertIntEquals(tc, 3, adt_hash_length(pHash));
   CuAssertPtrEquals(tc, val2, adt_hash_remove(pHash, "V2"));
   CuAssertPtrEquals(tc, val3, adt_hash_remove(pHash, "V3"));
   CuAssertPtrEquals(tc, val1, adt_hash_remove(pHash, "V1"));
   CuAssertIntEquals(tc, 0, adt_hash_length(pHash));

   adt_hash_delete(pHash);
   free(val1);
   free(val2);
   free(val3);

}

void test_adt_hash_value(CuTest* tc)
{
   int *val1 = malloc(sizeof(int));
   int *val2 = malloc(sizeof(int));
   int *val3 = malloc(sizeof(int));

   adt_hash_t *pHash = adt_hash_new(vfree);

   CuAssertPtrNotNull(tc, pHash);

   adt_hash_set(pHash,"V1",val1);
   adt_hash_set(pHash,"V2",val2);
   adt_hash_set(pHash,"V3",val3);

   CuAssertIntEquals(tc, 3, adt_hash_length(pHash));
   CuAssertPtrEquals(tc, val1, adt_hash_value(pHash, "V1"));
   CuAssertPtrEquals(tc, val2, adt_hash_value(pHash, "V2"));
   CuAssertPtrEquals(tc, val3, adt_hash_value(pHash, "V3"));

   adt_hash_delete(pHash);
}

void test_adt_hash_exists(CuTest* tc)
{
   adt_hash_t *pHash = adt_hash_new(NULL);
   int val = 123;
   CuAssertTrue(tc, !adt_hash_exists(pHash, "nonexistent"));

   adt_hash_set(pHash, "key1", &val);
   CuAssertTrue(tc, adt_hash_exists(pHash, "key1"));
   CuAssertTrue(tc, !adt_hash_exists(pHash, "key2"));

   adt_hash_delete(pHash);
}

void test_adt_hash_set_overwrite_destructor(CuTest* tc)
{
   adt_hash_t *pHash = adt_hash_new(vfree);
   CuAssertPtrNotNull(tc, pHash);

   adt_hash_set(pHash, "my_key", STRDUP("first_value"));
   CuAssertIntEquals(tc, 1, adt_hash_length(pHash));
   CuAssertStrEquals(tc, "first_value", (char*) adt_hash_value(pHash, "my_key"));

   // Overwriting should trigger vfree on "first_value"
   adt_hash_set(pHash, "my_key", STRDUP("second_value"));
   CuAssertIntEquals(tc, 1, adt_hash_length(pHash));
   CuAssertStrEquals(tc, "second_value", (char*) adt_hash_value(pHash, "my_key"));

   adt_hash_delete(pHash);
}

void test_adt_hash_multi_level_split_and_lookup(CuTest* tc)
{
   char key_buf[32];
   int values[200];
   bool seen[200];
   adt_hash_t *pHash = adt_hash_new(NULL);
   CuAssertPtrNotNull(tc, pHash);

   for (int i = 0; i < 200; i++)
   {
      values[i] = i * 10;
      seen[i] = false;
      snprintf(key_buf, sizeof(key_buf), "key_%03d", i);
      adt_hash_set(pHash, key_buf, &values[i]);
   }

   CuAssertIntEquals(tc, 200, adt_hash_length(pHash));

   // Verify all keys exist and return correct values
   for (int i = 0; i < 200; i++)
   {
      snprintf(key_buf, sizeof(key_buf), "key_%03d", i);
      CuAssertTrue(tc, adt_hash_exists(pHash, key_buf));
      int *val = (int*) adt_hash_value(pHash, key_buf);
      CuAssertPtrNotNull(tc, val);
      CuAssertIntEquals(tc, i * 10, *val);
   }

   // Verify iteration visits all 200 elements exactly once
   int iter_count = 0;
   const char *pKey = NULL;
   adt_hash_iter_init(pHash);
   while (adt_hash_iter_next(pHash, &pKey) != NULL)
   {
      CuAssertPtrNotNull(tc, pKey);
      if (strncmp(pKey, "key_", 4) == 0)
      {
         long idx = strtol(pKey + 4, NULL, 10);
         if (idx >= 0 && idx < 200)
         {
            CuAssertTrue(tc, !seen[idx]);
            seen[idx] = true;
         }
      }
      iter_count++;
   }
   CuAssertIntEquals(tc, 200, iter_count);
   for (int i = 0; i < 200; i++)
   {
      CuAssertTrue(tc, seen[i]);
   }

   // Overwrite 50 elements
   int new_val = 9999;
   for (int i = 0; i < 50; i++)
   {
      snprintf(key_buf, sizeof(key_buf), "key_%03d", i);
      adt_hash_set(pHash, key_buf, &new_val);
   }
   CuAssertIntEquals(tc, 200, adt_hash_length(pHash));
   for (int i = 0; i < 50; i++)
   {
      snprintf(key_buf, sizeof(key_buf), "key_%03d", i);
      CuAssertIntEquals(tc, 9999, *(int*)adt_hash_value(pHash, key_buf));
   }

   // Remove 50 elements
   for (int i = 0; i < 50; i++)
   {
      snprintf(key_buf, sizeof(key_buf), "key_%03d", i);
      void *removed = adt_hash_remove(pHash, key_buf);
      CuAssertPtrEquals(tc, &new_val, removed);
      CuAssertTrue(tc, !adt_hash_exists(pHash, key_buf));
   }
   CuAssertIntEquals(tc, 150, adt_hash_length(pHash));

   // Remaining 150 should still exist
   for (int i = 50; i < 200; i++)
   {
      snprintf(key_buf, sizeof(key_buf), "key_%03d", i);
      CuAssertTrue(tc, adt_hash_exists(pHash, key_buf));
      CuAssertIntEquals(tc, i * 10, *(int*)adt_hash_value(pHash, key_buf));
   }

   adt_hash_delete(pHash);
}

void test_adt_hash_empty_table_iter(CuTest* tc)
{
   adt_hash_t hash;
   adt_hash_create(&hash, NULL);
   CuAssertIntEquals(tc, 0, adt_hash_length(&hash));

   const char *pKey = NULL;
   adt_hash_iter_init(&hash);
   void **pVal = adt_hash_iter_next(&hash, &pKey);
   CuAssertPtrEquals(tc, NULL, pVal);
   CuAssertPtrEquals(tc, NULL, (void*)pKey);

   adt_hash_destroy(&hash);
}

void test_adt_hash_clear(CuTest* tc)
{
   adt_hash_t *pHash = adt_hash_new(vfree);
   adt_hash_set(pHash, "k1", STRDUP("v1"));
   adt_hash_set(pHash, "k2", STRDUP("v2"));
   adt_hash_set(pHash, "k3", STRDUP("v3"));
   CuAssertIntEquals(tc, 3, adt_hash_length(pHash));
   CuAssertTrue(tc, !adt_hash_is_empty(pHash));

   adt_hash_clear(pHash);
   CuAssertIntEquals(tc, 0, adt_hash_length(pHash));
   CuAssertTrue(tc, adt_hash_is_empty(pHash));
   CuAssertTrue(tc, !adt_hash_exists(pHash, "k1"));

   // Can insert again after clear
   adt_hash_set(pHash, "k4", STRDUP("v4"));
   CuAssertIntEquals(tc, 1, adt_hash_length(pHash));
   CuAssertStrEquals(tc, "v4", (char*) adt_hash_value(pHash, "k4"));

   adt_hash_delete(pHash);
}

void test_adt_hash_vdelete(CuTest* tc)
{
   adt_hash_t *pHash = adt_hash_new(vfree);
   adt_hash_set(pHash, "k1", STRDUP("v1"));
   adt_hash_vdelete(pHash);
   CuAssertTrue(tc, true);
}

void test_adt_hash_is_empty(CuTest* tc)
{
   CuAssertTrue(tc, adt_hash_is_empty(NULL));
   adt_hash_t *pHash = adt_hash_new(NULL);
   CuAssertTrue(tc, adt_hash_is_empty(pHash));
   int val = 1;
   adt_hash_set(pHash, "k", &val);
   CuAssertTrue(tc, !adt_hash_is_empty(pHash));
   adt_hash_remove(pHash, "k");
   CuAssertTrue(tc, adt_hash_is_empty(pHash));
   adt_hash_delete(pHash);
}

void test_adt_hash_erase(CuTest* tc)
{
   adt_hash_t *pHash = adt_hash_new(vfree);
   adt_hash_set(pHash, "k1", STRDUP("v1"));
   adt_hash_set(pHash, "k2", STRDUP("v2"));

   CuAssertIntEquals(tc, 2, adt_hash_length(pHash));
   CuAssertTrue(tc, adt_hash_erase(pHash, "k1"));
   CuAssertIntEquals(tc, 1, adt_hash_length(pHash));
   CuAssertTrue(tc, !adt_hash_exists(pHash, "k1"));
   CuAssertTrue(tc, adt_hash_exists(pHash, "k2"));

   // Erasing non-existent key returns false
   CuAssertTrue(tc, !adt_hash_erase(pHash, "k1"));
   CuAssertTrue(tc, !adt_hash_erase(pHash, "nonexistent"));

   adt_hash_delete(pHash);
}

typedef struct {
   int count;
   int sum;
} foreach_ctx_t;

static void foreach_callback(const char *key, void *val, void *arg)
{
   (void) key;
   foreach_ctx_t *ctx = (foreach_ctx_t*) arg;
   ctx->count++;
   ctx->sum += *(int*) val;
}

void test_adt_hash_foreach(CuTest* tc)
{
   adt_hash_t *pHash = adt_hash_new(NULL);
   int v1 = 10;
   int v2 = 20;
   int v3 = 30;
   adt_hash_set(pHash, "k1", &v1);
   adt_hash_set(pHash, "k2", &v2);
   adt_hash_set(pHash, "k3", &v3);

   const adt_hash_t *const_hash = pHash;
   foreach_ctx_t ctx = {0, 0};
   adt_hash_foreach(const_hash, foreach_callback, &ctx);
   CuAssertIntEquals(tc, 3, ctx.count);
   CuAssertIntEquals(tc, 60, ctx.sum);

   adt_hash_delete(pHash);
}

void test_adt_hash_insert(CuTest* tc)
{
   adt_hash_t *pHash = adt_hash_new(vfree);
   CuAssertPtrNotNull(tc, pHash);

   // Insert when absent succeeds
   CuAssertTrue(tc, adt_hash_insert(pHash, "key1", STRDUP("val1")));
   CuAssertIntEquals(tc, 1, adt_hash_length(pHash));
   CuAssertStrEquals(tc, "val1", (char*) adt_hash_value(pHash, "key1"));

   // Insert when present fails and does not overwrite
   char *dup_val = STRDUP("val2");
   CuAssertTrue(tc, !adt_hash_insert(pHash, "key1", dup_val));
   CuAssertIntEquals(tc, 1, adt_hash_length(pHash));
   CuAssertStrEquals(tc, "val1", (char*) adt_hash_value(pHash, "key1"));
   free(dup_val);

   // Set overwrites
   adt_hash_set(pHash, "key1", STRDUP("val3"));
   CuAssertIntEquals(tc, 1, adt_hash_length(pHash));
   CuAssertStrEquals(tc, "val3", (char*) adt_hash_value(pHash, "key1"));

   adt_hash_delete(pHash);
}

void test_adt_hash_keys_values_destructor_check(CuTest* tc)
{
   int val1 = 1;
   int val2 = 2;
   adt_hash_t *pHash = adt_hash_new(NULL);
   adt_ary_t *pArrDestructor = adt_ary_new(vfree);
   adt_ary_t *pArrNoDestructor = adt_ary_new(NULL);

   CuAssertPtrNotNull(tc, pHash);
   CuAssertPtrNotNull(tc, pArrDestructor);
   CuAssertPtrNotNull(tc, pArrNoDestructor);

   adt_hash_set(pHash, "first", &val1);
   adt_hash_set(pHash, "second", &val2);

   // Reject arrays with enabled destructor
   CuAssertIntEquals(tc, -1, adt_hash_keys(pHash, pArrDestructor));
   CuAssertIntEquals(tc, 0, adt_ary_length(pArrDestructor));
   CuAssertIntEquals(tc, -1, adt_hash_values(pHash, pArrDestructor));
   CuAssertIntEquals(tc, 0, adt_ary_length(pArrDestructor));

   // Reject NULL arguments
   CuAssertIntEquals(tc, -1, adt_hash_keys(NULL, pArrNoDestructor));
   CuAssertIntEquals(tc, -1, adt_hash_keys(pHash, NULL));
   CuAssertIntEquals(tc, -1, adt_hash_values(NULL, pArrNoDestructor));
   CuAssertIntEquals(tc, -1, adt_hash_values(pHash, NULL));

   // Succeed when destructor is disabled via adt_ary_destructor_enable
   adt_ary_destructor_enable(pArrDestructor, false);
   CuAssertIntEquals(tc, 2, adt_hash_keys(pHash, pArrDestructor));
   CuAssertIntEquals(tc, 2, adt_ary_length(pArrDestructor));
   CuAssertIntEquals(tc, 2, adt_hash_values(pHash, pArrDestructor));
   CuAssertIntEquals(tc, 2, adt_ary_length(pArrDestructor));

   adt_ary_delete(pArrDestructor);
   adt_ary_delete(pArrNoDestructor);
   adt_hash_delete(pHash);
}

CuSuite* testsuite_adt_hash(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_adt_hash_constructor);
   SUITE_ADD_TEST(suite, test_adt_hash_iterator);
   SUITE_ADD_TEST(suite, test_adt_hash_keys);
   SUITE_ADD_TEST(suite, test_adt_hash_values);
   SUITE_ADD_TEST(suite, test_adt_hash_remove);
   SUITE_ADD_TEST(suite, test_adt_hash_value);
   SUITE_ADD_TEST(suite, test_adt_hash_exists);
   SUITE_ADD_TEST(suite, test_adt_hash_set_overwrite_destructor);
   SUITE_ADD_TEST(suite, test_adt_hash_multi_level_split_and_lookup);
   SUITE_ADD_TEST(suite, test_adt_hash_empty_table_iter);
   SUITE_ADD_TEST(suite, test_adt_hash_clear);
   SUITE_ADD_TEST(suite, test_adt_hash_vdelete);
   SUITE_ADD_TEST(suite, test_adt_hash_is_empty);
   SUITE_ADD_TEST(suite, test_adt_hash_erase);
   SUITE_ADD_TEST(suite, test_adt_hash_foreach);
   SUITE_ADD_TEST(suite, test_adt_hash_insert);
   SUITE_ADD_TEST(suite, test_adt_hash_keys_values_destructor_check);
   return suite;
}
