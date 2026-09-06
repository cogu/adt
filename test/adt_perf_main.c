/*****************************************************************************
* \file      adt_perf_main.c
* \author    Conny Gustafsson
* \date      2026-09-06
* \brief     Performance benchmark runner main
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#include <stdio.h>
#include "perf_adt_hash.h"
#include "perf_adt_ary.h"

#ifndef DICTIONARY_PATH
#define DICTIONARY_PATH "test/enable1.txt"
#endif

int main(int argc, char **argv)
{
   const char *dict_path = (argc > 1) ? argv[1] : DICTIONARY_PATH;

   perf_adt_hash_run(dict_path);
   perf_adt_ary_run(dict_path);

   return 0;
}
