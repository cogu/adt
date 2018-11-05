/*****************************************************************************
* \file      adt_ringbuf_heap.c
* \author    Conny Gustafsson
* \date      2018-10-17
* \brief     Ringbuffers using heap memory
*
* Copyright (c) 2018 Conny Gustafsson
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
#include <assert.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include "adt_ringbuf.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
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


#define NUM_ACCEPTED_LENGTHS 8
//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////


#if(ADT_RBFSH_ENABLE)
static adt_buf_err_t adt_rbfh_grow(adt_rbfh_t* self);
static void adt_rbfh_swapBuffers(adt_rbfh_t* self, uint8_t *newAllocBuf, uint16_t newAllocLen);
DYN_STATIC  uint16_t adt_rbfh_nextLen(uint16_t wanted);
#endif
//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
#if(ADT_RBFSH_ENABLE)
adt_buf_err_t adt_rbfh_create(adt_rbfh_t* self, uint8_t u8ElemSize)
{
   return adt_rbfh_createEx(self, u8ElemSize, ADT_RBFSH_MIN_NUM_ELEM, ADT_RBFSH_MAX_NUM_ELEM);
}

adt_buf_err_t adt_rbfh_createEx(adt_rbfh_t* self, uint8_t u8ElemSize, uint16_t u16MinNumElems, uint16_t u16MaxNumElems)
{
   if ( (self != 0) && (u8ElemSize > 0) && ( (u16MaxNumElems == 0) || (u16MaxNumElems > u16MinNumElems) ) )
   {
      if (u16MinNumElems > 0)
      {
         self->u8AllocBuf = (uint8_t*) malloc( ((uint32_t)u8ElemSize)*u16MinNumElems);
         if (self->u8AllocBuf == 0)
         {
            return BUF_E_NOT_OK;
         }
      }
      else
      {
         self->u8AllocBuf = (uint8_t*) 0;
      }
      self->u16NumElem = 0;
      self->u16AllocLen = u16MinNumElems;
      self->u16MaxNumElem = (u16MaxNumElems == 0)? USHRT_MAX : u16MaxNumElems;
      self->u8ElemSize = u8ElemSize;
      self->u8ReadPtr = self->u8AllocBuf;
      self->u8WritePtr = self->u8AllocBuf;
      return BUF_E_OK;
   }
   return BUF_E_NOT_OK;
}

void adt_rbfh_destroy(adt_rbfh_t* self)
{
   if ( (self != 0) && (self->u8AllocBuf != 0) )
   {
      free(self->u8AllocBuf);
   }
}

/**
 * Copies one element (of data) from u8Data into the ringbuffer
 */
adt_buf_err_t adt_rbfh_insert(adt_rbfh_t* self, const uint8_t* u8Data)
{
   uint8_t* u8EndPtr = self->u8AllocBuf + (self->u16AllocLen * ((uint32_t)self->u8ElemSize));

   if (self->u16NumElem == self->u16AllocLen)
   {
      adt_buf_err_t result = adt_rbfh_grow(self);
      if (result != BUF_E_OK)
      {
         return result;
      }
   }
   self->u16NumElem++;
   memcpy(self->u8WritePtr, u8Data, (size_t) self->u8ElemSize);
   self->u8WritePtr+=self->u8ElemSize;
   if (self->u8WritePtr == u8EndPtr)
   {
      self->u8WritePtr = self->u8AllocBuf;
   }
   return BUF_E_OK;
}

/**
 * Copies oldest element into u8Data then removes the element.
 */
adt_buf_err_t adt_rbfh_remove(adt_rbfh_t* self, uint8_t* u8Data)
{
   uint8_t* u8EndPtr = self->u8AllocBuf + (self->u16AllocLen * ((uint32_t)self->u8ElemSize));

   if (self->u16NumElem == 0)
   {
      return BUF_E_UNDERFLOW;
   }
   memcpy(u8Data, self->u8ReadPtr, (size_t) self->u8ElemSize);
   self->u8ReadPtr+=self->u8ElemSize;
   if (self->u8ReadPtr == u8EndPtr)
   {
      self->u8ReadPtr = self->u8AllocBuf;
   }
   self->u16NumElem--;
   return BUF_E_OK;
}

