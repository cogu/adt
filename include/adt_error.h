/*****************************************************************************
* \file      adt_error.h
* \author    Conny Gustafsson
* \date      2018-12-07
* \brief     ADT error codes
*
* Copyright (c) 2018 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
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

typedef int8_t adt_error_t;
//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

#endif //ADT_ERROR_H
