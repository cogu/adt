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
static void adt_streambuffer_slab_shrink_if_needed(adt_streambuffer_t *self, const uint8_t slab_idx);
static adt_error_t adt_streambuffer_rollover(adt_streambuffer_t *self, const uint32_t total_needed, const uint32_t unread_bytes);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// LIFECYCLE MANAGEMENT
//////////////////////////////////////////////////////////////////////////////

void adt_streambuffer_create(adt_streambuffer_t *self, const uint32_t default_slab_size, const uint32_t max_retained_size)
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

adt_streambuffer_t* adt_streambuffer_new(const uint32_t default_slab_size, const uint32_t max_retained_size)
{
   adt_streambuffer_t * const self = (adt_streambuffer_t*) malloc(sizeof(adt_streambuffer_t));
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

adt_error_t adt_streambuffer_append(adt_streambuffer_t *self, const uint8_t *data, const uint32_t num_bytes)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (num_bytes == 0u)
   {
      return ADT_NO_ERROR;
   }
   if (data == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }

   uint32_t avail = 0u;
   uint8_t * const write_ptr = adt_streambuffer_write_begin(self, num_bytes, &avail);
   if (write_ptr == NULL)
   {
      return ADT_MEM_ERROR;
   }
   assert(avail >= num_bytes);

   memcpy(write_ptr, data, num_bytes);
   return adt_streambuffer_write_commit(self, num_bytes);
}

uint8_t* adt_streambuffer_write_begin(adt_streambuffer_t *self, const uint32_t min_bytes, uint32_t *avail_bytes)
{
   if (self == NULL)
   {
      if (avail_bytes != NULL)
      {
         *avail_bytes = 0u;
      }
      return NULL;
   }

   adt_bytearray_t * const curr_slab = &self->slabs[self->write_slab_idx];
   const uint32_t avail_in_curr = (curr_slab->u32AllocLen > curr_slab->u32CurLen) ?
                                  (curr_slab->u32AllocLen - curr_slab->u32CurLen) : 0u;

   // If the current slab already has enough space to satisfy min_bytes (and at least 1 byte if min_bytes == 0)
   if ((avail_in_curr >= min_bytes) && ((min_bytes > 0u) || (avail_in_curr > 0u)))
   {
      if (avail_bytes != NULL)
      {
         *avail_bytes = avail_in_curr;
      }
      return curr_slab->pData + curr_slab->u32CurLen;
   }

   // If current slab is completely empty (no data at all), grow it directly
   if (curr_slab->u32CurLen == 0u)
   {
      const uint32_t needed = (min_bytes > self->default_slab_size) ? min_bytes : self->default_slab_size;
      if (curr_slab->u32AllocLen < needed)
      {
         const adt_error_t err = adt_bytearray_grow(curr_slab, needed);
         if (err != ADT_NO_ERROR)
         {
            return NULL;
         }
      }
      const uint32_t avail_grown = curr_slab->u32AllocLen - curr_slab->u32CurLen;
      if (avail_bytes != NULL)
      {
         *avail_bytes = avail_grown;
      }
      return curr_slab->pData;
   }

   // Insufficient space in current slab: rollover to next slab.
   const uint32_t unread_bytes = (curr_slab->u32CurLen > self->read_pos) ?
                                 (curr_slab->u32CurLen - self->read_pos) : 0u;

   const uint32_t min_write = (min_bytes > 0u) ? min_bytes : 1u;
   if (UINT32_MAX - unread_bytes < min_write)
   {
      return NULL; // Arithmetic overflow guard
   }

   const uint32_t total_needed = unread_bytes + min_write;
   const adt_error_t err = adt_streambuffer_rollover(self, total_needed, unread_bytes);
   if (err != ADT_NO_ERROR)
   {
      return NULL;
   }

   adt_bytearray_t * const next_slab = &self->slabs[self->write_slab_idx];
   const uint32_t avail_in_next = next_slab->u32AllocLen - next_slab->u32CurLen;
   if (avail_bytes != NULL)
   {
      *avail_bytes = avail_in_next;
   }
   return next_slab->pData + next_slab->u32CurLen;
}

adt_error_t adt_streambuffer_write_commit(adt_streambuffer_t *self, const uint32_t written_bytes)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (written_bytes == 0u)
   {
      return ADT_NO_ERROR;
   }

   adt_bytearray_t * const slab = &self->slabs[self->write_slab_idx];
   const uint32_t avail = (slab->u32AllocLen > slab->u32CurLen) ?
                          (slab->u32AllocLen - slab->u32CurLen) : 0u;

   if (written_bytes > avail)
   {
      return ADT_LENGTH_ERROR;
   }

   slab->u32CurLen += written_bytes;
   return ADT_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// CONSUMER API (DATA EXTRACTION & FRAMING)
//////////////////////////////////////////////////////////////////////////////

