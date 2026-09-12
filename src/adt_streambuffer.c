/*****************************************************************************
* \file      adt_streambuffer.c
* \author    Conny Gustafsson
* \date      2026-09-10
* \brief     Rolling multi-slab byte stream buffer for zero-copy I/O
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_streambuffer.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void adt_streambuffer_slab_shrink_if_needed(adt_streambuffer_t *self, uint8_t slab_idx);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// LIFECYCLE MANAGEMENT
//////////////////////////////////////////////////////////////////////////////

void adt_streambuffer_create(adt_streambuffer_t *self, uint32_t default_slab_size, uint32_t max_retained_size)
{
   if (self != NULL)
   {
      self->default_slab_size = (default_slab_size == 0u) ? ADT_STREAMBUFFER_DEFAULT_SLAB_SIZE : default_slab_size;
      self->max_retained_size = (max_retained_size == 0u) ? ADT_STREAMBUFFER_DEFAULT_MAX_RETAINED_SIZE : max_retained_size;
      if (self->max_retained_size < self->default_slab_size)
      {
         self->max_retained_size = self->default_slab_size;
      }
      self->write_slab_idx = 0u;
      self->read_slab_idx = 0u;
      self->read_pos = 0u;

      for (uint8_t i = 0u; i < ADT_STREAMBUFFER_NUM_SLABS; i++)
      {
         adt_bytearray_create(&self->slabs[i]);
         (void) adt_bytearray_reserve(&self->slabs[i], self->default_slab_size);
      }
   }
}

void adt_streambuffer_destroy(adt_streambuffer_t *self)
{
   if (self != NULL)
   {
      for (uint8_t i = 0u; i < ADT_STREAMBUFFER_NUM_SLABS; i++)
      {
         adt_bytearray_destroy(&self->slabs[i]);
      }
      self->write_slab_idx = 0u;
      self->read_slab_idx = 0u;
      self->read_pos = 0u;
      self->default_slab_size = 0u;
      self->max_retained_size = 0u;
   }
}

adt_streambuffer_t* adt_streambuffer_new(uint32_t default_slab_size, uint32_t max_retained_size)
{
   adt_streambuffer_t *self = (adt_streambuffer_t*) malloc(sizeof(adt_streambuffer_t));
   if (self != NULL)
   {
      adt_streambuffer_create(self, default_slab_size, max_retained_size);
   }
   return self;
}

void adt_streambuffer_delete(adt_streambuffer_t *self)
{
   if (self != NULL)
   {
      adt_streambuffer_destroy(self);
      free(self);
   }
}

void adt_streambuffer_vdelete(void *arg)
{
   adt_streambuffer_delete((adt_streambuffer_t*) arg);
}

void adt_streambuffer_clear(adt_streambuffer_t *self)
{
   if (self != NULL)
   {
      self->write_slab_idx = 0u;
      self->read_slab_idx = 0u;
      self->read_pos = 0u;
      for (uint8_t i = 0u; i < ADT_STREAMBUFFER_NUM_SLABS; i++)
      {
         self->slabs[i].u32CurLen = 0u;
         adt_streambuffer_slab_shrink_if_needed(self, i);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
// PRODUCER API (DATA INGESTION)
//////////////////////////////////////////////////////////////////////////////

adt_error_t adt_streambuffer_append(adt_streambuffer_t *self, const uint8_t *pData, uint32_t u32DataLen)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (u32DataLen == 0u)
   {
      return ADT_NO_ERROR;
   }
   if (pData == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }

   uint32_t avail = 0u;
   uint8_t *pWrite = adt_streambuffer_write_begin(self, u32DataLen, &avail);
   if (pWrite == NULL)
   {
      return ADT_MEM_ERROR;
   }
   assert(avail >= u32DataLen);

   memcpy(pWrite, pData, u32DataLen);
   return adt_streambuffer_write_commit(self, u32DataLen);
}

uint8_t* adt_streambuffer_write_begin(adt_streambuffer_t *self, uint32_t u32MinSpace, uint32_t *pAvailSpace)
{
   if (self == NULL)
   {
      if (pAvailSpace != NULL)
      {
         *pAvailSpace = 0u;
      }
      return NULL;
   }

   adt_bytearray_t *curr_slab = &self->slabs[self->write_slab_idx];
   uint32_t avail_in_curr = (curr_slab->u32AllocLen > curr_slab->u32CurLen) ?
                            (curr_slab->u32AllocLen - curr_slab->u32CurLen) : 0u;

   // If the current slab already has enough space to satisfy u32MinSpace (and at least 1 byte if u32MinSpace == 0)
   if ((avail_in_curr >= u32MinSpace) && ((u32MinSpace > 0u) || (avail_in_curr > 0u)))
   {
      if (pAvailSpace != NULL)
      {
         *pAvailSpace = avail_in_curr;
      }
      return curr_slab->pData + curr_slab->u32CurLen;
   }

   // If current slab is completely empty (no data at all), grow it directly
   if (curr_slab->u32CurLen == 0u)
   {
      uint32_t needed = (u32MinSpace > self->default_slab_size) ? u32MinSpace : self->default_slab_size;
      if (curr_slab->u32AllocLen < needed)
      {
         adt_error_t err = adt_bytearray_grow(curr_slab, needed);
         if (err != ADT_NO_ERROR)
         {
            return NULL;
         }
      }
      avail_in_curr = curr_slab->u32AllocLen - curr_slab->u32CurLen;
      if (pAvailSpace != NULL)
      {
         *pAvailSpace = avail_in_curr;
      }
      return curr_slab->pData;
   }

   // Insufficient space in current slab: rollover to next slab.
   uint32_t R = 0u;
   if (curr_slab->u32CurLen > self->read_pos)
   {
      R = curr_slab->u32CurLen - self->read_pos;
   }

   uint32_t min_write = (u32MinSpace > 0u) ? u32MinSpace : 1u;
   if (UINT32_MAX - R < min_write)
   {
      return NULL; // Arithmetic overflow guard
   }

   uint32_t total_needed = R + min_write;
   if (total_needed < self->default_slab_size)
   {
      total_needed = self->default_slab_size;
   }

   uint8_t next_idx = (self->write_slab_idx + 1u) % ADT_STREAMBUFFER_NUM_SLABS;
   adt_bytearray_t *next_slab = &self->slabs[next_idx];

   next_slab->u32CurLen = 0u;

   if (next_slab->u32AllocLen < total_needed)
   {
      adt_error_t err = adt_bytearray_grow(next_slab, total_needed);
      if (err != ADT_NO_ERROR)
      {
         return NULL;
      }
   }

   // Copy unread remainder R to offset 0 of next slab
   if (R > 0u)
   {
      assert(curr_slab->pData != NULL);
      assert(next_slab->pData != NULL);
      memcpy(next_slab->pData, curr_slab->pData + self->read_pos, R);
   }
   next_slab->u32CurLen = R;

   // Retire old slab and apply shrink policy if needed
   curr_slab->u32CurLen = 0u;
   adt_streambuffer_slab_shrink_if_needed(self, self->write_slab_idx);

   // Update stream indices
   self->read_slab_idx = next_idx;
   self->read_pos = 0u;
   self->write_slab_idx = next_idx;

   uint32_t avail_in_next = next_slab->u32AllocLen - next_slab->u32CurLen;
   if (pAvailSpace != NULL)
   {
      *pAvailSpace = avail_in_next;
   }
   return next_slab->pData + next_slab->u32CurLen;
}

adt_error_t adt_streambuffer_write_commit(adt_streambuffer_t *self, uint32_t u32BytesWritten)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (u32BytesWritten == 0u)
   {
      return ADT_NO_ERROR;
   }

   adt_bytearray_t *slab = &self->slabs[self->write_slab_idx];
   uint32_t avail = (slab->u32AllocLen > slab->u32CurLen) ?
                    (slab->u32AllocLen - slab->u32CurLen) : 0u;

   if (u32BytesWritten > avail)
   {
      return ADT_LENGTH_ERROR;
   }

   slab->u32CurLen += u32BytesWritten;
   return ADT_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// CONSUMER API (DATA EXTRACTION & FRAMING)
//////////////////////////////////////////////////////////////////////////////

const uint8_t* adt_streambuffer_read_begin(adt_streambuffer_t *self, uint32_t *pAvailBytes)
{
   if (self == NULL)
   {
      if (pAvailBytes != NULL)
      {
         *pAvailBytes = 0u;
      }
      return NULL;
   }

   adt_bytearray_t *read_slab = &self->slabs[self->read_slab_idx];
   uint32_t avail = 0u;
   if (read_slab->u32CurLen > self->read_pos)
   {
      avail = read_slab->u32CurLen - self->read_pos;
   }

   if (pAvailBytes != NULL)
   {
      *pAvailBytes = avail;
   }

   if (avail == 0u)
   {
      return NULL;
   }

   return read_slab->pData + self->read_pos;
}

adt_error_t adt_streambuffer_read_commit(adt_streambuffer_t *self, uint32_t u32BytesConsumed)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (u32BytesConsumed == 0u)
   {
      return ADT_NO_ERROR;
   }

   adt_bytearray_t *read_slab = &self->slabs[self->read_slab_idx];
   uint32_t avail = 0u;
   if (read_slab->u32CurLen > self->read_pos)
   {
      avail = read_slab->u32CurLen - self->read_pos;
   }

   if (u32BytesConsumed > avail)
   {
      return ADT_LENGTH_ERROR;
   }

   self->read_pos += u32BytesConsumed;

   // Drain in-place: if all bytes in the slab are consumed
   if (self->read_pos == read_slab->u32CurLen)
   {
      self->read_pos = 0u;
      read_slab->u32CurLen = 0u;
      adt_streambuffer_slab_shrink_if_needed(self, self->read_slab_idx);
   }

   return ADT_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// QUERY API
//////////////////////////////////////////////////////////////////////////////

uint32_t adt_streambuffer_length(const adt_streambuffer_t *self)
{
   if (self == NULL)
   {
      return 0u;
   }

   uint32_t total = 0u;
   for (uint8_t i = 0u; i < ADT_STREAMBUFFER_NUM_SLABS; i++)
   {
      if (i == self->read_slab_idx)
      {
         if (self->slabs[i].u32CurLen > self->read_pos)
         {
            total += (self->slabs[i].u32CurLen - self->read_pos);
         }
      }
      else
      {
         total += self->slabs[i].u32CurLen;
      }
   }
   return total;
}

uint32_t adt_streambuffer_allocated_bytes(const adt_streambuffer_t *self)
{
   if (self == NULL)
   {
      return 0u;
   }

   uint32_t total = 0u;
   for (uint8_t i = 0u; i < ADT_STREAMBUFFER_NUM_SLABS; i++)
   {
      total += self->slabs[i].u32AllocLen;
   }
   return total;
}

bool adt_streambuffer_is_empty(const adt_streambuffer_t *self)
{
   return (adt_streambuffer_length(self) == 0u);
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void adt_streambuffer_slab_shrink_if_needed(adt_streambuffer_t *self, uint8_t slab_idx)
{
   assert(self != NULL);
   assert(slab_idx < ADT_STREAMBUFFER_NUM_SLABS);

   adt_bytearray_t *slab = &self->slabs[slab_idx];
   if (slab->u32AllocLen > self->max_retained_size)
   {
      adt_bytearray_destroy(slab);
      adt_bytearray_create(slab);
      (void) adt_bytearray_reserve(slab, self->default_slab_size);
   }
}
