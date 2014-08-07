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

#define ADT_ARRAY_VERSION 0.1.0

/***************** Public Function Declarations *******************/
typedef struct adt_ary_tag{
	void **ppAlloc;		//array of (void*)
	void **pFirst;		//pointer to first array element
	uint32_t u32AllocLen;	//number of elements allocated
	uint32_t u32CurLen;	//number of elements currently in the array
	void (*pDestructor)(void*);
	void *pFillElem;
} adt_ary_t;

/***************** Public Function Declarations *******************/
//Constructor/Destructor
adt_ary_t*	adt_ary_new(void (*pDestructor)(void*));
adt_ary_t*	adt_ary_make(void** ppElem, int32_t s32Len,void (*pDestructor)(void*));
void	adt_ary_delete(adt_ary_t *inst);
void 	adt_ary_create(adt_ary_t *inst, void (*pDestructor)(void*));
void 	adt_ary_destroy(adt_ary_t *inst);


//Accessors
void**	adt_ary_set(adt_ary_t *inst, int32_t s32Index, void *pElem);
void**	adt_ary_get(adt_ary_t *inst, int32_t s32Index);
void	adt_ary_push(adt_ary_t *inst, void *pElem);
void*	adt_ary_pop(adt_ary_t *inst);
void*	adt_ary_shift(adt_ary_t *inst);
void	adt_ary_unshift(adt_ary_t *inst, void *pElem);


//Utility functions
void	adt_ary_extend(adt_ary_t *inst, int32_t s32Len);
void	adt_ary_fill(adt_ary_t *inst, int32_t s32Len);
void	adt_ary_clear(adt_ary_t *inst);
uint32_t adt_ary_length(adt_ary_t *inst);
void 	adt_ary_set_fill_elem(adt_ary_t *inst,void* pFillElem);
void* 	adt_ary_get_fill_elem(adt_ary_t *inst);
void adt_ary_splice(adt_ary_t *inst,int32_t s32Index, int32_t s32Len);

int32_t	adt_ary_exists(adt_ary_t *inst, int32_t s32Index);

#endif //ADT_ARY_H
