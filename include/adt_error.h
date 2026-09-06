/*****************************************************************************
* \file      adt_error.h
* \author    Conny Gustafsson
* \date      2018-12-07
* \brief     ADT error codes
*
* Copyright (c) 2018-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_ERROR_H
#define ADT_ERROR_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define ADT_NO_ERROR                   0
#define ADT_INVALID_ARGUMENT_ERROR     1
#define ADT_MEM_ERROR                  2
#define ADT_INDEX_OUT_OF_BOUNDS_ERROR  3 //Used instead of invalid argument error where applicable
#define ADT_LENGTH_ERROR               4 //Used instead of invalid argument error where applicable
#define ADT_ARRAY_TOO_LARGE_ERROR      5
#define ADT_NOT_IMPLEMENTED_ERROR      6
#define ADT_UNKNOWN_ENCODING_ERROR     7
#define ADT_OBJECT_COMPARE_ERROR       8
#define ADT_OVERFLOW_ERROR             9
#define ADT_UNDERFLOW_ERROR            10
#define ADT_NOT_FOUND_ERROR            11
#define ADT_ALREADY_EXISTS_ERROR       12

typedef int8_t adt_error_t;
//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
const char *adt_error_str(adt_error_t error);

#endif //ADT_ERROR_H
