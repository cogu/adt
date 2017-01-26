/*****************************************************************************
* \file:    bytearray.c
* \author:  Conny Gustafsson
* \date:    2015-02-05
* \brief:   general purpose data container
*
* Copyright (c) 2015-2016 Conny Gustafsson
*
******************************************************************************/
#include "adt_bytearray.h"
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>


/**************** Private Function Declarations *******************/


/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
void adt_bytearray_create(adt_bytearray_t *self,uint32_t u32GrowSize){
   if(self){
      self->pData = 0;
      self->u32AllocLen = 0;
      self->u32CurLen = 0;
      if ( (u32GrowSize == 0) || (u32GrowSize > ADT_BYTE_ARRAY_MAX_GROW_SIZE) )
      {
         u32GrowSize = ADT_BYTE_ARRAY_DEFAULT_GROW_SIZE;
      }
      self->u32GrowSize = u32GrowSize;
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
      errno = ENOMEM;
   }
   return self;
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


int8_t adt_bytearray_reserve(adt_bytearray_t *self, uint32_t u32NewLen){
   if(self){
      if(u32NewLen > self->u32AllocLen){
         int8_t rc = adt_bytearray_grow(self,u32NewLen);
         if(rc != 0){
            return rc;
         }
      }
      return 0;
   }
   return -1;
}

int8_t adt_bytearray_append(adt_bytearray_t *self, const uint8_t *pData, uint32_t u32DataLen){
   if(self && pData && (u32DataLen > 0)){
      if(adt_bytearray_reserve(self,self->u32CurLen + u32DataLen) == 0){
         uint8_t *pNext, *pEnd;
         pNext = self->pData + self->u32CurLen;
         pEnd = self->pData + self->u32AllocLen;
         assert(pNext + u32DataLen <= pEnd);
         memcpy(pNext,pData,u32DataLen);
         self->u32CurLen+=u32DataLen;
      }
      else{
         errno = ENOMEM;
         return -1;
      }
      return 0;
   }
   errno = EINVAL;
   return -1;
}

/**
 * Removes all bytes to the left of pSrc, saves all bytes to the right of pSrc (including pSrc itself)
 * \param self pointer to bytearray_t
 * \param pSrc pointer to a byte inside the array
 */
int8_t adt_bytearray_trimLeft(adt_bytearray_t *self, const uint8_t *pSrc){
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
      return 0;
   }
   errno = EINVAL;
   return -1;
}

/**
 * grows byte array by a predefined size
 */
int8_t adt_bytearray_grow(adt_bytearray_t *self, uint32_t u32MinLen){
   if( (self != 0) && (u32MinLen > self->u32AllocLen) && (self->u32GrowSize > 0)){
      uint8_t *pNewData;
      uint32_t u32NewLen = self->u32AllocLen;
      while(u32NewLen<u32MinLen){
         u32NewLen += self->u32GrowSize;
      }
      pNewData = (uint8_t*) malloc(u32NewLen);
      if(pNewData){
         if(self->pData != 0){
            memcpy(pNewData,self->pData,self->u32CurLen);
            free(self->pData);
         }
         self->pData = pNewData;
         self->u32AllocLen = u32NewLen;
      }
      else{
         return -1;
      }
   }
   return 0;
}

/**
 * resizes bytearray to newLen
 */
int8_t adt_bytearray_resize(adt_bytearray_t *self, uint32_t u32NewLen)
{
   if(self != 0)
   {
      int8_t result = adt_bytearray_grow(self, u32NewLen);
      if (result == 0)
      {
         self->u32CurLen = u32NewLen;
         return 0;
      }
   }
   return -1;
}

uint8_t *adt_bytearray_data(adt_bytearray_t *self){
   if(self != 0){
      return self->pData;
   }
   return 0;
}

uint32_t adt_bytearray_length(adt_bytearray_t *self){
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

/***************** Private Function Definitions *******************/

