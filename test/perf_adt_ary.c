/*****************************************************************************
* \file      perf_adt_ary.c
* \author    Conny Gustafsson
* \date      2026-09-06
* \brief     Performance benchmark for adt_ary_t
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "adt_ary.h"
#include "adt_str.h"
#include "perf_adt_ary.h"

static FILE *open_dictionary(const char *path)
{
   FILE *fh = fopen(path, "r");
   if (fh == NULL)
   {
      fh = fopen("test/enable1.txt", "r");
   }
   if (fh == NULL)
   {
      fh = fopen("../test/enable1.txt", "r");
   }
   if (fh == NULL)
   {
      fh = fopen("test/3esl.txt", "r");
   }
   if (fh == NULL)
   {
      fh = fopen("../test/3esl.txt", "r");
   }
   return fh;
}

void perf_adt_ary_run(const char *dict_path)
{
   printf("\n=== adt_ary benchmarks ===\n");

   // 1. Push benchmark
   const int num_push_items = 50000;
   int *push_values = (int *)malloc(sizeof(int) * num_push_items);
   if (push_values == NULL)
   {
      fprintf(stderr, "Error: Memory allocation failed for push_values\n");
      return;
   }
   for (int i = 0; i < num_push_items; i++)
   {
      push_values[i] = i;
   }

   adt_ary_t *ary = adt_ary_new(NULL);
   if (ary == NULL)
   {
      free(push_values);
      return;
   }

   clock_t start = clock();
   for (int i = 0; i < num_push_items; i++)
   {
      adt_ary_push(ary, &push_values[i]);
   }
   clock_t end = clock();
   double elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_ary push time:             %f s (items: %d)\n", elapsed_time, num_push_items);
   adt_ary_delete(ary);
   free(push_values);

   // 2. Integer Sort (random)
   const int num_sort_items = 50000;
   int32_t *random_values = (int32_t *)malloc(sizeof(int32_t) * num_sort_items);
   if (random_values == NULL)
   {
      return;
   }
   uint32_t seed = 42;
   for (int i = 0; i < num_sort_items; i++)
   {
      seed = seed * 1103515245 + 12345;
      random_values[i] = (int32_t)(seed % 1000000);
   }

   ary = adt_ary_new(NULL);
   for (int i = 0; i < num_sort_items; i++)
   {
      adt_ary_push(ary, &random_values[i]);
   }

   start = clock();
   adt_ary_sort(ary, adt_i32_vlt, false);
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_ary sort random i32:       %f s (items: %d)\n", elapsed_time, num_sort_items);

   // 3. Integer Sort (already sorted)
   start = clock();
   adt_ary_sort(ary, adt_i32_vlt, false);
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_ary sort presorted i32:    %f s (items: %d)\n", elapsed_time, num_sort_items);

   // 4. Integer Sort (reverse)
   start = clock();
   adt_ary_sort(ary, adt_i32_vlt, true);
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_ary sort reversed i32:     %f s (items: %d)\n", elapsed_time, num_sort_items);

   adt_ary_delete(ary);
   free(random_values);

   // 5. String Sort (from dictionary)
   FILE *fh = open_dictionary(dict_path);
   if (fh != NULL)
   {
      ary = adt_ary_new(adt_str_vdelete);
      char line[256];
      int word_count = 0;
      const int max_words = 50000;
      while ((word_count < max_words) && (fgets(line, sizeof(line), fh) != NULL))
      {
         size_t len = strlen(line);
         while ((len > 0) && ((line[len - 1] == '\n') || (line[len - 1] == '\r')))
         {
            line[--len] = '\0';
         }
         if (len > 0)
         {
            adt_str_t *str = adt_str_new_cstr(line);
            if (str != NULL)
            {
               adt_ary_push(ary, str);
               word_count++;
            }
         }
      }
      fclose(fh);

      start = clock();
      adt_ary_sort(ary, adt_str_vlt, false);
      end = clock();
      elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
      printf("adt_ary sort strings:          %f s (words: %d)\n", elapsed_time, word_count);

      adt_ary_delete(ary);
   }
}
