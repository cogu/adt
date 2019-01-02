/*****************************************************************************
* \file:    adt_bytearray.h
* \author:  Conny Gustafsson
* \date:    2015-02-05
* \brief:   general purpose data container
*
* Copyright (c) 2015-2016 Conny Gustafsson
*
******************************************************************************/
#ifndef ADT_BYTE_ARRAY_H
#define ADT_BYTE_ARRAY_H
#include <stdint.h>
#include "adt_error.h"
#if defined(_MSC_PLATFORM_TOOLSET) && (_MSC_PLATFORM_TOOLSET<=110)
#ifndef _MSC_BOOL_DEFINED
#define _MSC_BOOL_DEFINED
#define false 0
#define true 1
typedef uint8_t bool;
#endif
#else
#include <stdbool.h>
#endif


typedef struct bytearray_t{
   uint8_t *pData;
   uint32_t u32CurLen;
   uint32_t u32AllocLen;
   uint32_t u32GrowSize;
} adt_bytearray_t;

#define ADT_BYTE_ARRAY_VERSION 0.1.1
#define ADT_BYTE_ARRAY_DEFAULT_GROW_SIZE ((uint32_t)8192u)
#define ADT_BYTE_ARRAY_MAX_GROW_SIZE ((uint32_t)32u*1024u*1024u)

/***************** Public Function Declarations *******************/
void adt_bytearray_create(adt_bytearray_t *self,uint32_t u32GrowSize);
void adt_bytearray_destroy(adt_bytearray_t *self);
adt_bytearray_t *adt_bytearray_new(uint32_t u32GrowSize);
adt_bytearray_t *adt_bytearray_make(const uint8_t *pData, uint32_t u32DataLen, uint32_t u32GrowSize);
void adt_bytearray_delete(adt_bytearray_t *self);
void adt_bytearray_vdelete(void *arg);
adt_error_t adt_bytearray_reserve(adt_bytearray_t *self, uint32_t u32NewLen);
adt_error_t adt_bytearray_grow(adt_bytearray_t *self, uint32_t u32MinLen);
adt_error_t adt_bytearray_append(adt_bytearray_t *self, const uint8_t *pData, uint32_t u32DataLen);
adt_error_t adt_bytearray_trimLeft(adt_bytearray_t *self, const uint8_t *pSrc);
adt_error_t adt_bytearray_resize(adt_bytearray_t *self, uint32_t u32NewLen);
uint8_t *adt_bytearray_data(const adt_bytearray_t *self);
uint32_t adt_bytearray_length(const adt_bytearray_t *self);
void adt_bytearray_clear(adt_bytearray_t *self);
bool adt_bytearray_equals(const adt_bytearray_t *lhs, const adt_bytearray_t *rhs);


#endif //ADT_BYTE_ARRAY_H
