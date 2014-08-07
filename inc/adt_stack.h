/*****************************************************************************
* \file:   		adt_stack.h
* \author:		Conny Gustafsson
* \date:		2013-07-21
* \brief:		stack data structure
*
* Copyright (c) 2013 Conny Gustafsson
*
******************************************************************************/
#ifndef ADT_STACK_H__
#define ADT_STACK_H__
#include <stdint.h>

#define ADT_STACK_VERSION 0.1.0

typedef struct adt_stack_tag{
	void **ppAlloc;		//array of (void*)
	uint32_t u32AllocLen; //number of elements allocated
	uint32_t u32CurLen;	//number of elements currently in the stack
	uint32_t u32MinLen; //minimum (reserved) number of items in stack
	void (*pDestructor)(void*);
} adt_stack_t;

/***************** Public Function Declarations *******************/
//Constructor/Destructor
adt_stack_t*	adt_stack_new(void (*pDestructor)(void*));
void 			adt_stack_delete(adt_stack_t *this);
void 	adt_stack_create(adt_stack_t *this, void (*pDestructor)(void*));
void 	adt_stack_destroy(adt_stack_t *this);
void 	adt_stack_clear(adt_stack_t *this);


//Accessors
void	adt_stack_push(adt_stack_t *this, void *pVal);
void*	adt_stack_top(adt_stack_t *this);
void*	adt_stack_pop(adt_stack_t *this);

//Utility functions
void	adt_stack_reserve(adt_stack_t *this,uint32_t u32Len);
void	adt_stack_resize(adt_stack_t *this,uint32_t u32Len);
uint32_t adt_stack_size(adt_stack_t *this);

#endif //ADT_STACK_H__
