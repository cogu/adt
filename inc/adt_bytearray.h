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

typedef struct bytearray_t{
   uint8_t *pData;
   uint32_t u32CurLen;
   uint32_t u32AllocLen;
   uint32_t u32GrowSize;
} adt_bytearray_t;

#define ADT_BYTE_ARRAY_DEFAULT_GROW_SIZE ((uint32_t)8192u)
#define ADT_BYTE_ARRAY_MAX_GROW_SIZE ((uint32_t)32u*1024u*1024u)

/***************** Public Function Declarations *******************/
void adt_bytearray_create(adt_bytearray_t *self,uint32_t u32GrowSize);
void adt_bytearray_destroy(adt_bytearray_t *self);
adt_bytearray_t *adt_bytearray_new(uint32_t u32GrowSize);
void adt_bytearray_delete(adt_bytearray_t *self);
void adt_bytearray_vdelete(void *arg);
int8_t adt_bytearray_reserve(adt_bytearray_t *self, uint32_t u32NewLen);
int8_t adt_bytearray_grow(adt_bytearray_t *self, uint32_t u32MinLen);
int8_t adt_bytearray_append(adt_bytearray_t *self, const uint8_t *pData, uint32_t u32DataLen);
int8_t adt_bytearray_trimLeft(adt_bytearray_t *self, const uint8_t *pSrc);
int8_t adt_bytearray_resize(adt_bytearray_t *self, uint32_t u32NewLen);
uint8_t *adt_bytearray_data(adt_bytearray_t *self);
uint32_t adt_bytearray_length(adt_bytearray_t *self);
void adt_bytearray_clear(adt_bytearray_t *self);

#endif //ADT_BYTE_ARRAY_H
