/*****************************************************************************
* \file      adt_bytes.c
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
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <malloc.h>
#include <string.h>
#include "adt_bytes.h"
#include "adt_bytearray.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
adt_error_t adt_bytes_create(adt_bytes_t *self, const uint8_t *dataBuf, uint32_t dataLen)
{
   if (self != 0)
   {
      self->dataLen = dataLen;
      self->dataBuf = (uint8_t*) 0;
      if ( (dataBuf != 0) && (dataLen > 0) )
      {
         self->dataBuf = (uint8_t*) malloc(dataLen);
         if (self->dataBuf == 0)
         {
            return ADT_MEM_ERROR;
         }
         memcpy(self->dataBuf, dataBuf, dataLen);
      }
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

void adt_bytes_destroy(adt_bytes_t *self)
{
   if ( (self != 0) && (self->dataBuf != 0) )
   {
      free(self->dataBuf);
   }
}

adt_bytes_t *adt_bytes_new(const uint8_t *dataBuf, uint32_t dataLen)
{
   adt_bytes_t *self = (adt_bytes_t*) malloc(sizeof(adt_bytes_t));
   if (self != 0)
   {
      adt_error_t result = adt_bytes_create(self, dataBuf, dataLen);
      if (result != ADT_NO_ERROR)
      {
         free(self);
         self = (adt_bytes_t*) 0;
      }
   }
   return self;
}

adt_bytes_t *adt_bytes_new_cstr(const char *cstr)
{
   if (cstr != 0)
   {
      uint32_t dataLen = (uint32_t) strlen(cstr);
      return adt_bytes_new((const uint8_t*) cstr, dataLen);
   }
   return (adt_bytes_t*) 0;
}

adt_bytes_t *adt_bytes_clone(const adt_bytes_t* other)
{
   if (other != 0)
   {
      return adt_bytes_new(other->dataBuf, other->dataLen);
   }
   return (adt_bytes_t*) 0;
}

void adt_bytes_delete(adt_bytes_t *self)
{
   if (self != 0)
   {
      adt_bytes_destroy(self);
      free(self);
   }
}

void adt_bytes_vdelete(void *arg)
{
   adt_bytes_delete((adt_bytes_t*) arg);
}

uint32_t adt_bytes_length(const adt_bytes_t *self)
{
   if (self != 0)
   {
      return self->dataLen;
   }
   return 0;
}

const uint8_t* adt_bytes_data(const adt_bytes_t *self)
{
   if (self != 0)
   {
      return (const uint8_t*) self->dataBuf;
   }
   return (const uint8_t*) 0;
}

struct adt_bytearray_tag *adt_bytes_bytearray(const adt_bytes_t *self, uint32_t u32GrowSize)
{
   if (self != 0)
   {
      return adt_bytearray_make(self->dataBuf, self->dataLen, u32GrowSize);
   }
   return (adt_bytearray_t*) 0;
}

bool adt_bytes_equals(const adt_bytes_t *self, const adt_bytes_t *other)
{
   bool retval = false;
   if ( (self != 0) && (other != 0) && (self->dataLen == other->dataLen) )
   {
      if ( memcmp(self->dataBuf, other->dataBuf, self->dataLen) == 0)
      {
         retval = true;
      }
   }
   return retval;
}


//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////


