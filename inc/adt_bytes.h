/*****************************************************************************
* \file      adt_bytes.h
* \author    Conny Gustafsson
* \date      2019-08-13
* \brief     An immutable byte array
*
* Copyright (c) 2019 Conny Gustafsson
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
#ifndef ADT_BYTES_H
#define ADT_BYTES_H

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
struct adt_bytearray_tag;


typedef struct adt_bytes_tag
{
   uint32_t dataLen;
   uint8_t *dataBuf;
} adt_bytes_t;

#define ADT_BYTES_VERSION 0.1.0

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
adt_error_t adt_bytes_create(adt_bytes_t *self, const uint8_t *dataBuf, uint32_t dataLen);
void adt_bytes_destroy(adt_bytes_t *self);
adt_bytes_t *adt_bytes_new(const uint8_t *dataBuf, uint32_t dataLen);
adt_bytes_t *adt_bytes_new_cstr(const char *cstr);
adt_bytes_t *adt_bytes_clone(const adt_bytes_t* other);
void adt_bytes_delete(adt_bytes_t *self);
void adt_bytes_vdelete(void *arg);

uint32_t adt_bytes_length(const adt_bytes_t *self);
const uint8_t* adt_bytes_data(const adt_bytes_t *self);
struct adt_bytearray_tag *adt_bytes_bytearray(const adt_bytes_t *self, uint32_t u32GrowSize);
bool adt_bytes_equals(const adt_bytes_t *self, const adt_bytes_t *other);

#endif //ADT_BYTES_H
