/*****************************************************************************
* \file:   		adt_array.c
* \author:		Conny Gustafsson
* \date:		2013-06-03
* \brief:		array data structure
*
* Copyright (c) 2013 Conny Gustafsson
*
******************************************************************************/
#include "adt_array.h"
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "CMemLeak.h"

#define DATA_BLOCK_MAX 65536 	//maximum amount of bytes that can be copied in memmmove is implementation specific,
								//use this define to control how many bytes shall be copied


/**************** Private Function Declarations *******************/


/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
adt_array_t*	adt_array_new(void (*pDestructor)(void*)){
	adt_array_t *this;
	if((this = malloc(sizeof(adt_array_t)))==(adt_array_t*)0){
		return (adt_array_t*)0;
	}
	adt_array_create(this,pDestructor);
	return this;
}
adt_array_t*	adt_array_make(void** ppElem, int32_t u32Len,void (*pDestructor)(void*)){
	adt_array_t *this;
	int32_t u32i;
	void **ppDest,**ppSrc;

	if(u32Len>= INT32_MAX){
		//array length too long
		return (adt_array_t*) 0;
	}

	this = adt_array_new(pDestructor);
	if(this==(adt_array_t*)0){
		return (adt_array_t*)0;
	}
	adt_array_extend(this,u32Len);
	ppDest=this->pFirst;
	ppSrc=ppElem;
	for(u32i=0;u32i<u32Len;u32i++){
		*(ppDest++) = *(ppSrc++);
	}
	return this;
}
void adt_array_delete(adt_array_t *this){
	if(this){
		adt_array_destroy(this);
		free(this);
	}
}

//Accessors
void**	adt_array_set(adt_array_t *this, int32_t s32Index, void *pElem){
	uint32_t u32Index;
	if(this==0){
		return (void**)0;
	}
	if(s32Index<0){
		u32Index = (uint32_t) (-s32Index);
		if(u32Index > (this->u32CurLen) ){
			//negative index outside array bounds
			return (void**) 0;
		}
		//negative index inside array bounds
		u32Index=this->u32CurLen-u32Index;
	}
	else{
		u32Index = (uint32_t) s32Index;
	}
	adt_array_fill(this,((int32_t) u32Index)+1);
	this->pFirst[u32Index]=pElem;
	return &this->pFirst[u32Index];
}
void**	adt_array_get(adt_array_t *this, int32_t s32Index){
	uint32_t u32Index;
	if(this==0){
		return (void**)0;
	}
	if(s32Index<0){
		u32Index = (int32_t) (-s32Index);
		if(u32Index > (this->u32CurLen) ){
			//negative index outside array bounds
			return (void**) 0;
		}
		//negative index inside array bounds
		u32Index=this->u32CurLen-u32Index;
	}
	else{
		u32Index = (int32_t) s32Index;
	}
	adt_array_fill(this,(int32_t) (u32Index+1));
	return &this->pFirst[u32Index];
}
void	adt_array_push(adt_array_t *this, void *pElem){
	int32_t u32Index;
	assert( (this != 0) && (pElem != 0));
	u32Index = this->u32CurLen;
	assert(this->u32CurLen < INT32_MAX);
	adt_array_extend(this,u32Index+1);
	this->pFirst[u32Index]=pElem;
}
void*	adt_array_pop(adt_array_t *this){
	void *pElem;
	assert( (this!=0) );
	if(this->u32CurLen==0){
		return (void*) 0;
	}
	pElem = this->pFirst[--this->u32CurLen];
	if(this->u32CurLen == 0){
		//reallign pFirst with pAlloc when buffer becomes empty
		this->pFirst = this->pAlloc;
	}
	return pElem;
}
void*	adt_array_shift(adt_array_t *this){
	void *pElem;
	assert( (this!=0) );
	if(this->u32CurLen==0){
		return (void*)0;
	}
	pElem=*(this->pFirst++); //move pFirst forward by 1
	this->u32CurLen--; //reduce array length by 1
	if(this->u32CurLen == 0){
		//reallign pFirst with pAlloc when buffer becomes empty
		this->pFirst = this->pAlloc;
	}
	return pElem;
}
void	adt_array_unshift(adt_array_t *this, void *pElem){
	assert( (this != 0) && (pElem != 0));
	assert(this->u32CurLen < INT32_MAX);
	if(this->pFirst > this->pAlloc){
		//room for one more element at the beginning
		*(--this->pFirst)=pElem;
		this->u32CurLen++;
	}
	else{
		//no room at beginning of array, move all array data forward by one
		int32_t u32Remain,u32ElemSize;
		uint8_t *pBegin,*pEnd;
		u32ElemSize = sizeof(void**);
		adt_array_extend(this,this->u32CurLen+1);
		pBegin = (uint8_t*) this->pFirst+u32ElemSize;
		pEnd = ((uint8_t*) &this->pFirst[this->u32CurLen])-1;
		u32Remain = pEnd-pBegin+1;
		while(u32Remain>0){
			int32_t u32Size = (u32Remain>DATA_BLOCK_MAX)? DATA_BLOCK_MAX : u32Remain;
			memmove(pBegin,pBegin-u32ElemSize,u32Size);
			u32Remain-=u32Size;
			pBegin+=u32Size;
		}
		this->pFirst[0]=pElem;
	}
}


