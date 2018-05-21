/*****************************************************************************
* \file:   		adt_ary.h
* \author:		Conny Gustafsson
* \date:		2013-06-03
* \brief:		array data structure
* \details		https://github.com/cogu/adt
*
* Copyright (c) 2013-2014 Conny Gustafsson
*
******************************************************************************/
#ifndef ADT_ARY_H
#define ADT_ARY_H
#include <stdint.h>

#define ADT_ARRAY_VERSION 0.1.1

typedef struct adt_ary_tag{
	void **ppAlloc;		//array of (void*)
	void **pFirst;		//pointer to first array element
	int32_t s32AllocLen;	//number of elements allocated
	int32_t s32CurLen;	//number of elements currently in the array
	void (*pDestructor)(void*);
	void *pFillElem;
	uint8_t destructorEnable;
} adt_ary_t;

/***************** Public Function Declarations *******************/
//Constructor/Destructor
adt_ary_t*	adt_ary_new(void (*pDestructor)(void*));
adt_ary_t*	adt_ary_make(void** ppElem, int32_t s32Len,void (*pDestructor)(void*));
void	adt_ary_delete(adt_ary_t *self);
void  adt_ary_vdelete(void *arg);
void 	adt_ary_create(adt_ary_t *self, void (*pDestructor)(void*));
void 	adt_ary_destroy(adt_ary_t *self);
void  adt_ary_destructorEnable(adt_ary_t *self, uint8_t enable);


//Accessors
void** adt_ary_set(adt_ary_t *self, int32_t s32Index, void *pElem);
void** adt_ary_get(adt_ary_t *self, int32_t s32Index);
void	 adt_ary_push(adt_ary_t *self, void *pElem);
void   adt_ary_push_unique(adt_ary_t *self, void *pElem);
void*	 adt_ary_pop(adt_ary_t *self);
void*	 adt_ary_shift(adt_ary_t *self);
void	 adt_ary_unshift(adt_ary_t *self, void *pElem);
void*  adt_ary_value(const adt_ary_t *self, int32_t s32Index);

//Utility functions
void	adt_ary_extend(adt_ary_t *self, int32_t s32Len);
void	adt_ary_fill(adt_ary_t *self, int32_t s32Len);
void  adt_ary_resize(adt_ary_t *self, int32_t s32Len);
void	adt_ary_clear(adt_ary_t *self);
int32_t adt_ary_length(const adt_ary_t *self);
void 	adt_ary_set_fill_elem(adt_ary_t *self,void* pFillElem);
void* 	adt_ary_get_fill_elem(const adt_ary_t *self);
void adt_ary_splice(adt_ary_t *self,int32_t s32Index, int32_t s32Len);

int32_t	adt_ary_exists(const adt_ary_t *self, int32_t s32Index);

#endif //ADT_ARY_H