adt_buf_err_t adt_rbfh_peek(const adt_rbfh_t* self, uint8_t* u8Data)
{

   if (self->u16NumElem == 0)
   {
      return BUF_E_UNDERFLOW;
   }

   memcpy(u8Data, self->u8ReadPtr, (size_t) self->u8ElemSize);

   return BUF_E_OK;
}

uint16_t adt_rbfh_length(const adt_rbfh_t* self)
{
   if (self != 0)
   {
      return self->u16NumElem;
   }
   return 0;
}

uint16_t adt_rbfh_free(const adt_rbfh_t* self)
{
   if (self != 0)
   {
      return USHRT_MAX - self->u16NumElem;
   }
   return 0;
}

void adt_rbfh_clear(adt_rbfh_t* self)
{
   if (self != 0)
   {
      self->u8ReadPtr = self->u8WritePtr = self->u8AllocBuf;
      self->u16NumElem = 0;
   }
}

#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
#if(ADT_RBFSH_ENABLE)
static adt_buf_err_t adt_rbfh_grow(adt_rbfh_t* self)
{
   if (self->u16AllocLen == USHRT_MAX)
   {
      return BUF_E_OVERFLOW;
   }
   else
   {
      uint8_t *newAllocBuf;
      uint16_t newAllocLen = adt_rbfh_nextLen(self->u16AllocLen+1);
      newAllocBuf = (uint8_t*) malloc( ((uint32_t)self->u8ElemSize)*newAllocLen);
      if (newAllocBuf != 0)
      {
         adt_rbfh_swapBuffers(self, newAllocBuf, newAllocLen);
         return BUF_E_OK;
      }
   }
   return BUF_E_NOT_OK;
}


/**
 * When a ring buffer is full both the read pointer and the write pointer are at the same place
 * Copying all data to a new buffer is done in two steps.
 * 1. Copy all data from the read pointer up to the end of buffer
 * 2. Copy all the data from the beginning of the buffer up to the read pointer
 */
static void adt_rbfh_swapBuffers(adt_rbfh_t* self, uint8_t *newAllocBuf, uint16_t newAllocLen)
{
   if ( (self->u8AllocBuf == 0) || (self->u8ReadPtr == 0) || (self->u8WritePtr == 0))
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
      uint8_t *u8EndPtr = self->u8AllocBuf + (self->u16AllocLen * ((uint32_t)self->u8ElemSize));
      //Step 1.
      copyLen = (uint32_t) (u8EndPtr-self->u8ReadPtr);
      memcpy(newAllocBuf, self->u8ReadPtr, copyLen);
      mark = newAllocBuf+copyLen;

      //Step 2.
      copyLen =  (uint32_t) (self->u8AllocBuf - self->u8ReadPtr);
      if (copyLen > 0)
      {
         memcpy(mark, self->u8AllocBuf, copyLen);
         mark+=copyLen;
      }

      //Update pointers and free memory
      if (self->u8AllocBuf != 0)
      {
         free(self->u8AllocBuf);
      }
      self->u8AllocBuf = newAllocBuf;
      self->u16AllocLen = newAllocLen;
      self->u8ReadPtr = newAllocBuf;
      self->u8WritePtr = mark;
   }
}

DYN_STATIC uint16_t adt_rbfh_nextLen(uint16_t wanted)
{
   static const uint16_t accepted[NUM_ACCEPTED_LENGTHS] = {10u, 50u, 100u, 500u, 1000u, 5000u, 10000u, 50000u};
   uint8_t i;
   for(i=0u; i<NUM_ACCEPTED_LENGTHS; i++)
   {
      if (accepted[i] >= wanted)
      {
         return accepted[i];
      }
   }
   return USHRT_MAX;
}
#endif