//Utility functions
void	adt_array_extend(adt_array_t *this, int32_t s32Len){
	void **ppAlloc;
	uint32_t u32Len = (uint32_t) s32Len;
	assert(this);
	//check if current length is greater than requested length
	if( this->u32CurLen>=u32Len ) return;

	//check if allocated length is greater than requested length
	if( (this->u32AllocLen>=u32Len) ){
		//shift array data to start of allocated array
		memmove(this->pAlloc,this->pFirst,this->u32CurLen * sizeof(void*));
		this->u32CurLen = u32Len;
	}
	else {
		//need to allocate new array data element and copy data to newly allocated memory
		if(u32Len>= INT32_MAX){
			//invalid argumment
			return;
		}
		ppAlloc = (void**) malloc(sizeof(void*)*u32Len);
		assert(ppAlloc != 0);
		if(this->pAlloc){
			memset(ppAlloc,0,this->u32CurLen * sizeof(void*));
			memcpy(ppAlloc,this->pFirst,this->u32CurLen * sizeof(void*));
			free(this->pAlloc);
		}
		this->pAlloc = this->pFirst = ppAlloc;
		this->u32AllocLen = this->u32CurLen = u32Len;
	}
}
void adt_array_fill(adt_array_t *this, int32_t s32Len){
	int32_t u32Index;
	uint32_t u32Len = (uint32_t) s32Len;
	uint32_t u32CurLen = this->u32CurLen;
	assert( this );
	if(this->u32CurLen >= u32Len) return; //increase not necessary
	adt_array_extend(this,u32Len);
	//set undef to all newly created array elements
	for(u32Index=u32CurLen; u32Index<u32Len; u32Index++){
		this->pFirst[u32Index]=this->pFillElem;
	}
	assert(this->u32CurLen>=u32Len);
	assert(u32Index==this->u32CurLen);
}
void adt_array_clear(adt_array_t *this){
	if(this){
		adt_array_destroy(this);
	}
}

uint32_t adt_array_len(adt_array_t *this){
	if(this){
		return this->u32CurLen;
	}
	return 0;
}

//Returns nonzero if the element exists
int32_t	adt_array_exists(adt_array_t *this, int32_t s32Index){
	int32_t u32Index;
	if(this==0){
		return 0;
	}
	if(s32Index<0){
		u32Index = (int32_t) (-s32Index);
		if(u32Index > (this->u32CurLen) ){
			//negative index outside array bounds
			return 0;
		}
		//negative index inside array bounds
		u32Index=this->u32CurLen-u32Index;
	}
	else{
		u32Index = (int32_t) s32Index;
	}
	if(u32Index<this->u32CurLen){
		return 1;
	}
	return 0;
}

void adt_array_create(adt_array_t *this,void (*pDestructor)(void*)){
	this->pAlloc = (void**) 0;
	this->pFirst = (void**) 0;
	this->u32AllocLen = 0;
	this->u32CurLen = 0;
	this->pDestructor = pDestructor;
	this->pFillElem = (void*)0;
}

void adt_array_destroy(adt_array_t *this){
	int32_t s32i;

	void **ppElem=this->pFirst;
	if(this->pDestructor){
		for(s32i=0;s32i<this->u32CurLen;s32i++){
			this->pDestructor(*(ppElem++));
		}
	}
	if(this->pAlloc != 0){
		free(this->pAlloc);
	}
	this->pAlloc = (void**) 0;
	this->u32AllocLen = 0;
	this->pFirst = (void**) 0;
	this->u32CurLen = 0;
}

void 	adt_array_set_fill_elem(adt_array_t *this,void* pFillElem){
	if(this){
		this->pFillElem = pFillElem;
	}
}
void* 	adt_array_get_fill_elem(adt_array_t *this){
	if(this){
		return this->pFillElem;
	}
	return (void*)0;
}


/***************** Private Function Definitions *******************/