const uint8_t* adt_streambuffer_read_begin(const adt_streambuffer_t *self, uint32_t *avail_bytes)
{
   if (self == NULL)
   {
      if (avail_bytes != NULL)
      {
         *avail_bytes = 0u;
      }
      return NULL;
   }

   const adt_bytearray_t * const read_slab = &self->slabs[self->read_slab_idx];
   const uint32_t avail = (read_slab->u32CurLen > self->read_pos) ?
                          (read_slab->u32CurLen - self->read_pos) : 0u;

   if (avail_bytes != NULL)
   {
      *avail_bytes = avail;
   }

   if (avail == 0u)
   {
      return NULL;
   }

   return read_slab->pData + self->read_pos;
}

adt_error_t adt_streambuffer_read_commit(adt_streambuffer_t *self, const uint32_t consumed_bytes)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (consumed_bytes == 0u)
   {
      return ADT_NO_ERROR;
   }

   adt_bytearray_t * const read_slab = &self->slabs[self->read_slab_idx];
   const uint32_t avail = (read_slab->u32CurLen > self->read_pos) ?
                          (read_slab->u32CurLen - self->read_pos) : 0u;

   if (consumed_bytes > avail)
   {
      return ADT_LENGTH_ERROR;
   }

   self->read_pos += consumed_bytes;

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
// CAPACITY MANAGEMENT
//////////////////////////////////////////////////////////////////////////////

adt_error_t adt_streambuffer_reserve(adt_streambuffer_t *self, const uint32_t min_bytes)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (min_bytes == 0u)
   {
      return ADT_NO_ERROR;
   }

   adt_bytearray_t * const curr_slab = &self->slabs[self->write_slab_idx];

   // Check if active slab already has enough contiguous capacity starting from read_pos
   if (curr_slab->u32AllocLen > self->read_pos)
   {
      const uint32_t contig_cap = curr_slab->u32AllocLen - self->read_pos;
      if (contig_cap >= min_bytes)
      {
         return ADT_NO_ERROR;
      }
   }

   const uint32_t needed = (min_bytes > self->default_slab_size) ? min_bytes : self->default_slab_size;

   // If reading from start of slab, simply grow in place without copying
   if (self->read_pos == 0u)
   {
      return adt_bytearray_grow(curr_slab, needed);
   }

   // Otherwise, perform compacting rollover to next slab
   const uint32_t unread_bytes = (curr_slab->u32CurLen > self->read_pos) ?
                                 (curr_slab->u32CurLen - self->read_pos) : 0u;
   return adt_streambuffer_rollover(self, needed, unread_bytes);
}

//////////////////////////////////////////////////////////////////////////////
// QUERY API
//////////////////////////////////////////////////////////////////////////////

uint32_t adt_streambuffer_size(const adt_streambuffer_t *self)
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
   return (adt_streambuffer_size(self) == 0u);
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void adt_streambuffer_slab_shrink_if_needed(adt_streambuffer_t *self, const uint8_t slab_idx)
{
   assert(self != NULL);
   assert(slab_idx < ADT_STREAMBUFFER_NUM_SLABS);

   adt_bytearray_t * const slab = &self->slabs[slab_idx];
   if (slab->u32AllocLen > self->max_retained_size)
   {
      adt_bytearray_destroy(slab);
      adt_bytearray_create(slab);
      (void) adt_bytearray_reserve(slab, self->default_slab_size);
   }
}

static adt_error_t adt_streambuffer_rollover(adt_streambuffer_t *self, const uint32_t total_needed, const uint32_t unread_bytes)
{
   const uint32_t needed = (total_needed < self->default_slab_size) ? self->default_slab_size : total_needed;
   const uint8_t next_idx = (self->write_slab_idx + 1u) % ADT_STREAMBUFFER_NUM_SLABS;
   adt_bytearray_t * const curr_slab = &self->slabs[self->write_slab_idx];
   adt_bytearray_t * const next_slab = &self->slabs[next_idx];

   next_slab->u32CurLen = 0u;

   if (next_slab->u32AllocLen < needed)
   {
      const adt_error_t err = adt_bytearray_grow(next_slab, needed);
      if (err != ADT_NO_ERROR)
      {
         return err;
      }
   }

   // Copy unread remainder to offset 0 of next slab
   if (unread_bytes > 0u)
   {
      assert(curr_slab->pData != NULL);
      assert(next_slab->pData != NULL);
      memcpy(next_slab->pData, curr_slab->pData + self->read_pos, unread_bytes);
   }
   next_slab->u32CurLen = unread_bytes;

   // Retire old slab and apply shrink policy if needed
   curr_slab->u32CurLen = 0u;
   adt_streambuffer_slab_shrink_if_needed(self, self->write_slab_idx);

   // Update stream indices
   self->read_slab_idx = next_idx;
   self->read_pos = 0u;
   self->write_slab_idx = next_idx;

   return ADT_NO_ERROR;
}
