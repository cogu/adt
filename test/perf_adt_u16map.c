/*****************************************************************************
* \file      perf_adt_u16map.c
* \author    Conny Gustafsson
* \date      2026-09-06
* \brief     Performance benchmark for adt_u16Map_t
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "adt_u16Map.h"
#include "perf_adt_u16map.h"

void perf_adt_u16map_run(void)
{
   printf("\n=== adt_u16Map benchmarks ===\n");

   // 1. Sequential Insertion benchmark
   const uint32_t num_seq_items = 20000;
   adt_u16Map_t *map = adt_u16Map_new(num_seq_items, NULL);
   if (map == NULL)
   {
      return;
   }

   clock_t start = clock();
   for (uint32_t i = 0; i < num_seq_items; i++)
   {
      adt_u16Map_insert(map, (uint16_t)i, (void *)(uintptr_t)(i + 1));
   }
   clock_t end = clock();
   double elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_u16Map sequential insert: %f s (items: %u)\n", elapsed_time, num_seq_items);

   // 2. Sequential Lookup benchmark
   start = clock();
   uint32_t found_count = 0;
   for (uint32_t i = 0; i < num_seq_items; i++)
   {
      if (adt_u16Map_find(map, (uint16_t)i) != NULL)
      {
         found_count++;
      }
   }
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_u16Map sequential lookup: %f s (queries: %u, found: %u)\n", elapsed_time, num_seq_items, found_count);

   adt_u16Map_delete(map);

   // 3. Random Insertion benchmark
   const uint32_t num_rand_items = 15000;
   uint16_t *rand_keys = (uint16_t *)malloc(sizeof(uint16_t) * (size_t)num_rand_items);
   if (rand_keys == NULL)
   {
      return;
   }

   uint32_t seed = 424242;
   for (uint32_t i = 0; i < num_rand_items; i++)
   {
      seed = seed * 1664525U + 1013904223U;
      rand_keys[i] = (uint16_t)(seed >> 16);
   }

   map = adt_u16Map_new(num_rand_items, NULL);
   if (map == NULL)
   {
      free(rand_keys);
      return;
   }

   start = clock();
   for (uint32_t i = 0; i < num_rand_items; i++)
   {
      adt_u16Map_insert(map, rand_keys[i], (void *)(uintptr_t)(i + 1));
   }
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   uint32_t unique_count = adt_u16Map_size(map);
   printf("adt_u16Map random insert:     %f s (inserted: %u, stored: %u)\n", elapsed_time, num_rand_items, unique_count);

   // 4. Random Lookup benchmark
   start = clock();
   found_count = 0;
   for (uint32_t i = 0; i < num_rand_items; i++)
   {
      if (adt_u16Map_find(map, rand_keys[i]) != NULL)
      {
         found_count++;
      }
   }
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_u16Map random lookup:     %f s (queries: %u, found: %u)\n", elapsed_time, num_rand_items, found_count);

   // 5. Random Removal benchmark
   start = clock();
   uint32_t removed_count = 0;
   for (uint32_t i = 0; i < num_rand_items / 2; i++)
   {
      adt_u16MapElem_t *elem = adt_u16Map_find(map, rand_keys[i]);
      if (elem != NULL)
      {
         adt_u16Map_remove(map, elem);
         removed_count++;
      }
   }
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_u16Map remove:            %f s (removed: %u, remaining: %u)\n", elapsed_time, removed_count, adt_u16Map_size(map));

   adt_u16Map_delete(map);
   free(rand_keys);
}
