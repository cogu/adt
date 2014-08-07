/*****************************************************************************
* \file:   		adt_array.c
* \author:		Conny Gustafsson
* \date:		2013-06-03
* \brief:		array data structure
*
* Copyright (c) 2013 Conny Gustafsson
*
******************************************************************************/
#include "adt_ary.h"
#include <malloc.h>
#include <assert.h>
#include <string.h>
#ifdef _DEBUG
#include "CMemLeak.h"
#endif


#define DATA_BLOCK_MAX 65536 	//maximum amount of bytes that can be copied in memmmove is implementation specific,
								//use inst define to control how many bytes shall be copied


/**************** Private Function Declarations *******************/


/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
adt_ary_t*	adt_ary_new(void (*pDestructor)(void*)){
	adt_ary_t *inst;
	if((inst = malloc(sizeof(adt_ary_t)))==(adt_ary_t*)0){
		return (adt_ary_t*)0;
	}
	adt_ary_create(inst,pDestructor);
	return inst;
}
adt_ary_t*	adt_ary_make(void** ppElem, int32_t s32Len,void (*pDestructor)(void*)){
	adt_ary_t *inst;
	int32_t s32i;
	void **ppDest,**ppSrc;

	if(s32Len>= INT32_MAX){
		//array length too long
		return (adt_ary_t*) 0;
	}

	inst = adt_ary_new(pDestructor);
	if(inst==(adt_ary_t*)0){
		return (adt_ary_t*)0;
	}
	adt_ary_extend(inst,s32Len);
	ppDest=inst->pFirst;
	ppSrc=ppElem;
	for(s32i=0;s32i<s32Len;s32i++){
		*(ppDest++) = *(ppSrc++);
	}
	return inst;
}
void adt_ary_delete(adt_ary_t *inst){
	if(inst != 0){
		adt_ary_destroy(inst);
		free(inst);
	}
}

//Accessors
void**	adt_ary_set(adt_ary_t *inst, int32_t s32Index, void *pElem){
	uint32_t u32Index;
	if(inst==0){
		return (void**)0;
	}
	if(s32Index<0){
		u32Index = (uint32_t) (-s32Index);
		if(u32Index > (inst->u32CurLen) ){
			//negative index outside array bounds
			return (void**) 0;
		}
		//negative index inside array bounds
		u32Index=inst->u32CurLen-u32Index;
	}
	else{
		u32Index = (uint32_t) s32Index;
	}
	adt_ary_fill(inst,((int32_t) u32Index)+1);
	inst->pFirst[u32Index]=pElem;
	return &inst->pFirst[u32Index];
}
void**	adt_ary_get(adt_ary_t *inst, int32_t s32Index){
	uint32_t u32Index;
	if(inst==0){
		return (void**)0;
	}
	if(s32Index<0){
		u32Index = (int32_t) (-s32Index);
		if(u32Index > (inst->u32CurLen) ){
			//negative index outside array bounds
			return (void**) 0;
		}
		//negative index inside array bounds
		u32Index=inst->u32CurLen-u32Index;
	}
	else{
		u32Index = (int32_t) s32Index;
	}
	adt_ary_fill(inst,(int32_t) (u32Index+1));
	return &inst->pFirst[u32Index];
}
void	adt_ary_push(adt_ary_t *inst, void *pElem){
	int32_t u32Index;
	assert( (inst != 0) && (pElem != 0));
	u32Index = inst->u32CurLen;
	assert(inst->u32CurLen < INT32_MAX);
	adt_ary_extend(inst,u32Index+1);
	inst->pFirst[u32Index]=pElem;
}
void*	adt_ary_pop(adt_ary_t *inst){
	void *pElem;
	assert( (inst!=0) );
	if(inst->u32CurLen==0){
		return (void*) 0;
	}
	pElem = inst->pFirst[--inst->u32CurLen];
	if(inst->u32CurLen == 0){
		//reallign pFirst with pAlloc when buffer becomes empty
		inst->pFirst = inst->ppAlloc;
	}
	return pElem;
}
void*	adt_ary_shift(adt_ary_t *inst){
	void *pElem;
	assert( (inst!=0) );
	if(inst->u32CurLen==0){
		return (void*)0;
	}
	pElem=*(inst->pFirst++); //move pFirst forward by 1
	inst->u32CurLen--; //reduce array length by 1
	if(inst->u32CurLen == 0){
		//reallign pFirst with pAlloc when buffer becomes empty
		inst->pFirst = inst->ppAlloc;
	}
	return pElem;
}
void	adt_ary_unshift(adt_ary_t *inst, void *pElem){
	assert( (inst != 0) && (pElem != 0));
	assert(inst->u32CurLen < INT32_MAX);
	if(inst->pFirst > inst->ppAlloc){
		//room for one more element at the beginning
		*(--inst->pFirst)=pElem;
		inst->u32CurLen++;
	}
	else{
		//no room at beginning of array, move all array data forward by one
		int32_t u32Remain,u32ElemSize;
		uint8_t *pBegin,*pEnd;
		u32ElemSize = sizeof(void**);
		adt_ary_extend(inst,inst->u32CurLen+1);
		pBegin = (uint8_t*) inst->pFirst+u32ElemSize;
		pEnd = ((uint8_t*) &inst->pFirst[inst->u32CurLen])-1;
		u32Remain = pEnd-pBegin+1;
		while(u32Remain>0){
			uint32_t u32Size = (u32Remain>DATA_BLOCK_MAX)? DATA_BLOCK_MAX : u32Remain;
			memmove(pBegin,pBegin-u32ElemSize,u32Size);
			u32Remain-=u32Size;
			pBegin+=u32Size;
		}
		inst->pFirst[0]=pElem;
	}
}


