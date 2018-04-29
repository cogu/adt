/*****************************************************************************
* \file      ringbuf.c
* \author    Conny Gustafsson
* \date      2013-12-19
* \brief     Ringbuffer data structure
*
* Copyright (c) 2013-2017 Conny Gustafsson
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

#include "adt_ringbuf.h"


/**************** Private Function Declarations *******************/


/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
#if(ADT_RBFS_ENABLE)
//returns 0 on success
uint8_t adt_rbfs_create(adt_rbfs_t* u8Rbf, uint8_t* u8Buffer, uint16_t u16NumElem, uint8_t u8ElemSize)
{
   u8Rbf->u8Buffer = u8Buffer;
   u8Rbf->u8ReadPtr = u8Buffer;
   u8Rbf->u8WritePtr = u8Buffer;
   u8Rbf->u8ElemSize = u8ElemSize;
   u8Rbf->u16MaxNumElem = u16NumElem;
   u8Rbf->u16NumElem = 0;
   return E_BUF_OK;
}


//returns 0 on success, 1 on overflow, 2 on underflow
uint8_t adt_rbfs_insert(adt_rbfs_t* u8Rbf, const uint8_t* u8Data)
{
   uint8_t u8i;
   uint8_t* u8EndPtr = u8Rbf->u8Buffer + (u8Rbf->u16MaxNumElem * u8Rbf->u8ElemSize);
   
   if (u8Rbf->u16NumElem >= u8Rbf->u16MaxNumElem)
   {
      return E_BUF_OVERFLOW;
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
   
   return E_BUF_OK;
}


//returns 0 on success, 1 on overflow, 2 on underflow
uint8_t adt_rbfs_remove(adt_rbfs_t* u8Rbf, uint8_t* u8Data)
{
   uint8_t u8i;
   uint8_t* u8EndPtr = u8Rbf->u8Buffer + (u8Rbf->u16MaxNumElem * u8Rbf->u8ElemSize);
   
   if (u8Rbf->u16NumElem == 0)
   {
      return E_BUF_UNDERFLOW;
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
   return E_BUF_OK;
}


uint16_t adt_rbfs_size(const adt_rbfs_t* u8Rbf)
{
   return u8Rbf->u16NumElem;
}

uint16_t adt_rbfs_free(const adt_rbfs_t* rbf)
{
   return (uint16_t) (rbf->u16MaxNumElem-rbf->u16NumElem);
}

uint8_t adt_rbfs_peek(const adt_rbfs_t* rbf, uint8_t* u8Data)
{
   uint8_t u8i;
   uint8_t* u8ReadPtr = rbf->u8ReadPtr;
   
   if (rbf->u16NumElem == 0)
   {
      return E_BUF_UNDERFLOW;
   }
   
   for (u8i = 0; u8i < rbf->u8ElemSize; u8i++)
   {
      *(u8Data++) = *(u8ReadPtr++);
   }
   
   return E_BUF_OK;
}

void adt_rbfs_clear(adt_rbfs_t* rbf)
{
   rbf->u8ReadPtr = rbf->u8Buffer;
   rbf->u8WritePtr = rbf->u8Buffer;
   rbf->u16NumElem = 0;
}

#endif

#if(ADT_RBFD_ENABLE)
//returns 0 on success, 1 on overflow, 2 on underflow
uint8_t adt_rbfd_create(adt_rbfd_t* rbfd, uint8_t* u8Buffer, uint16_t u16BufferSize)
{
   rbfd->u8Buffer = u8Buffer;
   rbfd->u8ReadPtr = u8Buffer;
   rbfd->u8WritePtr = u8Buffer;
   rbfd->u16BufferSize = u16BufferSize;
   rbfd->u16BytesAvail = u16BufferSize;
   return E_BUF_OK;
}


//returns 0 on success, 1 on overflow, 2 on underflow
uint8_t adt_rbfd_insert(adt_rbfd_t* rbfd, uint8_t* u8Data, uint8_t u8Len)
{
   uint8_t u8i;
   uint8_t* u8EndPtr = rbfd->u8Buffer + rbfd->u16BufferSize;
   
   if (rbfd->u16BytesAvail < u8Len)
   {
      //overflow
      return 1;
   }
   
   for (u8i = 0; u8i < u8Len; u8i++)
   {
      *(rbfd->u8WritePtr++) = *(u8Data++);
      
      if (rbfd->u8WritePtr >= u8EndPtr)
      {
         //rewind
         rbfd->u8WritePtr = rbfd->u8Buffer;
      }
   }
   
   rbfd->u16BytesAvail -= u8Len;
   return E_BUF_OK;
}


//returns 0 on success, 1 on overflow, 2 on underflow
uint8_t adt_rbfd_remove(adt_rbfd_t* rbfd, uint8_t* u8Data, uint8_t u8Len)
{
   uint8_t u8i;
   uint8_t* u8EndPtr = rbfd->u8Buffer + rbfd->u16BufferSize;
   uint16_t u16BytesToRead = rbfd->u16BufferSize - rbfd->u16BytesAvail;
   
   if (u8Len > u16BytesToRead)
   {
      return E_BUF_UNDERFLOW;
   }
   
   for (u8i = 0; u8i < u8Len; u8i++)
   {
      *(u8Data++) = *(rbfd->u8ReadPtr++);
      
      if (rbfd->u8ReadPtr >= u8EndPtr)
      {
         //rewind
         rbfd->u8ReadPtr = rbfd->u8Buffer;
      }
   }
   
   rbfd->u16BytesAvail += u8Len;
   return E_BUF_OK;
}


uint16_t adt_rbfd_size(const adt_rbfd_t* rbfd)
{
   uint16_t u16BytesToRead = rbfd->u16BufferSize - rbfd->u16BytesAvail;
   return u16BytesToRead;
}


//returns 0 on success, 1 on overflow, 2 on underflow
uint8_t adt_rbfd_peekU8(const adt_rbfd_t* rbfd, uint8_t* u8Value)
{
   uint16_t u16BytesToRead = rbfd->u16BufferSize - rbfd->u16BytesAvail;
   
   if (u16BytesToRead < 1)
   {
      return E_BUF_UNDERFLOW;
   }
   
   *u8Value = *(rbfd->u8ReadPtr);
   return E_BUF_OK;
}


//returns 0 on success, 1 on overflow, 2 on underflow
uint8_t adt_rbfd_peekU16(const adt_rbfd_t* rbfd, uint16_t* u16Value)
{
   uint16_t u16BytesToRead = rbfd->u16BufferSize - rbfd->u16BytesAvail;
   
   if (u16BytesToRead < 2)
   {
      return E_BUF_UNDERFLOW;
   }
   
   #warning unaligned memory access in adt_rbfd_peekU16
   *u16Value = *((uint16_t*) rbfd->u8ReadPtr);
   return E_BUF_OK;
}


//returns 0 on success, 1 on overflow, 2 on underflow
uint8_t adt_rbfd_peekU32(const adt_rbfd_t* rbfd, uint32_t* u32Value)
{
   uint16_t u16BytesToRead = rbfd->u16BufferSize - rbfd->u16BytesAvail;
   
   if (u16BytesToRead < 4)
   {
      return E_BUF_UNDERFLOW;
   }
   
   #warning unaligned memory access in adt_rbfd_peekU32
   *u32Value = *((uint32_t*) rbfd->u8ReadPtr);
   return E_BUF_OK;
}
#endif

#if(ADT_RBFU16_ENABLE)
uint8_t adt_rbfu16_create(adt_rbfu16_t* rbf, uint16_t* u16Buffer, uint16_t u16NumElem){
   if ((rbf != 0) && (u16Buffer != 0) && (u16NumElem > 0)){
      rbf->u16Buffer = u16Buffer;
      rbf->u16MaxNumElem = u16NumElem;
      rbf->u16NumElem = 0;
      rbf->u16ReadPtr = u16Buffer;
      rbf->u16WritePtr = u16Buffer;
      return E_BUF_OK;
   }
   return E_BUF_NOT_OK;
}

uint8_t adt_rbfu16_insert(adt_rbfu16_t* rbf, uint16_t u16Data){
   if( rbf != 0 ){
      uint16_t *pEndPtr = rbf->u16Buffer + rbf->u16MaxNumElem;
      if(rbf->u16NumElem >= rbf->u16MaxNumElem){
         //full
         return E_BUF_OVERFLOW;
      }
      rbf->u16NumElem++;
      *rbf->u16WritePtr++ = u16Data;
      if (rbf->u16WritePtr >= pEndPtr)
      {
          //rewind
         rbf->u16WritePtr = rbf->u16Buffer;
      }
      return E_BUF_OK;
   }
   return E_BUF_NOT_OK;
}

uint8_t adt_rbfu16_remove(adt_rbfu16_t* rbf, uint16_t* u16Data){
   if( (rbf != 0) && (u16Data != 0) ){
      uint16_t *pEndPtr = rbf->u16Buffer + rbf->u16MaxNumElem;
      if(rbf->u16NumElem == 0){
         //full
         return E_BUF_UNDERFLOW;
      }
      *u16Data = *rbf->u16ReadPtr++;
      if (rbf->u16ReadPtr >= pEndPtr)
      {
          //rewind
         rbf->u16ReadPtr = rbf->u16Buffer;
      }
      rbf->u16NumElem--;
      return E_BUF_OK;
   }
   return E_BUF_NOT_OK;
}

uint8_t adt_rbfu16_peek(const adt_rbfu16_t* rbf, uint16_t* u16Data){
   if( (rbf != 0) && (u16Data != 0) ){
      if(rbf->u16NumElem == 0){
         //full
         return E_BUF_UNDERFLOW;
      }
      *u16Data = *rbf->u16ReadPtr;
      return E_BUF_OK;
   }
   return E_BUF_NOT_OK;
}

uint16_t adt_rbfu16_length(const adt_rbfu16_t* rbf){
   if(rbf != 0){
      return rbf->u16NumElem;
   }
   return 0;
}
#endif
