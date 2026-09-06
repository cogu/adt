/*****************************************************************************
* \file      perf_adt_set.c
* \author    Conny Gustafsson
* \date      2026-09-06
* \brief     Performance benchmark for adt_u32Set_t
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "adt_set.h"
#include "perf_adt_set.h"

void perf_adt_set_run(void)
{
   printf("\n=== adt_u32Set benchmarks ===\n");

   // 1. Sequential Insertion benchmark
   const int num_seq_items = 50000;
   adt_u32Set_t set;
   adt_u32Set_create(&set);

   clock_t start = clock();
   for (int i = 0; i < num_seq_items; i++)
   {
      adt_u32Set_insert(&set, (uint32_t)i);
   }
   clock_t end = clock();
   double elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_u32Set sequential insert: %f s (items: %d)\n", elapsed_time, num_seq_items);

   // 2. Sequential Lookup benchmark
   start = clock();
   int found_count = 0;
   for (int i = 0; i < num_seq_items; i++)
   {
      if (adt_u32Set_contains(&set, (uint32_t)i))
      {
         found_count++;
      }
   }
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_u32Set sequential lookup: %f s (queries: %d, found: %d)\n", elapsed_time, num_seq_items, found_count);

   adt_u32Set_destroy(&set);

   // 3. Random Insertion benchmark
   const int num_rand_items = 20000;
   uint32_t *rand_values = (uint32_t *)malloc(sizeof(uint32_t) * (size_t)num_rand_items);
   if (rand_values == NULL)
   {
      return;
   }

   uint32_t seed = 12345;
   for (int i = 0; i < num_rand_items; i++)
   {
      seed = seed * 1103515245 + 12345;
      rand_values[i] = seed % 1000000;
   }

   adt_u32Set_create(&set);

   start = clock();
   for (int i = 0; i < num_rand_items; i++)
   {
      adt_u32Set_insert(&set, rand_values[i]);
   }
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   int unique_count = adt_u32Set_length(&set);
   printf("adt_u32Set random insert:     %f s (inserted: %d, unique: %d)\n", elapsed_time, num_rand_items, unique_count);

   // 4. Random Lookup benchmark
   start = clock();
   found_count = 0;
   for (int i = 0; i < num_rand_items; i++)
   {
      if (adt_u32Set_contains(&set, rand_values[i]))
      {
         found_count++;
      }
   }
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_u32Set random lookup:     %f s (queries: %d, found: %d)\n", elapsed_time, num_rand_items, found_count);

   // 5. Random Removal benchmark
   start = clock();
   int removed_count = 0;
   for (int i = 0; i < num_rand_items / 2; i++)
   {
      if (adt_u32Set_remove(&set, rand_values[i]))
      {
         removed_count++;
      }
   }
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_u32Set remove:            %f s (removed: %d, remaining: %d)\n", elapsed_time, removed_count, adt_u32Set_length(&set));

   adt_u32Set_destroy(&set);
   free(rand_values);
}