//Utility functions
void	adt_ary_extend(adt_ary_t *inst, int32_t s32Len){
	void **ppAlloc;
	uint32_t u32Len = (uint32_t) s32Len;
	assert(inst);
	//check if current length is greater than requested length
	if( inst->u32CurLen>=u32Len ) return;

	//check if allocated length is greater than requested length
	if( (inst->u32AllocLen>=u32Len) ){
	  //shift array data to start of allocated array
      memmove(inst->ppAlloc,inst->pFirst,inst->u32CurLen * sizeof(uint16_t));
      inst->pFirst = inst->ppAlloc;
      inst->u32CurLen = u32Len;
	}
	else {
		//need to allocate new array data element and copy data to newly allocated memory
		if(u32Len>= INT32_MAX){
			//invalid argument
			return;
		}
		ppAlloc = (void**) malloc(sizeof(void*)*u32Len);
		assert(ppAlloc != 0);
		if(inst->ppAlloc){
			memset(ppAlloc,0,inst->u32CurLen * sizeof(void*));
			memcpy(ppAlloc,inst->pFirst,inst->u32CurLen * sizeof(void*));
			free(inst->ppAlloc);
		}
		inst->ppAlloc = inst->pFirst = ppAlloc;
		inst->u32AllocLen = inst->u32CurLen = u32Len;
	}
}
void adt_ary_fill(adt_ary_t *inst, int32_t s32Len){
	int32_t u32Index;
	uint32_t u32Len = (uint32_t) s32Len;
	uint32_t u32CurLen = inst->u32CurLen;
	assert( inst != 0);
	if(inst->u32CurLen >= u32Len) return; //increase not necessary
	adt_ary_extend(inst,u32Len);
	//set undef to all newly created array elements
	for(u32Index=u32CurLen; u32Index<u32Len; u32Index++){
		inst->pFirst[u32Index]=inst->pFillElem;
	}
	assert(inst->u32CurLen>=u32Len);
	assert(u32Index==inst->u32CurLen);
}
void adt_ary_clear(adt_ary_t *inst){
	if(inst){
		adt_ary_destroy(inst);
	}
}

uint32_t adt_ary_length(adt_ary_t *inst){
	if(inst){
		return inst->u32CurLen;
	}
	return 0;
}

