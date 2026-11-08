/*****************************************************************************
* \file      adt_range_list.c
* \author    Conny Gustafsson
* \date      2026-11-08
* \brief     Disjoint sorted range list containers (16-bit and 32-bit offsets)
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_range_list.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef MEM_LEAK_CHECK
# include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define ADT_RANGE_LIST_INITIAL_CAPACITY 8u

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS: adt_u16_range_list_t
//////////////////////////////////////////////////////////////////////////////

void adt_u16_range_list_create(adt_u16_range_list_t *self)
{
   if (self != NULL)
   {
      self->ranges = NULL;
      self->num_ranges = 0u;
      self->capacity = 0u;
   }
}

void adt_u16_range_list_destroy(adt_u16_range_list_t *self)
{
   if (self != NULL)
   {
      if (self->ranges != NULL)
      {
         free(self->ranges);
         self->ranges = NULL;
      }
      self->num_ranges = 0u;
      self->capacity = 0u;
   }
}

adt_u16_range_list_t* adt_u16_range_list_new(void)
{
   adt_u16_range_list_t *self = (adt_u16_range_list_t*)malloc(sizeof(adt_u16_range_list_t));
   if (self != NULL)
   {
      adt_u16_range_list_create(self);
   }
   return self;
}

void adt_u16_range_list_delete(adt_u16_range_list_t *self)
{
   if (self != NULL)
   {
      adt_u16_range_list_destroy(self);
      free(self);
   }
}

void adt_u16_range_list_vdelete(void *arg)
{
   adt_u16_range_list_delete((adt_u16_range_list_t*)arg);
}

void adt_u16_range_list_clear(adt_u16_range_list_t *self)
{
   if (self != NULL)
   {
      self->num_ranges = 0u;
   }
}

adt_error_t adt_u16_range_list_reserve(adt_u16_range_list_t *self, const uint32_t capacity)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (capacity > self->capacity)
   {
      adt_u16_range_t *new_ranges = (adt_u16_range_t*)realloc(self->ranges, capacity * sizeof(adt_u16_range_t));
      if (new_ranges == NULL)
      {
         return ADT_MEM_ERROR;
      }
      self->ranges = new_ranges;
      self->capacity = capacity;
   }
   return ADT_NO_ERROR;
}

adt_error_t adt_u16_range_list_insert(adt_u16_range_list_t *self, const uint16_t start, const uint16_t length, const uint16_t max_gap)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (length == 0u)
   {
      return ADT_NO_ERROR;
   }
   if ((uint32_t)UINT16_MAX - (uint32_t)start < (uint32_t)length)
   {
      return ADT_OVERFLOW_ERROR;
   }

   const uint32_t new_start = (uint32_t)start;
   const uint32_t new_end = (uint32_t)start + (uint32_t)length;

   uint32_t first = 0u;
   while (first < self->num_ranges)
   {
      uint32_t const r_end = (uint32_t)self->ranges[first].start + (uint32_t)self->ranges[first].length;
      if ((new_start > r_end) && ((new_start - r_end) > (uint32_t)max_gap))
      {
         first++;
      }
      else
      {
         break;
      }
   }

   if ((first == self->num_ranges) ||
       (((uint32_t)self->ranges[first].start > new_end) &&
        (((uint32_t)self->ranges[first].start - new_end) > (uint32_t)max_gap)))
   {
      if (self->num_ranges == self->capacity)
      {
         uint32_t const new_capacity = (self->capacity == 0u) ? ADT_RANGE_LIST_INITIAL_CAPACITY : (self->capacity * 2u);
         adt_error_t const rc = adt_u16_range_list_reserve(self, new_capacity);
         if (rc != ADT_NO_ERROR)
         {
            return rc;
         }
      }
      if (first < self->num_ranges)
      {
         memmove(&self->ranges[first + 1u], &self->ranges[first], (self->num_ranges - first) * sizeof(adt_u16_range_t));
      }
      self->ranges[first].start = (uint16_t)new_start;
      self->ranges[first].length = (uint16_t)length;
      self->num_ranges++;
      return ADT_NO_ERROR;
   }

   uint32_t merged_start = ((uint32_t)self->ranges[first].start < new_start) ? (uint32_t)self->ranges[first].start : new_start;
   uint32_t const r_first_end = (uint32_t)self->ranges[first].start + (uint32_t)self->ranges[first].length;
   uint32_t merged_end = (r_first_end > new_end) ? r_first_end : new_end;

   uint32_t last = first + 1u;
   while (last < self->num_ranges)
   {
      if (((uint32_t)self->ranges[last].start <= merged_end) ||
          (((uint32_t)self->ranges[last].start - merged_end) <= (uint32_t)max_gap))
      {
         uint32_t const r_last_end = (uint32_t)self->ranges[last].start + (uint32_t)self->ranges[last].length;
         if (r_last_end > merged_end)
         {
            merged_end = r_last_end;
         }
         last++;
      }
      else
      {
         break;
      }
   }

   self->ranges[first].start = (uint16_t)merged_start;
   self->ranges[first].length = (uint16_t)(merged_end - merged_start);

   if (last > (first + 1u))
   {
      uint32_t const num_swallowed = last - (first + 1u);
      uint32_t const remaining = self->num_ranges - last;
      if (remaining > 0u)
      {
         memmove(&self->ranges[first + 1u], &self->ranges[last], remaining * sizeof(adt_u16_range_t));
      }
      self->num_ranges -= num_swallowed;
   }

   return ADT_NO_ERROR;
}

uint32_t adt_u16_range_list_length(const adt_u16_range_list_t *self)
{
   if (self != NULL)
   {
      return self->num_ranges;
   }
   return 0u;
}

bool adt_u16_range_list_is_empty(const adt_u16_range_list_t *self)
{
   if (self != NULL)
   {
      return self->num_ranges == 0u;
   }
   return true;
}

adt_error_t adt_u16_range_list_get_range(const adt_u16_range_list_t *self, const uint32_t index, adt_u16_range_t *range)
{
   if ((self == NULL) || (range == NULL))
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (index >= self->num_ranges)
   {
      return ADT_INDEX_OUT_OF_BOUNDS_ERROR;
   }
   *range = self->ranges[index];
   return ADT_NO_ERROR;
}

const adt_u16_range_t* adt_u16_range_list_at(const adt_u16_range_list_t *self, const uint32_t index)
{
   if ((self != NULL) && (index < self->num_ranges))
   {
      return &self->ranges[index];
   }
   return NULL;
}

int32_t adt_u16_range_list_find(const adt_u16_range_list_t *self, const uint16_t offset)
{
   if ((self == NULL) || (self->num_ranges == 0u))
   {
      return -1;
   }
   int32_t left = 0;
   int32_t right = (int32_t)self->num_ranges - 1;
   while (left <= right)
   {
      int32_t const mid = left + (right - left) / 2;
      uint32_t const r_start = (uint32_t)self->ranges[mid].start;
      uint32_t const r_end = r_start + (uint32_t)self->ranges[mid].length;
      if ((uint32_t)offset < r_start)
      {
         right = mid - 1;
      }
      else if ((uint32_t)offset >= r_end)
      {
         left = mid + 1;
      }
      else
      {
         return mid;
      }
   }
   return -1;
}

bool adt_u16_range_list_contains(const adt_u16_range_list_t *self, const uint16_t offset)
{
   return adt_u16_range_list_find(self, offset) >= 0;
}

uint32_t adt_u16_range_list_total_length(const adt_u16_range_list_t *self)
{
   uint32_t total = 0u;
   if (self != NULL)
   {
      uint32_t i;
      for (i = 0u; i < self->num_ranges; i++)
      {
         total += (uint32_t)self->ranges[i].length;
      }
   }
   return total;
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS: adt_u32_range_list_t
//////////////////////////////////////////////////////////////////////////////

void adt_u32_range_list_create(adt_u32_range_list_t *self)
{
   if (self != NULL)
   {
      self->ranges = NULL;
      self->num_ranges = 0u;
      self->capacity = 0u;
   }
}

void adt_u32_range_list_destroy(adt_u32_range_list_t *self)
{
   if (self != NULL)
   {
      if (self->ranges != NULL)
      {
         free(self->ranges);
         self->ranges = NULL;
      }
      self->num_ranges = 0u;
      self->capacity = 0u;
   }
}

adt_u32_range_list_t* adt_u32_range_list_new(void)
{
   adt_u32_range_list_t *self = (adt_u32_range_list_t*)malloc(sizeof(adt_u32_range_list_t));
   if (self != NULL)
   {
      adt_u32_range_list_create(self);
   }
   return self;
}

void adt_u32_range_list_delete(adt_u32_range_list_t *self)
{
   if (self != NULL)
   {
      adt_u32_range_list_destroy(self);
      free(self);
   }
}

void adt_u32_range_list_vdelete(void *arg)
{
   adt_u32_range_list_delete((adt_u32_range_list_t*)arg);
}

void adt_u32_range_list_clear(adt_u32_range_list_t *self)
{
   if (self != NULL)
   {
      self->num_ranges = 0u;
   }
}

adt_error_t adt_u32_range_list_reserve(adt_u32_range_list_t *self, const uint32_t capacity)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (capacity > self->capacity)
   {
      adt_u32_range_t *new_ranges = (adt_u32_range_t*)realloc(self->ranges, capacity * sizeof(adt_u32_range_t));
      if (new_ranges == NULL)
      {
         return ADT_MEM_ERROR;
      }
      self->ranges = new_ranges;
      self->capacity = capacity;
   }
   return ADT_NO_ERROR;
}

adt_error_t adt_u32_range_list_insert(adt_u32_range_list_t *self, const uint32_t start, const uint32_t length, const uint32_t max_gap)
{
   if (self == NULL)
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (length == 0u)
   {
      return ADT_NO_ERROR;
   }
   if (UINT32_MAX - start < length)
   {
      return ADT_OVERFLOW_ERROR;
   }

   const uint32_t new_start = start;
   const uint32_t new_end = start + length;

   uint32_t first = 0u;
   while (first < self->num_ranges)
   {
      uint32_t const r_end = self->ranges[first].start + self->ranges[first].length;
      if ((new_start > r_end) && ((new_start - r_end) > max_gap))
      {
         first++;
      }
      else
      {
         break;
      }
   }

   if ((first == self->num_ranges) ||
       ((self->ranges[first].start > new_end) &&
        ((self->ranges[first].start - new_end) > max_gap)))
   {
      if (self->num_ranges == self->capacity)
      {
         uint32_t const new_capacity = (self->capacity == 0u) ? ADT_RANGE_LIST_INITIAL_CAPACITY : (self->capacity * 2u);
         adt_error_t const rc = adt_u32_range_list_reserve(self, new_capacity);
         if (rc != ADT_NO_ERROR)
         {
            return rc;
         }
      }
      if (first < self->num_ranges)
      {
         memmove(&self->ranges[first + 1u], &self->ranges[first], (self->num_ranges - first) * sizeof(adt_u32_range_t));
      }
      self->ranges[first].start = new_start;
      self->ranges[first].length = length;
      self->num_ranges++;
      return ADT_NO_ERROR;
   }

   uint32_t merged_start = (self->ranges[first].start < new_start) ? self->ranges[first].start : new_start;
   uint32_t const r_first_end = self->ranges[first].start + self->ranges[first].length;
   uint32_t merged_end = (r_first_end > new_end) ? r_first_end : new_end;

   uint32_t last = first + 1u;
   while (last < self->num_ranges)
   {
      if ((self->ranges[last].start <= merged_end) ||
          ((self->ranges[last].start - merged_end) <= max_gap))
      {
         uint32_t const r_last_end = self->ranges[last].start + self->ranges[last].length;
         if (r_last_end > merged_end)
         {
            merged_end = r_last_end;
         }
         last++;
      }
      else
      {
         break;
      }
   }

   self->ranges[first].start = merged_start;
   self->ranges[first].length = merged_end - merged_start;

   if (last > (first + 1u))
   {
      uint32_t const num_swallowed = last - (first + 1u);
      uint32_t const remaining = self->num_ranges - last;
      if (remaining > 0u)
      {
         memmove(&self->ranges[first + 1u], &self->ranges[last], remaining * sizeof(adt_u32_range_t));
      }
      self->num_ranges -= num_swallowed;
   }

   return ADT_NO_ERROR;
}

uint32_t adt_u32_range_list_length(const adt_u32_range_list_t *self)
{
   if (self != NULL)
   {
      return self->num_ranges;
   }
   return 0u;
}

bool adt_u32_range_list_is_empty(const adt_u32_range_list_t *self)
{
   if (self != NULL)
   {
      return self->num_ranges == 0u;
   }
   return true;
}

adt_error_t adt_u32_range_list_get_range(const adt_u32_range_list_t *self, const uint32_t index, adt_u32_range_t *range)
{
   if ((self == NULL) || (range == NULL))
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (index >= self->num_ranges)
   {
      return ADT_INDEX_OUT_OF_BOUNDS_ERROR;
   }
   *range = self->ranges[index];
   return ADT_NO_ERROR;
}

const adt_u32_range_t* adt_u32_range_list_at(const adt_u32_range_list_t *self, const uint32_t index)
{
   if ((self != NULL) && (index < self->num_ranges))
   {
      return &self->ranges[index];
   }
   return NULL;
}

int32_t adt_u32_range_list_find(const adt_u32_range_list_t *self, const uint32_t offset)
{
   if ((self == NULL) || (self->num_ranges == 0u))
   {
      return -1;
   }
   int32_t left = 0;
   int32_t right = (int32_t)self->num_ranges - 1;
   while (left <= right)
   {
      int32_t const mid = left + (right - left) / 2;
      uint32_t const r_start = self->ranges[mid].start;
      uint32_t const r_end = r_start + self->ranges[mid].length;
      if (offset < r_start)
      {
         right = mid - 1;
      }
      else if (offset >= r_end)
      {
         left = mid + 1;
      }
      else
      {
         return mid;
      }
   }
   return -1;
}

bool adt_u32_range_list_contains(const adt_u32_range_list_t *self, const uint32_t offset)
{
   return adt_u32_range_list_find(self, offset) >= 0;
}

uint64_t adt_u32_range_list_total_length(const adt_u32_range_list_t *self)
{
   uint64_t total = 0u;
   if (self != NULL)
   {
      uint32_t i;
      for (i = 0u; i < self->num_ranges; i++)
      {
         total += (uint64_t)self->ranges[i].length;
      }
   }
   return total;
}

