/*****************************************************************************
* \file      perf_adt_hash.c
* \author    Conny Gustafsson
* \date      2019-09-02
* \brief     Performance benchmark for adt_hash_t
*
* Copyright (c) 2019-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "adt_hash.h"

#ifndef DICTIONARY_PATH
#define DICTIONARY_PATH "test/enable1.txt"
#endif

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

int main(int argc, char **argv)
{
   const char *dict_path = (argc > 1) ? argv[1] : DICTIONARY_PATH;
   FILE *fh = open_dictionary(dict_path);
   if (fh == NULL)
   {
      fprintf(stderr, "Error: Could not open dictionary file '%s'\n", dict_path);
      return 1;
   }

   adt_hash_t *pHash = adt_hash_new(NULL);
   if (pHash == NULL)
   {
      fprintf(stderr, "Error: Failed to create adt_hash instance\n");
      fclose(fh);
      return 1;
   }

   char line[256];
   int value = 42;
   int items = 0;
   int dup = 0;

   // 1. Insertion benchmark
   clock_t start = clock();
   while (fgets(line, sizeof(line), fh) != NULL)
   {
      // Strip trailing newline/carriage return
      size_t len = strlen(line);
      while ((len > 0) && ((line[len - 1] == '\n') || (line[len - 1] == '\r')))
      {
         line[--len] = '\0';
      }
      if (len > 0)
      {
         items++;
         if (adt_hash_exists(pHash, line))
         {
            dup++;
         }
         adt_hash_set(pHash, line, &value);
      }
   }
   fclose(fh);
   clock_t end = clock();
   double elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_hash insertion time: %f s (items: %d, duplicates: %d)\n", elapsed_time, items, dup);

   // 2. Lookup benchmark
   fh = open_dictionary(dict_path);
   if (fh == NULL)
   {
      fprintf(stderr, "Error: Could not re-open dictionary file '%s'\n", dict_path);
      adt_hash_delete(pHash);
      return 1;
   }

   int count = 0;
   start = clock();
   while (fgets(line, sizeof(line), fh) != NULL)
   {
      size_t len = strlen(line);
      while ((len > 0) && ((line[len - 1] == '\n') || (line[len - 1] == '\r')))
      {
         line[--len] = '\0';
      }
      if (len > 0)
      {
         if (adt_hash_get(pHash, line) != NULL)
         {
            count++;
         }
      }
   }
   fclose(fh);
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_hash lookup time:    %f s (count: %d)\n", elapsed_time, count);

   // 3. Iteration benchmark
   int iter_count = 0;
   const char *iter_key = NULL;
   start = clock();
   adt_hash_iter_init(pHash);
   while (adt_hash_iter_next(pHash, &iter_key) != NULL)
   {
      iter_count++;
   }
   end = clock();
   elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("adt_hash iteration time: %f s (count: %d)\n", elapsed_time, iter_count);

   adt_hash_delete(pHash);
   return 0;
}
