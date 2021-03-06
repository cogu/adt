/*****************************************************************************
* \file      adt_bytearray.c
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
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_bytearray.h"
#include "adt_bytes.h"
#include <string.h>
#include <malloc.h>
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
static adt_error_t adt_bytearray_realloc(adt_bytearray_t *self, uint32_t u32NewLen);


//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void adt_bytearray_create(adt_bytearray_t *self,uint32_t u32GrowSize){
   if(self){
      self->pData = 0;
      self->u32AllocLen = 0;
      self->u32CurLen = 0;
      adt_bytearray_setGrowthSize(self, u32GrowSize);
   }
}

void adt_bytearray_destroy(adt_bytearray_t *self){
   if(self){
      if(self->pData != 0){
         free(self->pData);
         self->pData = 0;
      }
   }
}

adt_bytearray_t *adt_bytearray_new(uint32_t u32GrowSize)
{
   adt_bytearray_t *self = (adt_bytearray_t*) malloc(sizeof(adt_bytearray_t));
   if(self != 0){
      adt_bytearray_create(self,u32GrowSize);
   }
   return self;
}

adt_bytearray_t *adt_bytearray_make(const uint8_t *pData, uint32_t u32DataLen, uint32_t u32GrowSize)
{
   if (pData != 0)
   {
      adt_bytearray_t *self = adt_bytearray_new(u32GrowSize);
      if (self != 0)
      {
         adt_error_t errorCode = adt_bytearray_append(self, pData, u32DataLen);
         if (errorCode != ADT_NO_ERROR)
         {
            adt_bytearray_delete(self);
            self = (adt_bytearray_t*) 0;
         }
      }
      return self;
   }
   return (adt_bytearray_t*) 0;
}

adt_bytearray_t *adt_bytearray_make_cstr(const char *cstr, uint32_t u32GrowSize)
{
   if (cstr != 0)
   {
      size_t len = strlen(cstr);
      return adt_bytearray_make((const uint8_t*) cstr, (uint32_t) len, u32GrowSize);
   }
   return (adt_bytearray_t*) 0;
}

adt_bytearray_t *adt_bytearray_clone(const adt_bytearray_t *other, uint32_t u32GrowSize)
{
   if (other != 0)
   {
      return adt_bytearray_make(other->pData, other->u32CurLen, u32GrowSize);
   }
   return (adt_bytearray_t*) 0;
}

void adt_bytearray_delete(adt_bytearray_t *self)
{
   if (self != 0)
   {
      adt_bytearray_destroy(self);
      free(self);
   }
}

void adt_bytearray_vdelete(void *arg)
{
   adt_bytearray_delete((adt_bytearray_t*) arg);
}

void adt_bytearray_setGrowthSize(adt_bytearray_t *self,uint32_t u32GrowSize)
{
   if (self != 0)
   {
      if ( u32GrowSize > ADT_BYTE_ARRAY_MAX_GROW_SIZE )
      {
         u32GrowSize = ADT_BYTE_ARRAY_DEFAULT_GROW_SIZE;
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

/**
 * Removes all bytes to the left of pSrc, saves all bytes to the right of pSrc (including pSrc itself)
 * \param self pointer to bytearray_t
 * \param pSrc pointer to a byte inside the array
 */
adt_error_t adt_bytearray_trimLeft(adt_bytearray_t *self, const uint8_t *pSrc){
   if( (self!=0) && (pSrc!=0) && (self->pData <= pSrc) && (pSrc <= self->pData + self->u32CurLen) ){
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
 * grows byte array by a predefined size
 */
adt_error_t adt_bytearray_grow(adt_bytearray_t *self, uint32_t u32MinLen){
   if( self != 0 ){
      if (u32MinLen > self->u32AllocLen) {
         uint32_t u32NewLen = self->u32AllocLen;
         if (self->u32GrowSize > 0){
            while(u32NewLen<u32MinLen){
               u32NewLen += self->u32GrowSize;
            }
         }
         else
         {
            u32NewLen = u32MinLen;
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
   if(self != 0)
   {
      adt_error_t errorCode;
      if ( (self->u32GrowSize == ADT_BYTE_ARRAY_NO_GROWTH) && (u32NewLen < self->u32CurLen) ) {
         errorCode = adt_bytearray_realloc(self, u32NewLen); //reduce allocated array
      }
      else {
         errorCode = adt_bytearray_grow(self, u32NewLen);
      }
      if (errorCode == ADT_NO_ERROR) {
         self->u32CurLen = u32NewLen;
      }
      return errorCode;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

/**
 * Appends a single uint8_t value to the bytearray
 */
adt_error_t adt_bytearray_push(adt_bytearray_t *self, uint8_t value)
{
   if(self != 0)
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
   if(self != 0){
      return self->pData;
   }
   return 0;
}

uint8_t const* adt_bytearray_const_data(adt_bytearray_t const* self)
{
   if (self != 0)
   {
      return (uint8_t const*) self->pData;
   }
   return 0;
}

uint32_t adt_bytearray_length(const adt_bytearray_t *self){
   if(self != 0){
      return self->u32CurLen;
   }
   return 0;
}

void adt_bytearray_clear(adt_bytearray_t *self){
   if(self != 0){
      self->u32CurLen = 0;
   }
}

/**
 * Returns true if both bytearrays are of equal length and equal content
 */
bool adt_bytearray_equals(const adt_bytearray_t *lhs, const adt_bytearray_t *rhs)
{
   if ( (lhs != 0) && (rhs != 0) )
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
   if ( (self != 0) && (data != 0) )
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
   if (self != 0)
   {
      return adt_bytes_new(self->pData, self->u32CurLen);
   }
   return (adt_bytes_t*) 0;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static adt_error_t adt_bytearray_realloc(adt_bytearray_t *self, uint32_t u32NewLen) {
   if (self != 0) {
      uint8_t *pNewData = (uint8_t*) malloc(u32NewLen);
      if(pNewData != 0){
         if(self->pData != 0) {
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
