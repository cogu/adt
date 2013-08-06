/*****************************************************************************
* \file:   		adt_array.h
* \author:		Conny Gustafsson
* \date:		2013-06-03
* \brief:		array data structure
*
* Copyright (c) 2013 Conny Gustafsson
*
******************************************************************************/
#ifndef ADT_ARRAY_H__
#define ADT_ARRAY_H__
#include <stdint.h>

#define ADT_ARRAY_VERSION 0.1.0

/***************** Public Function Declarations *******************/
typedef struct adt_array_tag{
	void **pAlloc;		//array of (void*)
	void **pFirst;		//pointer to first array element
	uint32_t u32AllocLen;	//number of elements allocated
	uint32_t u32CurLen;	//number of elements currently in the array
	void (*pDestructor)(void*);
	void *pFillElem;
} adt_array_t;

/***************** Public Function Declarations *******************/
//Constructor/Destructor
adt_array_t*	adt_array_new(void (*pDestructor)(void*));
adt_array_t*	adt_array_make(void** ppElem, int32_t s32Len,void (*pDestructor)(void*));
void	adt_array_delete(adt_array_t *this);
void 	adt_array_create(adt_array_t *this, void (*pDestructor)(void*));
void 	adt_array_destroy(adt_array_t *this);


//Accessors
void**	adt_array_set(adt_array_t *this, int32_t s32Index, void *pElem);
void**	adt_array_get(adt_array_t *this, int32_t s32Index);
void	adt_array_push(adt_array_t *this, void *pElem);
void*	adt_array_pop(adt_array_t *this);
void*	adt_array_shift(adt_array_t *this);
void	adt_array_unshift(adt_array_t *this, void *pElem);


//Utility functions
void	adt_array_extend(adt_array_t *this, int32_t s32Len);
void	adt_array_fill(adt_array_t *this, int32_t s32Len);
void	adt_array_clear(adt_array_t *this);
uint32_t adt_array_len(adt_array_t *this);
void 	adt_array_set_fill_elem(adt_array_t *this,void* pFillElem);
void* 	adt_array_get_fill_elem(adt_array_t *this);

int32_t	adt_array_exists(adt_array_t *this, int32_t s32Index);

#endif //ADT_ARRAY_H__
