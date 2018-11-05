/*****************************************************************************
* \file      adt_ringbuf_heap.h
* \author    Conny Gustafsson
* \date      2018-10-17
* \brief     Description
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
#ifndef ADT_RINGBUF_HEAP_H
#define ADT_RINGBUF_HEAP_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

#define ADT_RBFSH_MIN_NUM_ELEMS_DEFAULT 10 //minimum number of elements (1-65535)
#define ADT_RBFSH_MAX_NUM_ELEM_DEFAULT 65535 //maximum number of elements (1-65535)

//RBFHS: Same as RBFS but automatically manages allocated data on the heap
#if(ADT_RBFSH_ENABLE)
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
//////////////////////////////////////////////////////////////////////////////
// PUBLIC VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
#if(ADT_RBFSH_ENABLE)
adt_buf_err_t adt_rbfh_create(adt_rbfh_t* self, uint8_t u8ElemSize);
adt_buf_err_t adt_rbfh_createEx(adt_rbfh_t* self, uint8_t u8ElemSize, uint16_t u16MinNumElems, uint16_t u16MaxNumElems);
void adt_rbfh_destroy(adt_rbfh_t* self);
adt_buf_err_t adt_rbfh_insert(adt_rbfh_t* self, const uint8_t* u8Data);
adt_buf_err_t adt_rbfh_remove(adt_rbfh_t* self, uint8_t* u8Data);
adt_buf_err_t adt_rbfh_peek(const adt_rbfh_t* self, uint8_t* u8Data);
uint16_t adt_rbfh_length(const adt_rbfh_t* self);
uint16_t adt_rbfh_free(const adt_rbfh_t* self);
void adt_rbfh_clear(adt_rbfh_t* self);

# ifdef UNIT_TEST
uint16_t adt_rbfh_nextLen(uint16_t wanted);
# endif
#endif

#endif //ADT_RINGBUF_HEAP_H
