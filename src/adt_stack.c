/*****************************************************************************
* \file:   		adt_stack.c
* \author:		Conny Gustafsson
* \date:		2013-07-21
* \brief:		stack data structure
*
* Copyright (c) 2013 Conny Gustafsson
*
******************************************************************************/
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "adt_stack.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif



#define ADT_STACK_DEFAULT_MIN_LEN 8

/**************** Private Function Declarations *******************/


/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/

//Constructor/Destructor
adt_stack_t* adt_stack_new(void (*pDestructor)(void*)){
	adt_stack_t *this;
	if((this = (adt_stack_t*) malloc(sizeof(adt_stack_t))) == 0){
		return (adt_stack_t*)0;
	}
	adt_stack_create(this,pDestructor);
	return this;
}

void adt_stack_delete(adt_stack_t *this){
	if(this){
		adt_stack_destroy(this);
		free(this);
	}
}

void adt_stack_create(adt_stack_t *this, void (*pDestructor)(void*)){
	this->ppAlloc = (void**) 0;
	this->u32AllocLen = 0;
	this->u32CurLen = 0;
	this->u32MinLen = ADT_STACK_DEFAULT_MIN_LEN;
	this->pDestructor = pDestructor;
}

void adt_stack_destroy(adt_stack_t *this){
	uint32_t u32i;

	void **ppElem=this->ppAlloc;
	if(this->pDestructor){
		for(u32i=0;u32i<this->u32CurLen;u32i++){
			this->pDestructor(*(ppElem++));
		}
	}
	if(this->ppAlloc != 0){
		free(this->ppAlloc);
	}
	this->ppAlloc = (void**) 0;
	this->u32AllocLen = 0;
	this->u32CurLen = 0;
}

void adt_stack_clear(adt_stack_t *this){
	adt_stack_destroy(this);
}


//Accessors
void	adt_stack_push(adt_stack_t *this, void *pVal){
	if(!this) return;
	if(this->u32CurLen==this->u32AllocLen){
		if(this->u32AllocLen==0){
			adt_stack_resize(this,this->u32MinLen);
		}
		else{
			//default growth by doubling items available
			adt_stack_resize(this,this->u32AllocLen*2);
		}
	}
	assert(this->u32CurLen<this->u32AllocLen);
	assert(this->ppAlloc != 0);
	this->ppAlloc[this->u32CurLen++] = pVal;
}
void* adt_stack_top(adt_stack_t *this){
	if(this && (this->u32CurLen>0)){
		assert(this->ppAlloc);
		return this->ppAlloc[this->u32CurLen-1];
	}
	return (void*)0;
}

void* adt_stack_pop(adt_stack_t *this){
	if(this && (this->u32CurLen>0)){
		assert(this->ppAlloc);
		return this->ppAlloc[--this->u32CurLen];
	}
	return (void*)0;
}

//Utility functions
void adt_stack_reserve(adt_stack_t *this,uint32_t u32Len){
	if(this){
		this->u32MinLen = u32Len;
		if(this->u32AllocLen < this->u32MinLen){
			adt_stack_resize(this,this->u32MinLen);
		}
	}
}

void adt_stack_resize(adt_stack_t *this,uint32_t u32Len){
	void **ppAlloc = (void**) 0;
	if(!this) return;
	if(u32Len == this->u32AllocLen) return; //nothing to do

	assert(this->u32AllocLen>=this->u32CurLen);
	if(u32Len>0){
		ppAlloc = (void**) malloc(u32Len * sizeof(void*));
		assert(ppAlloc != 0);
	}
	if(this->ppAlloc){
		if(u32Len > this->u32AllocLen){
			//grow
			memcpy(ppAlloc,this->ppAlloc,this->u32AllocLen * sizeof(void*));
		}
		else{
			//shrink
			if( this->u32CurLen > u32Len){
				//call destructor on superfluous elements
				if(this->pDestructor){
					uint32_t u32i;
					for(u32i=u32Len; u32i < this->u32CurLen;u32i++){
						this->pDestructor(this->ppAlloc[u32i]);
					}
				}
			}
			memcpy(ppAlloc,this->ppAlloc,u32Len * sizeof(void*));
		}
		free(this->ppAlloc);
	}
	this->ppAlloc = ppAlloc;
	this->u32AllocLen = u32Len;

}
uint32_t adt_stack_size(adt_stack_t *this){
	if(this){
		return this->u32CurLen;
	}
	return 0;
}


/***************** Private Function Definitions *******************/
