/*****************************************************************************
* \file      adt_ringbuf.c
* \author    Conny Gustafsson
* \date      2013-12-19
* \brief     Ringbuffer data structure
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <string.h>
#include <stddef.h>
#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
#include <malloc.h>
#include <limits.h>
#endif
#include "adt_ringbuf.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define RBFH_NUM_ACCEPTED_LENGTHS 8

#ifndef ADT_RBFSH_MIN_NUM_ELEM
#define ADT_RBFSH_MIN_NUM_ELEM ADT_RBFSH_MIN_NUM_ELEMS_DEFAULT
#endif

#ifndef ADT_RBFSH_MAX_NUM_ELEM
#define ADT_RBFSH_MAX_NUM_ELEM ADT_RBFSH_MAX_NUM_ELEM_DEFAULT
#endif

#ifdef UNIT_TEST
#define DYN_STATIC
#else
#define DYN_STATIC static
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
static adt_error_t adt_rbfh_grow(adt_rbfh_t* self);
static void adt_rbfh_swapBuffers(adt_rbfh_t* self, uint8_t *newAllocBuf, uint16_t newAllocLen);
DYN_STATIC  uint16_t adt_rbfh_nextLen(uint16_t wanted);
#endif


//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

adt_error_t adt_rbfs_create(adt_rbfs_t* u8Rbf, uint8_t* u8Buffer, uint16_t u16NumElem, uint8_t u8ElemSize)
{
   if ((u8Rbf == NULL) || (u8Buffer == NULL) || (u16NumElem == 0) || (u8ElemSize == 0))
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   u8Rbf->u8Buffer = u8Buffer;
   u8Rbf->u8ReadPtr = u8Buffer;
   u8Rbf->u8WritePtr = u8Buffer;
   u8Rbf->u8ElemSize = u8ElemSize;
   u8Rbf->u16MaxNumElem = u16NumElem;
   u8Rbf->u16NumElem = 0;
   return ADT_NO_ERROR;
}

adt_error_t adt_rbfs_insert(adt_rbfs_t* u8Rbf, const uint8_t* u8Data)
{
   if ((u8Rbf == NULL) || (u8Data == NULL))
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   uint8_t u8i;
   uint8_t* u8EndPtr = u8Rbf->u8Buffer + (((size_t) u8Rbf->u16MaxNumElem) * u8Rbf->u8ElemSize);

   if (u8Rbf->u16NumElem >= u8Rbf->u16MaxNumElem)
   {
      return ADT_OVERFLOW_ERROR;
   }

   u8Rbf->u16NumElem++;

   //copy data from elem to buffer
   for (u8i = 0; u8i < u8Rbf->u8ElemSize; u8i++)
   {
      *(u8Rbf->u8WritePtr++) = *(u8Data++);
   }

   if (u8Rbf->u8WritePtr >= u8EndPtr)
   {
      //rewind
      u8Rbf->u8WritePtr = u8Rbf->u8Buffer;
   }

   return ADT_NO_ERROR;
}

adt_error_t adt_rbfs_remove(adt_rbfs_t* u8Rbf, uint8_t* u8Data)
{
   if ((u8Rbf == NULL) || (u8Data == NULL))
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   uint8_t u8i;
   uint8_t* u8EndPtr = u8Rbf->u8Buffer + (((size_t) u8Rbf->u16MaxNumElem) * u8Rbf->u8ElemSize);

   if (u8Rbf->u16NumElem == 0)
   {
      return ADT_UNDERFLOW_ERROR;
   }

   for (u8i = 0; u8i < u8Rbf->u8ElemSize; u8i++)
   {
      *(u8Data++) = *(u8Rbf->u8ReadPtr++);
   }

   if (u8Rbf->u8ReadPtr >= u8EndPtr)
   {
      //rewind
      u8Rbf->u8ReadPtr = u8Rbf->u8Buffer;
   }

   u8Rbf->u16NumElem--;
   return ADT_NO_ERROR;
}

uint16_t adt_rbfs_size(const adt_rbfs_t* u8Rbf)
{
   return u8Rbf->u16NumElem;
}

uint16_t adt_rbfs_free(const adt_rbfs_t* rbf)
{
   return (uint16_t) (rbf->u16MaxNumElem-rbf->u16NumElem);
}

adt_error_t adt_rbfs_peek(const adt_rbfs_t* rbf, uint8_t* u8Data)
{
   if ((rbf == NULL) || (u8Data == NULL))
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   uint8_t u8i;
   uint8_t* u8ReadPtr = rbf->u8ReadPtr;

   if (rbf->u16NumElem == 0)
   {
      return ADT_UNDERFLOW_ERROR;
   }

   for (u8i = 0; u8i < rbf->u8ElemSize; u8i++)
   {
      *(u8Data++) = *(u8ReadPtr++);
   }

   return ADT_NO_ERROR;
}

void adt_rbfs_clear(adt_rbfs_t* rbf)
{
   rbf->u8ReadPtr = rbf->u8Buffer;
   rbf->u8WritePtr = rbf->u8Buffer;
   rbf->u16NumElem = 0;
}

adt_error_t adt_rbfu16_create(adt_rbfu16_t* rbf, uint16_t* u16Buffer, uint16_t u16NumElem){
   if ((rbf != NULL) && (u16Buffer != NULL) && (u16NumElem > 0)){
      rbf->u16Buffer = u16Buffer;
      rbf->u16MaxNumElem = u16NumElem;
      rbf->u16NumElem = 0;
      rbf->u16ReadPtr = u16Buffer;
      rbf->u16WritePtr = u16Buffer;
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

adt_error_t adt_rbfu16_insert(adt_rbfu16_t* rbf, uint16_t u16Data){
   if( rbf != NULL ){
      uint16_t *pEndPtr = rbf->u16Buffer + rbf->u16MaxNumElem;
      if(rbf->u16NumElem >= rbf->u16MaxNumElem){
         //full
         return ADT_OVERFLOW_ERROR;
      }
      rbf->u16NumElem++;
      *rbf->u16WritePtr++ = u16Data;
      if (rbf->u16WritePtr >= pEndPtr)
      {
          //rewind
         rbf->u16WritePtr = rbf->u16Buffer;
      }
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

adt_error_t adt_rbfu16_remove(adt_rbfu16_t* rbf, uint16_t* u16Data){
   if( (rbf != NULL) && (u16Data != NULL) ){
      uint16_t *pEndPtr = rbf->u16Buffer + rbf->u16MaxNumElem;
      if(rbf->u16NumElem == 0){
         //empty
         return ADT_UNDERFLOW_ERROR;
      }
      *u16Data = *rbf->u16ReadPtr++;
      if (rbf->u16ReadPtr >= pEndPtr)
      {
          //rewind
         rbf->u16ReadPtr = rbf->u16Buffer;
      }
      rbf->u16NumElem--;
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

adt_error_t adt_rbfu16_peek(const adt_rbfu16_t* rbf, uint16_t* u16Data){
   if( (rbf != NULL) && (u16Data != NULL) ){
      if(rbf->u16NumElem == 0){
         //empty
         return ADT_UNDERFLOW_ERROR;
      }
      *u16Data = *rbf->u16ReadPtr;
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

uint16_t adt_rbfu16_length(const adt_rbfu16_t* rbf){
   if(rbf != NULL){
      return rbf->u16NumElem;
   }
   return 0;
}

#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
/**
 * Creates new heap-managed ringbuffer containing elements of fixed size (u8ElemSize)
 */
