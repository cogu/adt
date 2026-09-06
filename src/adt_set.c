/*****************************************************************************
* \file      adt_set.c
* \author    Conny Gustafsson
* \date      2018-07-27
* \brief     Set container for 32-bit unsigned integers
*
* Copyright (c) 2018-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <malloc.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include "adt_set.h"
#ifdef MEM_LEAK_CHECK
# include "CMemLeak.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define ADT_U32SET_MIN_CAPACITY 4

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static bool adt_u32Set_find_index(const adt_u32Set_t *self, uint32_t val, int32_t *out_index);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void adt_u32Set_create(adt_u32Set_t *self)
{
   if (self != NULL)
   {
      self->pAlloc = NULL;
      self->s32AllocLen = 0;
      self->s32CurLen = 0;
   }
}

void adt_u32Set_destroy(adt_u32Set_t *self)
{
   if (self != NULL)
   {
      if (self->pAlloc != NULL)
      {
         free(self->pAlloc);
      }
      self->pAlloc = NULL;
      self->s32AllocLen = 0;
      self->s32CurLen = 0;
   }
}

adt_u32Set_t*  adt_u32Set_new(void)
{
   adt_u32Set_t *self = (adt_u32Set_t*) malloc(sizeof(adt_u32Set_t));
   if (self != NULL)
   {
      adt_u32Set_create(self);
   }
   return self;
}

void  adt_u32Set_delete(adt_u32Set_t *self)
{
   if (self != NULL)
   {
      adt_u32Set_destroy(self);
      free(self);
   }
}

void  adt_u32Set_vdelete(void *arg)
{
   adt_u32Set_delete((adt_u32Set_t*) arg);
}

void adt_u32Set_clear(adt_u32Set_t *self)
{
   if (self != NULL)
   {
      self->s32CurLen = 0;
   }
}

int32_t adt_u32Set_length(adt_u32Set_t *self)
{
   if (self != NULL)
   {
      return self->s32CurLen;
   }
   return -1;
}

/**
 * inserts val into the internal array. The array is automatically kept sorted.
 */
void adt_u32Set_insert(adt_u32Set_t *self, uint32_t val)
{
   if (self != NULL)
   {
      int32_t insert_pos = 0;
      if (adt_u32Set_find_index(self, val, &insert_pos))
      {
         return; // Value already exists, prevent duplicates
      }

      if (self->s32CurLen == INT32_MAX)
      {
         return;
      }

      if (self->s32CurLen == self->s32AllocLen)
      {
         int32_t new_capacity = (self->s32AllocLen < ADT_U32SET_MIN_CAPACITY) ? ADT_U32SET_MIN_CAPACITY : self->s32AllocLen * 2;
         if (self->s32AllocLen > (INT32_MAX / 2))
         {
            new_capacity = INT32_MAX;
         }
         uint32_t *pAlloc = (uint32_t*) realloc(self->pAlloc, sizeof(uint32_t) * (size_t)new_capacity);
         if (pAlloc == NULL)
         {
            return;
         }
         self->pAlloc = pAlloc;
         self->s32AllocLen = new_capacity;
      }

      int32_t num_to_move = self->s32CurLen - insert_pos;
      if (num_to_move > 0)
      {
         memmove(&self->pAlloc[insert_pos + 1], &self->pAlloc[insert_pos], sizeof(uint32_t) * (size_t)num_to_move);
      }

      self->pAlloc[insert_pos] = val;
      self->s32CurLen++;
   }
}

bool adt_u32Set_remove(adt_u32Set_t *self, uint32_t val)
{
   if ((self != NULL) && (self->s32CurLen > 0))
   {
      int32_t remove_pos = 0;
      if (adt_u32Set_find_index(self, val, &remove_pos))
      {
         int32_t num_to_move = self->s32CurLen - 1 - remove_pos;
         if (num_to_move > 0)
         {
            memmove(&self->pAlloc[remove_pos], &self->pAlloc[remove_pos + 1], sizeof(uint32_t) * (size_t)num_to_move);
         }
         self->s32CurLen--;
         return true;
      }
   }
   return false;
}

bool adt_u32Set_contains(adt_u32Set_t *self, uint32_t val)
{
   if ((self != NULL) && (self->s32CurLen > 0))
   {
      return adt_u32Set_find_index(self, val, NULL);
   }
   return false;
}

bool adt_u32Set_is_empty(const adt_u32Set_t *self)
{
   if (self != NULL)
   {
      return self->s32CurLen == 0;
   }
   return false;
}

uint32_t adt_u32Set_value(const adt_u32Set_t *self, int32_t index)
{
   if ((self != NULL) && (index >= 0) && (index < self->s32CurLen))
   {
      return self->pAlloc[index];
   }
   return 0;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static bool adt_u32Set_find_index(const adt_u32Set_t *self, uint32_t val, int32_t *out_index)
{
   int32_t left = 0;
   int32_t right = self->s32CurLen - 1;

   while (left <= right)
   {
      int32_t mid = left + (right - left) / 2;
      uint32_t mid_val = self->pAlloc[mid];

      if (mid_val == val)
      {
         if (out_index != NULL)
         {
            *out_index = mid;
         }
         return true;
      }
      else if (mid_val < val)
      {
         left = mid + 1;
      }
      else
      {
         right = mid - 1;
      }
   }

   if (out_index != NULL)
   {
      *out_index = left;
   }
   return false;
}
