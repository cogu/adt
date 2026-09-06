/*****************************************************************************
* \file      adt_bytearray.c
* \author    Conny Gustafsson
* \date      2015-02-05
* \brief     A mutable byte array
*
* Copyright (c) 2015-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_bytearray.h"
#include "adt_bytes.h"
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stddef.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define ADT_BYTEARRAY_MIN_CAPACITY 16u
#define ADT_BYTEARRAY_MAX_GROW_SIZE ((uint32_t)32u*1024u*1024u)

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static adt_error_t adt_bytearray_realloc(adt_bytearray_t *self, uint32_t u32NewLen);
static uint32_t adt_bytearray_next_capacity(uint32_t current_capacity, uint32_t min_needed);


//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void adt_bytearray_create(adt_bytearray_t *self)
{
   if (self != NULL)
   {
      self->pData = NULL;
      self->u32AllocLen = 0u;
      self->u32CurLen = 0u;
      self->u32GrowSize = 0u;
   }
}

void adt_bytearray_destroy(adt_bytearray_t *self)
{
   if (self != NULL)
   {
      if (self->pData != NULL)
      {
         free(self->pData);
         self->pData = NULL;
      }
   }
}

adt_bytearray_t *adt_bytearray_new(void)
{
   adt_bytearray_t *self = (adt_bytearray_t*) malloc(sizeof(adt_bytearray_t));
   if (self != NULL)
   {
      adt_bytearray_create(self);
   }
   return self;
}

adt_bytearray_t *adt_bytearray_make(const uint8_t *pData, uint32_t u32DataLen)
{
   if (pData != NULL)
   {
      adt_bytearray_t *self = adt_bytearray_new();
      if (self != NULL)
      {
         adt_error_t errorCode = adt_bytearray_append(self, pData, u32DataLen);
         if (errorCode != ADT_NO_ERROR)
         {
            adt_bytearray_delete(self);
            self = NULL;
         }
      }
      return self;
   }
   return NULL;
}

adt_bytearray_t *adt_bytearray_make_cstr(const char *cstr)
{
   if (cstr != NULL)
   {
      size_t len = strlen(cstr);
      return adt_bytearray_make((const uint8_t*) cstr, (uint32_t) len);
   }
   return NULL;
}

adt_bytearray_t *adt_bytearray_clone(const adt_bytearray_t *other)
{
   if (other != NULL)
   {
      adt_bytearray_t *clone = adt_bytearray_make(other->pData, other->u32CurLen);
      if ((clone != NULL) && (other->u32GrowSize > 0u))
      {
         clone->u32GrowSize = other->u32GrowSize;
      }
      return clone;
   }
   return NULL;
}

void adt_bytearray_delete(adt_bytearray_t *self)
{
   if (self != NULL)
   {
      adt_bytearray_destroy(self);
      free(self);
   }
}

void adt_bytearray_vdelete(void *arg)
{
   adt_bytearray_delete((adt_bytearray_t*) arg);
}

void adt_bytearray_set_growth_size(adt_bytearray_t *self, uint32_t u32GrowSize)
{
   if (self != NULL)
   {
      if (u32GrowSize > ADT_BYTEARRAY_MAX_GROW_SIZE)
      {
         u32GrowSize = ADT_BYTEARRAY_MAX_GROW_SIZE;
      }
      self->u32GrowSize = u32GrowSize;
   }
}

adt_error_t adt_bytearray_reserve(adt_bytearray_t *self, uint32_t u32NewLen){
   if(self){
      if(u32NewLen > self->u32AllocLen){
         adt_error_t errorCode = adt_bytearray_grow(self,u32NewLen);
         if(errorCode != ADT_NO_ERROR){
            return errorCode;
         }
      }
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

adt_error_t adt_bytearray_append(adt_bytearray_t *self, const uint8_t *pData, uint32_t u32DataLen){
   if(self && pData && (u32DataLen > 0)){
      adt_error_t errorCode = adt_bytearray_reserve(self, self->u32CurLen + u32DataLen);
      if(errorCode == ADT_NO_ERROR){
         uint8_t *pNext, *pEnd;
         pNext = self->pData + self->u32CurLen;
         pEnd = self->pData + self->u32AllocLen;
         assert(pNext + u32DataLen <= pEnd);
         memcpy(pNext,pData,u32DataLen);
         self->u32CurLen+=u32DataLen;
      }
      return errorCode;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

adt_error_t adt_bytearray_trim_left(adt_bytearray_t *self, const uint8_t *pSrc){
   if( (self != NULL) && (pSrc != NULL) && (self->pData <= pSrc) && (pSrc <= self->pData + self->u32CurLen) ){
      uint32_t start, remain;
      /*
       * boundary cases:
       *    pBegin = self->pData
       *    =>
       *       start = 0
       *       remain = self->u32CurLen
       *
       *    pBegin = self->pData+self->u32CurLen
       *    =>
       *       start = self->u32CurLen
       *       remain = 0
       */
      start = (uint32_t) (pSrc - self->pData);
      remain = self->u32CurLen - start;
      if(pSrc == self->pData){
         //no action
         assert(start == 0);
      }
      else if(remain == 0){
         //remove all
         self->u32CurLen = 0;
      }
      else{
         memmove(self->pData,pSrc,remain);
         self->u32CurLen = remain;
      }
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

/**
 * grows byte array using geometric growth (or fixed chunks if u32GrowSize > 0)
 */