adt_error_t adt_rbfh_create(adt_rbfh_t* self, uint8_t u8ElemSize)
{
   return adt_rbfh_create_with_params(self, u8ElemSize, ADT_RBFSH_MIN_NUM_ELEM, ADT_RBFSH_MAX_NUM_ELEM);
}

/**
 * More advanced constructor which allows user to select lower and upper limit of the number of elements
 * When u16MaxNumElems is 0, it is treated as if it has no upper limit
 */
adt_error_t adt_rbfh_create_with_params(adt_rbfh_t* self, uint8_t u8ElemSize, uint16_t u16MinNumElems, uint16_t u16MaxNumElems)
{
   if ( (self != NULL) && (u8ElemSize > 0) && ( (u16MaxNumElems == 0) || (u16MaxNumElems > u16MinNumElems) ) )
   {
      if (u16MinNumElems > 0)
      {
         self->u8AllocBuf = (uint8_t*) malloc( ((size_t) u8ElemSize) * u16MinNumElems);
         if (self->u8AllocBuf == NULL)
         {
            return ADT_MEM_ERROR;
         }
      }
      else
      {
         self->u8AllocBuf = NULL;
      }
      self->u16NumElem = 0;
      self->u16AllocLen = u16MinNumElems;
      self->u16MaxNumElem = (u16MaxNumElems == 0)? USHRT_MAX : u16MaxNumElems;
      self->u8ElemSize = u8ElemSize;
      self->u8ReadPtr = self->u8AllocBuf;
      self->u8WritePtr = self->u8AllocBuf;
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

/**
 * Destructor
 */
void adt_rbfh_destroy(adt_rbfh_t* self)
{
   if ( (self != NULL) && (self->u8AllocBuf != NULL) )
   {
      free(self->u8AllocBuf);
   }
}

/**
 * Allocates and returns new ringbuffer allocated from heap.
 */
adt_rbfh_t *adt_rbfh_new(uint8_t u8ElemSize)
{
   adt_rbfh_t *self = (adt_rbfh_t*) malloc(sizeof(adt_rbfh_t));
   if (self != NULL)
   {
      adt_buf_err_t result = adt_rbfh_create(self, u8ElemSize);
      if (result != BUF_E_OK)
      {
         free(self);
         self = NULL;
      }
   }
   return self;
}

/**
 * Same as adt_rbfh_new but internally calls adt_rbfh_create_with_params.
 */
adt_rbfh_t *adt_rbfh_new_with_params(uint8_t u8ElemSize, uint16_t u16MinNumElems, uint16_t u16MaxNumElems)
{
   adt_rbfh_t *self = (adt_rbfh_t*) malloc(sizeof(adt_rbfh_t));
   if (self != NULL)
   {
      adt_buf_err_t result = adt_rbfh_create_with_params(self, u8ElemSize, u16MinNumElems, u16MaxNumElems);
      if (result != BUF_E_OK)
      {
         free(self);
         self = NULL;
      }
   }
   return self;
}

/**
 * Deletes ringbuffer created using adt_rbfh_new or adt_rbfh_new_with_params
 */
void adt_rbfh_delete(adt_rbfh_t* self)
{
   if (self != NULL)
   {
      adt_rbfh_destroy(self);
      free(self);
   }
}


/**
 * Inserts one element into the rinbguffer
 */
adt_error_t adt_rbfh_insert(adt_rbfh_t* self, const uint8_t* u8Data)
{
   if ((self == NULL) || (u8Data == NULL))
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (self->u16NumElem == self->u16AllocLen)
   {
      adt_error_t result = adt_rbfh_grow(self);
      if (result != ADT_NO_ERROR)
      {
         return result;
      }
   }
   uint8_t* u8EndPtr = self->u8AllocBuf + (((size_t) self->u16AllocLen) * self->u8ElemSize);
   self->u16NumElem++;
   memcpy(self->u8WritePtr, u8Data, (size_t) self->u8ElemSize);
   self->u8WritePtr+=self->u8ElemSize;
   if (self->u8WritePtr == u8EndPtr)
   {
      self->u8WritePtr = self->u8AllocBuf;
   }
   return ADT_NO_ERROR;
}

/**
 * Removes (and copies) oldest element from the ringbuffer.
 */
adt_error_t adt_rbfh_remove(adt_rbfh_t* self, uint8_t* u8Data)
{
   if ((self == NULL) || (u8Data == NULL))
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (self->u16NumElem == 0)
   {
      return ADT_UNDERFLOW_ERROR;
   }
   uint8_t* u8EndPtr = self->u8AllocBuf + (((size_t) self->u16AllocLen) * self->u8ElemSize);
   memcpy(u8Data, self->u8ReadPtr, (size_t) self->u8ElemSize);
   self->u8ReadPtr+=self->u8ElemSize;
   if (self->u8ReadPtr == u8EndPtr)
   {
      self->u8ReadPtr = self->u8AllocBuf;
   }
   self->u16NumElem--;
   return ADT_NO_ERROR;
}

/**
 * Copies oldest data element but does not (yet) remove it from the buffer.
 */
adt_error_t adt_rbfh_peek(const adt_rbfh_t* self, uint8_t* u8Data)
{
   if ((self == NULL) || (u8Data == NULL))
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (self->u16NumElem == 0)
   {
      return ADT_UNDERFLOW_ERROR;
   }

   memcpy(u8Data, self->u8ReadPtr, (size_t) self->u8ElemSize);

   return ADT_NO_ERROR;
}

/**
 * Returns the number of elements waiting to be removed/read
 */
uint16_t adt_rbfh_length(const adt_rbfh_t* self)
{
   if (self != NULL)
   {
      return self->u16NumElem;
   }
   return 0;
}

/**
 * Returns how many elements can still be inserted before internal memory runs out
 */
uint16_t adt_rbfh_free(const adt_rbfh_t* self)
{
   if (self != NULL)
   {
      return USHRT_MAX - self->u16NumElem;
   }
   return 0;
}

/**
 * Clears the ringbuffer and resets internal read and write pointers
 */
void adt_rbfh_clear(adt_rbfh_t* self)
{
   if (self != NULL)
   {
      self->u8ReadPtr = self->u8WritePtr = self->u8AllocBuf;
      self->u16NumElem = 0;
   }
}

#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
static adt_error_t adt_rbfh_grow(adt_rbfh_t* self)
{
   if (self->u16AllocLen == USHRT_MAX)
   {
      return ADT_OVERFLOW_ERROR;
   }
   else
   {
      uint8_t *newAllocBuf;
      uint16_t newAllocLen = adt_rbfh_nextLen(self->u16AllocLen+1);
      newAllocBuf = (uint8_t*) malloc( ((size_t) self->u8ElemSize) * newAllocLen);
      if (newAllocBuf != NULL)
      {
         adt_rbfh_swapBuffers(self, newAllocBuf, newAllocLen);
         return ADT_NO_ERROR;
      }
   }
   return ADT_MEM_ERROR;
}


/**
 * When a ring buffer is full both the read pointer and the write pointer are at the same place
 * Copying all data to a new buffer is done in two steps.
 * 1. Copy all data from the read pointer up to the end of buffer
 * 2. Copy all the data from the beginning of the buffer up to the read pointer
 */
static void adt_rbfh_swapBuffers(adt_rbfh_t* self, uint8_t *newAllocBuf, uint16_t newAllocLen)
{
   if ( (self->u8AllocBuf == NULL) || (self->u8ReadPtr == NULL) || (self->u8WritePtr == NULL))
   {
      self->u8AllocBuf = newAllocBuf;
      self->u16AllocLen = newAllocLen;
      self->u8ReadPtr = self->u8WritePtr = newAllocBuf;
      self->u16NumElem = 0;
   }
   else
   {
      uint8_t *mark;
      uint32_t copyLen;
      uint8_t *u8EndPtr;
      uint32_t allocSize = ((uint32_t) self->u16AllocLen) * ((uint32_t) self->u8ElemSize);
      assert(self->u8AllocBuf != NULL);
      assert(allocSize > 0);
      u8EndPtr = self->u8AllocBuf + allocSize;
      //Step 1.
      copyLen = (uint32_t) (u8EndPtr-self->u8ReadPtr);
      assert(copyLen <= allocSize);
      memcpy(newAllocBuf, self->u8ReadPtr, copyLen);
      mark = newAllocBuf+copyLen;

      //Step 2.
      copyLen =  (uint32_t) (self->u8ReadPtr - self->u8AllocBuf);
      assert(copyLen <= allocSize);
      if (copyLen > 0)
      {
         memcpy(mark, self->u8AllocBuf, copyLen);
         mark+=copyLen;
      }

      //Update pointers and free memory
      free(self->u8AllocBuf);
      self->u8AllocBuf = newAllocBuf;
      self->u16AllocLen = newAllocLen;
      self->u8ReadPtr = newAllocBuf;
      self->u8WritePtr = mark;
   }
}

DYN_STATIC uint16_t adt_rbfh_nextLen(uint16_t wanted)
{
   static const uint16_t accepted[RBFH_NUM_ACCEPTED_LENGTHS] = {10u, 50u, 100u, 500u, 1000u, 5000u, 10000u, 50000u};
   uint8_t i;
   for(i=0u; i<RBFH_NUM_ACCEPTED_LENGTHS; i++)
   {
      if (accepted[i] >= wanted)
      {
         return accepted[i];
      }
   }
   return USHRT_MAX;
}
#endif
