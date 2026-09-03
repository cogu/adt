/*****************************************************************************
* \file      adt_ringbuf.h
* \author    Conny Gustafsson
* \date      2013-12-19
* \brief     Ringbuffer data structure
*
* Copyright (c) 2013-2018 Conny Gustafsson
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
#ifndef ADT_RINGBUF_H__
#define ADT_RINGBUF_H__

#include <stdint.h>

#ifdef ADT_RBF_CFG
#include "adt_ringbuf_cfg.h"
#endif

#ifndef BUF_E_OK
#define BUF_E_OK        0
#endif
#ifndef BUF_E_NOT_OK
#define BUF_E_NOT_OK    1
#endif
#ifndef BUF_E_OVERFLOW
#define BUF_E_OVERFLOW  2
#endif
#ifndef BUF_E_UNDERFLOW
#define BUF_E_UNDERFLOW 3
#endif

typedef uint8_t adt_buf_err_t;

#ifndef ADT_RBFS_ENABLE
#define ADT_RBFS_ENABLE 0
#endif

#ifndef ADT_RBFU16_ENABLE
#define ADT_RBFU16_ENABLE 0
#endif

#ifndef ADT_RBFH_ENABLE
#define ADT_RBFH_ENABLE 0
#endif

//RBFS: Ringbuffers containing elements of equal size
#if(ADT_RBFS_ENABLE)
typedef struct adt_rbfs_tag   //ring buffer (fixed block size) -
{
   uint8_t* u8Buffer;
   uint8_t* u8WritePtr;
   uint8_t* u8ReadPtr;
   uint16_t u16MaxNumElem;
   uint16_t u16NumElem;
   uint8_t u8ElemSize;
} adt_rbfs_t;
#endif

//RBFU16 Ringbuffers containing elements of uint16_t
#if (ADT_RBFU16_ENABLE)
typedef struct adt_rbfu16_tag   //ring buffer (fixed block size) -
{
   uint16_t* u16Buffer;
   uint16_t* u16WritePtr;
   uint16_t* u16ReadPtr;
   uint16_t u16MaxNumElem;
   uint16_t u16NumElem;
} adt_rbfu16_t;
#endif

#if(ADT_RBFH_ENABLE)

#define ADT_RBFSH_MIN_NUM_ELEMS_DEFAULT 10 //minimum number of elements (1-65535)
#define ADT_RBFSH_MAX_NUM_ELEM_DEFAULT 65535 //maximum number of elements (1-65535)

typedef struct adt_rbfh_tag   //ring buffer (fixed block size) -
{
   uint8_t* u8AllocBuf; //automatically maintained on heap
   uint8_t* u8WritePtr;
   uint8_t* u8ReadPtr;
   uint16_t u16AllocLen;
   uint16_t u16MaxNumElem;
   uint16_t u16NumElem;
   uint8_t u8ElemSize;
} adt_rbfh_t;
#endif

/***************** Public Function Declarations *******************/
#if(ADT_RBFS_ENABLE)
uint8_t adt_rbfs_create(adt_rbfs_t* rbf, uint8_t* u8Buffer, uint16_t u32NumElem, uint8_t u8ElemSize);
uint8_t adt_rbfs_insert(adt_rbfs_t* rbf, const uint8_t* u8Data);
uint8_t adt_rbfs_remove(adt_rbfs_t* rbf, uint8_t* u8Data);
uint8_t adt_rbfs_peek(const adt_rbfs_t* rbf, uint8_t* u8Data);
uint16_t adt_rbfs_size(const adt_rbfs_t* rbf);
uint16_t adt_rbfs_free(const adt_rbfs_t* rbf);
void adt_rbfs_clear(adt_rbfs_t* rbf);
#endif //ADT_RBFS_ENABLE

#if(ADT_RBFU16_ENABLE)
uint8_t adt_rbfu16_create(adt_rbfu16_t* rbf, uint16_t* u16Buffer, uint16_t u16NumElem);
uint8_t adt_rbfu16_insert(adt_rbfu16_t* rbf, uint16_t u16Data);
uint8_t adt_rbfu16_remove(adt_rbfu16_t* rbf, uint16_t* u16Data);
uint8_t adt_rbfu16_peek(const adt_rbfu16_t* rbf, uint16_t* u16Data);
uint16_t adt_rbfu16_length(const adt_rbfu16_t* rbf);
#endif //ADT_RBFU16_ENABLE

#if(ADT_RBFH_ENABLE)
adt_buf_err_t adt_rbfh_create(adt_rbfh_t* self, uint8_t u8ElemSize);
adt_buf_err_t adt_rbfh_createEx(adt_rbfh_t* self, uint8_t u8ElemSize, uint16_t u16MinNumElems, uint16_t u16MaxNumElems);
void adt_rbfh_destroy(adt_rbfh_t* self);
adt_rbfh_t *adt_rbfh_new(uint8_t u8ElemSize);
adt_rbfh_t *adt_rbfh_newEx(uint8_t u8ElemSize, uint16_t u16MinNumElems, uint16_t u16MaxNumElems);
void adt_rbfh_delete(adt_rbfh_t* self);
adt_buf_err_t adt_rbfh_insert(adt_rbfh_t* self, const uint8_t* u8Data);
adt_buf_err_t adt_rbfh_remove(adt_rbfh_t* self, uint8_t* u8Data);
adt_buf_err_t adt_rbfh_peek(const adt_rbfh_t* self, uint8_t* u8Data);
uint16_t adt_rbfh_length(const adt_rbfh_t* self);
uint16_t adt_rbfh_free(const adt_rbfh_t* self);
void adt_rbfh_clear(adt_rbfh_t* self);

# ifdef UNIT_TEST
uint16_t adt_rbfh_nextLen(uint16_t wanted);
# endif
#endif //ADT_RBFH_ENABLE

#endif //ADT_RINGBUF_H__