//Returns nonzero if the element exists
int32_t	adt_ary_exists(adt_ary_t *inst, int32_t s32Index){
	int32_t u32Index;
	if(inst==0){
		return 0;
	}
	if(s32Index<0){
		u32Index = (int32_t) (-s32Index);
		if(u32Index > (inst->u32CurLen) ){
			//negative index outside array bounds
			return 0;
		}
		//negative index inside array bounds
		u32Index=inst->u32CurLen-u32Index;
	}
	else{
		u32Index = (int32_t) s32Index;
	}
	if(u32Index<inst->u32CurLen){
		return 1;
	}
	return 0;
}

void adt_ary_create(adt_ary_t *inst,void (*pDestructor)(void*)){
	inst->ppAlloc = (void**) 0;
	inst->pFirst = (void**) 0;
	inst->u32AllocLen = 0;
	inst->u32CurLen = 0;
	inst->pDestructor = pDestructor;
	inst->pFillElem = (void*)0;
}

void adt_ary_destroy(adt_ary_t *inst){
	int32_t s32i;

	void **ppElem=inst->pFirst;
	if(inst->pDestructor){
		for(s32i=0;s32i<inst->u32CurLen;s32i++){
			inst->pDestructor(*(ppElem++));
		}
	}
	if(inst->ppAlloc != 0){
		free(inst->ppAlloc);
	}
	inst->ppAlloc = (void**) 0;
	inst->u32AllocLen = 0;
	inst->pFirst = (void**) 0;
	inst->u32CurLen = 0;
}

void 	adt_ary_set_fill_elem(adt_ary_t *inst,void* pFillElem){
	if(inst){
		inst->pFillElem = pFillElem;
	}
}
void* 	adt_ary_get_fill_elem(adt_ary_t *inst){
	if(inst){
		return inst->pFillElem;
	}
	return (void*)0;
}

void adt_ary_splice(adt_ary_t *inst,int32_t s32Index, int32_t s32Len){
   uint32_t u32Index;
   uint32_t u32Source;
   uint32_t u32Destination;
   uint32_t u32Removed = 0;
   uint32_t i;
   if( (inst==0) || (s32Len <= 0) ){
      return; //invalid input argument
   }
   if(s32Index<0){
      u32Index = (int32_t) (-s32Index);
      if(u32Index > (inst->u32CurLen) ){
         return; //negative index out of bounds
      }
      //negative index inside array bounds
      u32Index=inst->u32CurLen-u32Index;
   }
   else{
      u32Index = (int32_t) s32Index;
   }
   if(u32Index >= inst->u32CurLen){
      return;
   }
   u32Destination = u32Index;
   u32Source = u32Index + ((uint32_t) s32Len);
   for(i=0;i<s32Len;i++){
      if(u32Source < inst->u32CurLen){
         //move item
         assert(u32Destination != u32Source);
         assert(u32Destination < inst->u32CurLen);
         assert(u32Source < inst->u32CurLen);
         if( inst->pDestructor != 0 ){
            u32Removed++;
            inst->pDestructor(inst->pFirst[u32Destination]);
         }
         inst->pFirst[u32Destination] = inst->pFirst[u32Source];
         inst->pFirst[u32Source] = inst->pFillElem;
      }
      else if(u32Destination < inst->u32CurLen){
         //remove item
         if( inst->pDestructor != 0 ){
            inst->pDestructor(inst->pFirst[u32Destination]);
         }
         u32Removed++;
         inst->pFirst[u32Destination] = inst->pFillElem;
      }
      u32Source++;
      u32Destination++;
   }
   //move remaining items
   while(u32Source < inst->u32CurLen){
      inst->pFirst[u32Destination] = inst->pFirst[u32Source];
      inst->pFirst[u32Source] = inst->pFillElem;
      u32Source++;
      u32Destination++;
   }
   inst->u32CurLen-=u32Removed;
}

/***************** Private Function Definitions *******************/