adt_error_t adt_bytearray_grow(adt_bytearray_t *self, uint32_t u32MinLen)
{
   if (self != NULL)
   {
      if (u32MinLen > self->u32AllocLen)
      {
         uint32_t u32NewLen;
         if (self->u32GrowSize > 0u)
         {
            u32NewLen = self->u32AllocLen;
            while (u32NewLen < u32MinLen)
            {
               if (u32NewLen > (UINT32_MAX - self->u32GrowSize))
               {
                  u32NewLen = UINT32_MAX;
                  break;
               }
               u32NewLen += self->u32GrowSize;
            }
         }
         else
         {
            u32NewLen = adt_bytearray_next_capacity(self->u32AllocLen, u32MinLen);
         }
         return adt_bytearray_realloc(self, u32NewLen);
      }
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

/**
 * resizes bytearray to newLen
 */
adt_error_t adt_bytearray_resize(adt_bytearray_t *self, uint32_t u32NewLen)
{
   if (self != NULL)
   {
      if (u32NewLen > self->u32AllocLen)
      {
         adt_error_t errorCode = adt_bytearray_grow(self, u32NewLen);
         if (errorCode != ADT_NO_ERROR)
         {
            return errorCode;
         }
      }
      self->u32CurLen = u32NewLen;
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

/**
 * Appends a single uint8_t value to the bytearray
 */
adt_error_t adt_bytearray_push(adt_bytearray_t *self, uint8_t value)
{
   if(self != NULL)
   {
      adt_error_t errorCode = adt_bytearray_reserve(self, self->u32CurLen + 1);
      if(errorCode == ADT_NO_ERROR)
      {
         uint8_t *pNext, *pEnd;
         pNext = self->pData + self->u32CurLen;
         pEnd = self->pData + self->u32AllocLen;
         assert(pNext + 1 <= pEnd);
         *pNext = value;
         self->u32CurLen++;
      }
      return errorCode;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

uint8_t *adt_bytearray_data(const adt_bytearray_t *self){
   if(self != NULL){
      return self->pData;
   }
   return NULL;
}

uint8_t const* adt_bytearray_const_data(adt_bytearray_t const* self)
{
   if (self != NULL)
   {
      return (uint8_t const*) self->pData;
   }
   return NULL;
}

uint32_t adt_bytearray_length(const adt_bytearray_t *self){
   if(self != NULL){
      return self->u32CurLen;
   }
   return 0;
}

void adt_bytearray_clear(adt_bytearray_t *self){
   if(self != NULL){
      self->u32CurLen = 0;
   }
}

/**
 * Returns true if both bytearrays are of equal length and equal content
 */
bool adt_bytearray_equals(const adt_bytearray_t *lhs, const adt_bytearray_t *rhs)
{
   if ( (lhs != NULL) && (rhs != NULL) )
   {
      uint32_t leftLen;
      uint32_t rightLen;
      leftLen = adt_bytearray_length(lhs);
      rightLen = adt_bytearray_length(rhs);
      if (leftLen == rightLen)
      {
         if (leftLen == true)
         {
            return true;
         }
         else
         {
            return (memcmp(lhs->pData, rhs->pData, leftLen) == 0) ? true : false;
         }
      }
   }
   return false;
}

bool adt_bytearray_data_equals(const adt_bytearray_t* self, const uint8_t* data, uint32_t dataLen)
{
   if ( (self != NULL) && (data != NULL) )
   {
      uint32_t myLen;
      myLen = adt_bytearray_length(self);
      if (myLen == dataLen)
      {
         const uint8_t* myData = self->pData;
         if (myLen == 0u)
         {
            return true;
         }
         else
         {
            return (memcmp(myData, data, myLen) == 0) ? true : false;
         }
      }
   }
   return false;
}

struct adt_bytes_tag* adt_bytearray_bytes(const adt_bytearray_t *self)
{
   if (self != NULL)
   {
      return adt_bytes_new(self->pData, self->u32CurLen);
   }
   return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static adt_error_t adt_bytearray_realloc(adt_bytearray_t *self, uint32_t u32NewLen) {
   if (self != NULL) {
      uint8_t *pNewData = (uint8_t*) malloc(u32NewLen);
      if(pNewData != NULL){
         if(self->pData != NULL) {
            uint32_t u32CopyLen = (u32NewLen < self->u32CurLen)? u32NewLen : self->u32CurLen;
            memcpy(pNewData, self->pData, u32CopyLen);
            free(self->pData);
         }
         self->pData = pNewData;
         self->u32AllocLen = u32NewLen;
      }
      else {
         return ADT_MEM_ERROR;
      }
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

static uint32_t adt_bytearray_next_capacity(uint32_t current_capacity, uint32_t min_needed)
{
   uint32_t new_capacity = (current_capacity < ADT_BYTEARRAY_MIN_CAPACITY) ? ADT_BYTEARRAY_MIN_CAPACITY : current_capacity;
   while (new_capacity < min_needed)
   {
      if (new_capacity >= ADT_BYTEARRAY_MAX_GROW_SIZE)
      {
         if (new_capacity > (UINT32_MAX - ADT_BYTEARRAY_MAX_GROW_SIZE))
         {
            new_capacity = UINT32_MAX;
            break;
         }
         new_capacity += ADT_BYTEARRAY_MAX_GROW_SIZE;
      }
      else
      {
         if (new_capacity > (ADT_BYTEARRAY_MAX_GROW_SIZE / 2u))
         {
            new_capacity = ADT_BYTEARRAY_MAX_GROW_SIZE;
         }
         else
         {
            new_capacity *= 2u;
         }
      }
   }
   return new_capacity;
}
