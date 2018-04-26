/*****************************************************************************
* \file:   		adt_u16Map.h
* \author:		Conny Gustafsson
* \date:		2013-10-01
* \brief:		key/value map table using a sorted array as storage container
* \details		https://github.com/cogu/adt
*
* Copyright (c) 2013-2014 Conny Gustafsson
*
******************************************************************************/
#ifndef ADT_U16MAP_H
#define ADT_U16MAP_H
#include <stdint.h>

typedef struct u16MapElem_tag{
   uint16_t key;
   void *val;
}adt_u16MapElem_t;

typedef struct u16Map_tag {
   adt_u16MapElem_t *pBegin;
   adt_u16MapElem_t *pEnd;
   adt_u16MapElem_t *pIter;
   uint16_t maxNumElem;
   uint16_t numElem;
   void (*pDestructor)(void*);
   uint8_t destructorEnable;
} adt_u16Map_t;

/***************** Public Function Declarations *******************/
void adt_u16Map_create(adt_u16Map_t *self, adt_u16MapElem_t *pArray, uint16_t maxNumElem, void (*pDestructor)(void*));
void adt_u16Map_destroy(adt_u16Map_t *self);
adt_u16Map_t *adt_u16Map_new(uint16_t maxNumElem,void (*pDestructor)(void*) );
void adt_u16Map_delete(adt_u16Map_t *self);
void adt_u16Map_destructorEnable(adt_u16Map_t *self,uint8_t enable);

void adt_u16Map_clear(adt_u16Map_t *self);
void adt_u16Map_insert(adt_u16Map_t *self, uint16_t key, const void *val);
void adt_u16Map_remove(adt_u16Map_t *self, const adt_u16MapElem_t *pElem);
adt_u16MapElem_t* adt_u16Map_find(adt_u16Map_t *self, uint16_t key);
adt_u16MapElem_t* adt_u16Map_findExact(adt_u16Map_t *self, uint16_t key, const void *val);
uint16_t adt_u16Map_size(const adt_u16Map_t *self);
adt_u16MapElem_t* adt_u16Map_iterInit(adt_u16Map_t *self, const adt_u16MapElem_t *pElem);
adt_u16MapElem_t* adt_u16Map_iterNext(adt_u16Map_t *self);
uint16_t adt_u16Map_moveElem(adt_u16Map_t *dest, adt_u16Map_t *src, uint16_t key);
void adt_u16Map_removeVal(adt_u16Map_t *self, const void *val);



#endif //ADT_U16MAP_H
