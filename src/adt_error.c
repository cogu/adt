/*****************************************************************************
* \file      adt_error.c
* \author    Conny Gustafsson
* \date      2026-09-06
* \brief     ADT error code utilities
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_error.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
const char *adt_error_str(adt_error_t error)
{
   switch(error)
   {
   case ADT_NO_ERROR:
      return "No error";
   case ADT_INVALID_ARGUMENT_ERROR:
      return "Invalid argument";
   case ADT_MEM_ERROR:
      return "Out of memory";
   case ADT_INDEX_OUT_OF_BOUNDS_ERROR:
      return "Index out of bounds";
   case ADT_LENGTH_ERROR:
      return "Invalid length";
   case ADT_ARRAY_TOO_LARGE_ERROR:
      return "Array too large";
   case ADT_NOT_IMPLEMENTED_ERROR:
      return "Not implemented";
   case ADT_UNKNOWN_ENCODING_ERROR:
      return "Unknown encoding";
   case ADT_OBJECT_COMPARE_ERROR:
      return "Object compare error";
   case ADT_OVERFLOW_ERROR:
      return "Buffer overflow";
   case ADT_UNDERFLOW_ERROR:
      return "Buffer underflow";
   case ADT_NOT_FOUND_ERROR:
      return "Not found";
   case ADT_ALREADY_EXISTS_ERROR:
      return "Already exists";
   default:
      return "Unknown error";
   }
}
