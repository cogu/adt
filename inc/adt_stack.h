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

#define ADT_STACK_VERSION 0.1.1

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
void 			adt_stack_delete(adt_stack_t *self);
void 	adt_stack_create(adt_stack_t *self, void (*pDestructor)(void*));
void 	adt_stack_destroy(adt_stack_t *self);
void 	adt_stack_clear(adt_stack_t *self);


//Accessors
void	adt_stack_push(adt_stack_t *self, void *pVal);
void*	adt_stack_top(const adt_stack_t *self);
void*	adt_stack_pop(adt_stack_t *self);

//Utility functions
void	adt_stack_reserve(adt_stack_t *self,uint32_t u32Len);
void	adt_stack_resize(adt_stack_t *self,uint32_t u32Len);
uint32_t adt_stack_size(const adt_stack_t *self);

#endif //ADT_STACK_H__
