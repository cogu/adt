/*****************************************************************************
* \file      adt_partition.c
* \author    Conny Gustafsson
* \date      2026-10-03
* \brief     Contiguous partition index containers (16-bit and 32-bit offsets)
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_partition.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef MEM_LEAK_CHECK
# include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define ADT_PARTITION_INITIAL_CAPACITY 8u

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS: adt_u16_partition_t
//////////////////////////////////////////////////////////////////////////////

void adt_u16_partition_create(adt_u16_partition_t *self)
{
   if (self != NULL)
   {
      self->start_offsets = NULL;
      self->num_segments = 0u;
      self->capacity = 0u;
      self->total_size = 0u;
   }
}

void adt_u16_partition_destroy(adt_u16_partition_t *self)
{
   if (self != NULL)
   {
      if (self->start_offsets != NULL)
      {
         free(self->start_offsets);
         self->start_offsets = NULL;
      }
      self->num_segments = 0u;
      self->capacity = 0u;
      self->total_size = 0u;
   }
}

adt_u16_partition_t* adt_u16_partition_new(void)
{
   adt_u16_partition_t *self = (adt_u16_partition_t*)malloc(sizeof(adt_u16_partition_t));
   if (self != NULL)
   {
      adt_u16_partition_create(self);
   }
   return self;
}

void adt_u16_partition_delete(adt_u16_partition_t *self)
{
   if (self != NULL)
   {
      adt_u16_partition_destroy(self);
      free(self);
   }
}

void adt_u16_partition_vdelete(void *arg)
{
   adt_u16_partition_delete((adt_u16_partition_t*)arg);
}

void adt_u16_partition_clear(adt_u16_partition_t *self)
{
   if (self != NULL)
   {
      self->num_segments = 0u;
      self->total_size = 0u;
      if (self->start_offsets != NULL)
      {
         self->start_offsets[0] = 0u;
      }
   }
}

adt_error_t adt_u16_partition_reserve(adt_u16_partition_t *self, const uint32_t capacity)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (capacity <= self->capacity)
   {
      return ADT_NO_ERROR;
   }
   uint32_t const num_entries = capacity + 1u;
   if (num_entries < capacity)
   {
      return ADT_ARRAY_TOO_LARGE_ERROR;
   }
   size_t const alloc_size = (size_t)num_entries * sizeof(uint16_t);
   uint16_t *new_offsets = (uint16_t*)realloc(self->start_offsets, alloc_size);
   if (new_offsets == NULL)
   {
      return ADT_MEM_ERROR;
   }
   if (self->start_offsets == NULL)
   {
      new_offsets[0] = 0u;
   }
   self->start_offsets = new_offsets;
   self->capacity = capacity;
   return ADT_NO_ERROR;
}

adt_error_t adt_u16_partition_append(adt_u16_partition_t *self, const uint16_t segment_length)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   uint32_t const new_total = (uint32_t)self->total_size + (uint32_t)segment_length;
   if (new_total > UINT16_MAX)
   {
      return ADT_OVERFLOW_ERROR;
   }
   if (self->num_segments >= self->capacity)
   {
      uint32_t new_capacity = (self->capacity == 0u) ? ADT_PARTITION_INITIAL_CAPACITY : (self->capacity * 2u);
      if (new_capacity < self->capacity)
      {
         return ADT_ARRAY_TOO_LARGE_ERROR;
      }
      adt_error_t const err = adt_u16_partition_reserve(self, new_capacity);
      if (err != ADT_NO_ERROR)
      {
         return err;
      }
   }
   self->total_size = (uint16_t)new_total;
   self->start_offsets[self->num_segments + 1u] = self->total_size;
   self->num_segments++;
   return ADT_NO_ERROR;
}

int32_t adt_u16_partition_find(const adt_u16_partition_t *self, const uint16_t offset)
{
   if ((self == NULL) || (self->num_segments == 0u) || (offset >= self->total_size))
   {
      return -1;
   }
   int32_t left = 0;
   int32_t right = (int32_t)self->num_segments - 1;
   int32_t result = -1;

   while (left <= right)
   {
      int32_t const mid = left + (right - left) / 2;
      if (self->start_offsets[mid] <= offset)
      {
         result = mid;
         left = mid + 1;
      }
      else
      {
         right = mid - 1;
      }
   }
   return result;
}

uint32_t adt_u16_partition_length(const adt_u16_partition_t *self)
{
   return (self != NULL) ? self->num_segments : 0u;
}

uint16_t adt_u16_partition_total_size(const adt_u16_partition_t *self)
{
   return (self != NULL) ? self->total_size : 0u;
}

bool adt_u16_partition_is_empty(const adt_u16_partition_t *self)
{
   return (self == NULL) || (self->num_segments == 0u);
}

adt_error_t adt_u16_partition_get_segment(const adt_u16_partition_t *self, const uint32_t index, uint16_t *start_offset, uint16_t *length)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (index >= self->num_segments)
   {
      return ADT_INDEX_OUT_OF_BOUNDS_ERROR;
   }
   if (start_offset != NULL)
   {
      *start_offset = self->start_offsets[index];
   }
   if (length != NULL)
   {
      *length = (uint16_t)(self->start_offsets[index + 1u] - self->start_offsets[index]);
   }
   return ADT_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS: adt_u32_partition_t
//////////////////////////////////////////////////////////////////////////////

void adt_u32_partition_create(adt_u32_partition_t *self)
{
   if (self != NULL)
   {
      self->start_offsets = NULL;
      self->num_segments = 0u;
      self->capacity = 0u;
      self->total_size = 0u;
   }
}

void adt_u32_partition_destroy(adt_u32_partition_t *self)
{
   if (self != NULL)
   {
      if (self->start_offsets != NULL)
      {
         free(self->start_offsets);
         self->start_offsets = NULL;
      }
      self->num_segments = 0u;
      self->capacity = 0u;
      self->total_size = 0u;
   }
}

adt_u32_partition_t* adt_u32_partition_new(void)
{
   adt_u32_partition_t *self = (adt_u32_partition_t*)malloc(sizeof(adt_u32_partition_t));
   if (self != NULL)
   {
      adt_u32_partition_create(self);
   }
   return self;
}

void adt_u32_partition_delete(adt_u32_partition_t *self)
{
   if (self != NULL)
   {
      adt_u32_partition_destroy(self);
      free(self);
   }
}

void adt_u32_partition_vdelete(void *arg)
{
   adt_u32_partition_delete((adt_u32_partition_t*)arg);
}

void adt_u32_partition_clear(adt_u32_partition_t *self)
{
   if (self != NULL)
   {
      self->num_segments = 0u;
      self->total_size = 0u;
      if (self->start_offsets != NULL)
      {
         self->start_offsets[0] = 0u;
      }
   }
}

adt_error_t adt_u32_partition_reserve(adt_u32_partition_t *self, const uint32_t capacity)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (capacity <= self->capacity)
   {
      return ADT_NO_ERROR;
   }
   uint32_t const num_entries = capacity + 1u;
   if (num_entries < capacity)
   {
      return ADT_ARRAY_TOO_LARGE_ERROR;
   }
   size_t const alloc_size = (size_t)num_entries * sizeof(uint32_t);
   uint32_t *new_offsets = (uint32_t*)realloc(self->start_offsets, alloc_size);
   if (new_offsets == NULL)
   {
      return ADT_MEM_ERROR;
   }
   if (self->start_offsets == NULL)
   {
      new_offsets[0] = 0u;
   }
   self->start_offsets = new_offsets;
   self->capacity = capacity;
   return ADT_NO_ERROR;
}

adt_error_t adt_u32_partition_append(adt_u32_partition_t *self, const uint32_t segment_length)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   uint64_t const new_total = (uint64_t)self->total_size + (uint64_t)segment_length;
   if (new_total > UINT32_MAX)
   {
      return ADT_OVERFLOW_ERROR;
   }
   if (self->num_segments >= self->capacity)
   {
      uint32_t new_capacity = (self->capacity == 0u) ? ADT_PARTITION_INITIAL_CAPACITY : (self->capacity * 2u);
      if (new_capacity < self->capacity)
      {
         return ADT_ARRAY_TOO_LARGE_ERROR;
      }
      adt_error_t const err = adt_u32_partition_reserve(self, new_capacity);
      if (err != ADT_NO_ERROR)
      {
         return err;
      }
   }
   self->total_size = (uint32_t)new_total;
   self->start_offsets[self->num_segments + 1u] = self->total_size;
   self->num_segments++;
   return ADT_NO_ERROR;
}

int32_t adt_u32_partition_find(const adt_u32_partition_t *self, const uint32_t offset)
{
   if ((self == NULL) || (self->num_segments == 0u) || (offset >= self->total_size))
   {
      return -1;
   }
   int32_t left = 0;
   int32_t right = (int32_t)self->num_segments - 1;
   int32_t result = -1;

   while (left <= right)
   {
      int32_t const mid = left + (right - left) / 2;
      if (self->start_offsets[mid] <= offset)
      {
         result = mid;
         left = mid + 1;
      }
      else
      {
         right = mid - 1;
      }
   }
   return result;
}

uint32_t adt_u32_partition_length(const adt_u32_partition_t *self)
{
   return (self != NULL) ? self->num_segments : 0u;
}

uint32_t adt_u32_partition_total_size(const adt_u32_partition_t *self)
{
   return (self != NULL) ? self->total_size : 0u;
}

bool adt_u32_partition_is_empty(const adt_u32_partition_t *self)
{
   return (self == NULL) || (self->num_segments == 0u);
}

adt_error_t adt_u32_partition_get_segment(const adt_u32_partition_t *self, const uint32_t index, uint32_t *start_offset, uint32_t *length)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (index >= self->num_segments)
   {
      return ADT_INDEX_OUT_OF_BOUNDS_ERROR;
   }
   if (start_offset != NULL)
   {
      *start_offset = self->start_offsets[index];
   }
   if (length != NULL)
   {
      *length = self->start_offsets[index + 1u] - self->start_offsets[index];
   }
   return ADT_NO_ERROR;
}
