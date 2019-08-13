/*****************************************************************************
* \file:    bytearray.c
* \author:  Conny Gustafsson
* \date:    2015-02-05
* \brief:   general purpose data container
*
* Copyright (c) 2015-2019 Conny Gustafsson
*
******************************************************************************/
#include "adt_bytearray.h"
#include <string.h>
#include <malloc.h>
#include <assert.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


/**************** Private Function Declarations *******************/
static adt_error_t adt_bytearray_realloc(adt_bytearray_t *self, uint32_t u32NewLen);

/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
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
   else{
      adt_setError(ADT_MEM_ERROR);
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
            adt_setError(errorCode);
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
         adt_error_t erorCode = adt_bytearray_grow(self,u32NewLen);
         if(erorCode != ADT_NO_ERROR){
            return erorCode;
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
      start = pSrc - self->pData;
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

uint8_t *adt_bytearray_data(const adt_bytearray_t *self){
   if(self != 0){
      return self->pData;
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
bool adt_bytearray_equals(const adt_bytearray_t *lhs, const adt_bytearray_t *rhs){
   if ( (lhs != 0) && (rhs != 0) ) {
      int32_t leftLen;
      int32_t rightLen;
      leftLen = adt_bytearray_length(lhs);
      rightLen = adt_bytearray_length(rhs);
      if (leftLen == rightLen) {
         int32_t i;
         const uint8_t *pLeft;
         const uint8_t *pRight;
         for(i=0, pLeft=lhs->pData, pRight=rhs->pData; i<leftLen; i++){
            if (*pLeft++ != *pRight++ ) {
               return false;
            }
         }
         return true;
      }
   }
   return false;
}

/***************** Private Function Definitions *******************/
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
