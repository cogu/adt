/*****************************************************************************
* \file      adt_bytearray.h
* \author    Conny Gustafsson
* \date      2015-02-05
* \brief     A mutable byte array
*
* Copyright (c) 2015-2019 Conny Gustafsson
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
#ifndef ADT_BYTE_ARRAY_H
#define ADT_BYTE_ARRAY_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "adt_error.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
//forward declarations
struct adt_bytes_tag;

typedef struct adt_bytearray_tag
{
   uint8_t *pData;
   uint32_t u32CurLen;
   uint32_t u32AllocLen;
   uint32_t u32GrowSize;
} adt_bytearray_t;

//Corrected names
#define ADT_BYTEARRAY_NO_GROWTH 0u  //will malloc exactly the number of bytes it currently needs
#define ADT_BYTEARRAY_DEFAULT_GROW_SIZE ((uint32_t)8192u)
#define ADT_BYTEARRAY_MAX_GROW_SIZE ((uint32_t)32u*1024u*1024u)

//Deprecated names
#define ADT_BYTE_ARRAY_VERSION ADT_BYTEARRAY_NO_GROWTH
#define ADT_BYTE_ARRAY_NO_GROWTH ADT_BYTEARRAY_NO_GROWTH
#define ADT_BYTE_ARRAY_DEFAULT_GROW_SIZE ADT_BYTEARRAY_DEFAULT_GROW_SIZE
#define ADT_BYTE_ARRAY_MAX_GROW_SIZE ADT_BYTEARRAY_MAX_GROW_SIZE


//////////////////////////////////////////////////////////////////////////////
// PUBLIC VARIABLES
//////////////////////////////////////////////////////////////////////////////
void adt_bytearray_create(adt_bytearray_t *self,uint32_t u32GrowSize);
void adt_bytearray_destroy(adt_bytearray_t *self);
adt_bytearray_t *adt_bytearray_new(uint32_t u32GrowSize);
adt_bytearray_t *adt_bytearray_make(const uint8_t *pData, uint32_t u32DataLen, uint32_t u32GrowSize);
adt_bytearray_t *adt_bytearray_make_cstr(const char *cstr, uint32_t u32GrowSize);
adt_bytearray_t *adt_bytearray_clone(const adt_bytearray_t *other, uint32_t u32GrowSize);
void adt_bytearray_delete(adt_bytearray_t *self);
void adt_bytearray_vdelete(void *arg);
void adt_bytearray_setGrowthSize(adt_bytearray_t *self,uint32_t u32GrowSize);
adt_error_t adt_bytearray_reserve(adt_bytearray_t *self, uint32_t u32NewLen);
adt_error_t adt_bytearray_grow(adt_bytearray_t *self, uint32_t u32MinLen);
adt_error_t adt_bytearray_append(adt_bytearray_t *self, const uint8_t *pData, uint32_t u32DataLen);
adt_error_t adt_bytearray_trimLeft(adt_bytearray_t *self, const uint8_t *pSrc);
adt_error_t adt_bytearray_resize(adt_bytearray_t *self, uint32_t u32NewLen);
adt_error_t adt_bytearray_push(adt_bytearray_t *self, uint8_t value);
uint8_t *adt_bytearray_data(const adt_bytearray_t *self);
uint32_t adt_bytearray_length(const adt_bytearray_t *self);
void adt_bytearray_clear(adt_bytearray_t *self);
bool adt_bytearray_equals(const adt_bytearray_t *lhs, const adt_bytearray_t *rhs);
struct adt_bytes_tag* adt_bytearray_bytes(const adt_bytearray_t *self);


//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////


#endif //ADT_BYTE_ARRAY_H
