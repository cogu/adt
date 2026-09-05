/*****************************************************************************
* \file      adt_bytes.c
* \author    Conny Gustafsson
* \date      2019-08-13
* \brief     An immutable byte array
*
* Copyright (c) 2019-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <malloc.h>
#include <string.h>
#include <stddef.h>
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
   if (self != NULL)
   {
      self->dataLen = dataLen;
      self->dataBuf = NULL;
      if ( (dataBuf != NULL) && (dataLen > 0) )
      {
         self->dataBuf = (uint8_t*) malloc(dataLen);
         if (self->dataBuf == NULL)
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
   if ( (self != NULL) && (self->dataBuf != NULL) )
   {
      free(self->dataBuf);
   }
}

adt_bytes_t *adt_bytes_new(const uint8_t *dataBuf, uint32_t dataLen)
{
   adt_bytes_t *self = (adt_bytes_t*) malloc(sizeof(adt_bytes_t));
   if (self != NULL)
   {
      adt_error_t result = adt_bytes_create(self, dataBuf, dataLen);
      if (result != ADT_NO_ERROR)
      {
         free(self);
         self = NULL;
      }
   }
   return self;
}

adt_bytes_t *adt_bytes_new_cstr(const char *cstr)
{
   if (cstr != NULL)
   {
      uint32_t dataLen = (uint32_t) strlen(cstr);
      return adt_bytes_new((const uint8_t*) cstr, dataLen);
   }
   return NULL;
}

adt_bytes_t *adt_bytes_clone(const adt_bytes_t* other)
{
   if (other != NULL)
   {
      return adt_bytes_new(other->dataBuf, other->dataLen);
   }
   return NULL;
}

void adt_bytes_delete(adt_bytes_t *self)
{
   if (self != NULL)
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
   if (self != NULL)
   {
      return self->dataLen;
   }
   return 0;
}

const uint8_t* adt_bytes_constData(const adt_bytes_t *self)
{
   if (self != NULL)
   {
      return (const uint8_t*) self->dataBuf;
   }
   return NULL;
}

struct adt_bytearray_tag *adt_bytes_bytearray(const adt_bytes_t *self, uint32_t u32GrowSize)
{
   if (self != NULL)
   {
      return adt_bytearray_make(self->dataBuf, self->dataLen, u32GrowSize);
   }
   return NULL;
}

bool adt_bytes_equals(const adt_bytes_t *self, const adt_bytes_t *other)
{
   bool retval = false;
   if ( (self != NULL) && (other != NULL) && (self->dataLen == other->dataLen) )
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


