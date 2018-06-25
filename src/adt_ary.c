/*****************************************************************************
* \author:     Conny Gustafsson
* \date:       2013-06-03
* \brief:      array data structure
* \details     https://github.com/cogu/adt
*
* Copyright (c) 2013-2016 Conny Gustafsson
*
******************************************************************************/
#include "adt_ary.h"
#include <malloc.h>
#include <assert.h>
#include <string.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


#define DATA_BLOCK_MAX 65536 	//maximum amount of bytes that can be copied in memmmove is implementation specific,
								      //use define to control how many bytes shall be copied

#define ELEM_LEN (sizeof(void*))

/**************** Private Function Declarations *******************/

/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
adt_ary_t*	adt_ary_new(void (*pDestructor)(void*)){
	adt_ary_t *self;
	if((self = malloc(sizeof(adt_ary_t)))==(adt_ary_t*)0){
		return (adt_ary_t*)0;
	}
	adt_ary_create(self,pDestructor);
	return self;
}
adt_ary_t*	adt_ary_make(void** ppElem, int32_t s32Len,void (*pDestructor)(void*)){
	adt_ary_t *self;
	int32_t s32i;
	void **ppDest,**ppSrc;

	if(s32Len>= INT32_MAX){
		//array length too long
		return (adt_ary_t*) 0;
	}

	self = adt_ary_new(pDestructor);
	if(self==(adt_ary_t*)0){
		return (adt_ary_t*)0;
	}
	adt_ary_extend(self,s32Len);
	ppDest=self->pFirst;
	ppSrc=ppElem;
	for(s32i=0;s32i<s32Len;s32i++){
		*(ppDest++) = *(ppSrc++);
	}
	return self;
}
void adt_ary_delete(adt_ary_t *self){
	if(self != 0){
		adt_ary_destroy(self);
		free(self);
	}
}

void  adt_ary_vdelete(void *arg)
{
   adt_ary_delete((adt_ary_t*) arg);
}

void adt_ary_destructorEnable(adt_ary_t *self, bool enable){
   if(self != 0){
      self->destructorEnable = enable;
   }
}

//Accessors
void**	adt_ary_set(adt_ary_t *self, int32_t s32Index, void *pElem){
	if(self==0){
		return (void**)0;
	}
	if(s32Index<0){
		s32Index = (-s32Index);
		if(s32Index > (self->s32CurLen) ){
			//negative index outside array bounds
			return (void**) 0;
		}
		//negative index inside array bounds
		s32Index=self->s32CurLen-s32Index;
	}
	adt_ary_fill(self,((int32_t) s32Index)+1);
	self->pFirst[s32Index]=pElem;
	return &self->pFirst[s32Index];
}

/**
 * This returns pointer to pointer, make sure to dereference the result.
 * It returns NULL in case a negative index is out of range
 */
void**	adt_ary_get(adt_ary_t *self, int32_t s32Index){
	if(self==0){
		return (void**)0;
	}
	if(s32Index<0){
		s32Index = (-s32Index);
		if(s32Index > (self->s32CurLen) ){
			//negative index outside array bounds
			return (void**) 0;
		}
		//negative index inside array bounds
		s32Index=self->s32CurLen-s32Index;
	}
	adt_ary_fill(self,(int32_t) (s32Index+1));
	return &self->pFirst[s32Index];
}

/**
 * If you are certain that the index is going to be within bounds, you can use
 * this convenience function for getting the value without additional pointer dereference.
 */
void* adt_ary_value(const adt_ary_t *self, int32_t s32Index)
{
	if(self==0){
		return (void*) 0;
	}
	if(s32Index<0){
		//negative index inside array bounds ensured by caller
		s32Index=self->s32CurLen+s32Index;
	}
	return self->pFirst[s32Index];
}

void	adt_ary_push(adt_ary_t *self, void *pElem){
	int32_t s32Index;
	assert( (self != 0) && (pElem != 0));
	s32Index = self->s32CurLen;
	assert(self->s32CurLen < INT32_MAX);
	adt_ary_extend(self,((int32_t) s32Index+1));
	self->pFirst[s32Index]=pElem;
}

/**
 * Same as adt_ary_push but skips adding if it already exists.
 * Runs in linear time.
 */
void   adt_ary_push_unique(adt_ary_t *self, void *pElem){
   if (self != 0)
   {
      int32_t s32Index;
      for(s32Index = 0; s32Index < self->s32CurLen; s32Index++ )
      {
         if (self->pFirst[s32Index] == pElem){
            return;
         }
      }
      adt_ary_push(self, pElem);
   }
}

void*	adt_ary_pop(adt_ary_t *self){
	void *pElem;
	assert( (self!=0) );
	if(self->s32CurLen==0){
		return (void*) 0;
	}
	pElem = self->pFirst[--self->s32CurLen];
	if(self->s32CurLen == 0){
		//reallign pFirst with pAlloc when buffer becomes empty
		self->pFirst = self->ppAlloc;
	}
	return pElem;
}
void*	adt_ary_shift(adt_ary_t *self){
	void *pElem;
	assert( (self!=0) );
	if(self->s32CurLen==0){
		return (void*)0;
	}
	pElem=*(self->pFirst++); //move pFirst forward by 1
	self->s32CurLen--; //reduce array length by 1
	if(self->s32CurLen == 0){
		//reallign pFirst with pAlloc when buffer becomes empty
		self->pFirst = self->ppAlloc;
	}
	return pElem;
}
void	adt_ary_unshift(adt_ary_t *self, void *pElem){
	assert( (self != 0) && (pElem != 0));
	assert(self->s32CurLen < INT32_MAX);
	if(self->pFirst > self->ppAlloc){
		//room for one more element at the beginning
		*(--self->pFirst)=pElem;
		self->s32CurLen++;
	}
	else{
		//no room at beginning of array, move all array data forward by one
		uint32_t u32Remain,u32ElemSize;
		uint8_t *pBegin,*pEnd;
		u32ElemSize = sizeof(void**);
		adt_ary_extend(self,(int32_t) (self->s32CurLen+1));
		pBegin = (uint8_t*) self->pFirst+u32ElemSize;
		pEnd = ((uint8_t*) &self->pFirst[self->s32CurLen])-1;
		u32Remain = (uint32_t) (pEnd-pBegin+1);
		while(u32Remain>0){
			uint32_t u32Size = (u32Remain>DATA_BLOCK_MAX)? DATA_BLOCK_MAX : u32Remain;
			memmove(pBegin,pBegin-u32ElemSize,u32Size);
			u32Remain-=u32Size;
			pBegin+=u32Size;
		}
		self->pFirst[0]=pElem;
	}
}


//Utility functions
void	adt_ary_extend(adt_ary_t *self, int32_t s32Len){
	void **ppAlloc;
	assert(self);
	//check if current length is greater than requested length
	if( self->s32CurLen>=s32Len ) return;

	//check if allocated length is greater than requested length
	if( (self->s32AllocLen>=s32Len) ){
		//shift array data to start of allocated array
		memmove(self->ppAlloc,self->pFirst,((unsigned int)self->s32CurLen) * sizeof(void*));
		self->pFirst = self->ppAlloc;
		self->s32CurLen = s32Len;
	}
	else {
		//need to allocate new array data element and copy data to newly allocated memory
		if(s32Len>= INT32_MAX){
			//invalid argument
			return;
		}
		ppAlloc = (void**) malloc(ELEM_LEN*((unsigned int)s32Len));
		assert(ppAlloc != 0);
		if(self->ppAlloc){
         size_t numNewElems = (size_t) (s32Len - self->s32CurLen);
         memset(ppAlloc+self->s32CurLen, 0,  numNewElems * ELEM_LEN);
         memcpy(ppAlloc,self->pFirst, ((unsigned int)self->s32CurLen) * ELEM_LEN);
         free(self->ppAlloc);
		}
      self->ppAlloc = self->pFirst = ppAlloc;
      self->s32AllocLen = self->s32CurLen = s32Len;
	}
}

void adt_ary_fill(adt_ary_t *self, int32_t s32Len){
	assert( self != 0);
	int32_t s32Index;
	int32_t s32CurLen = self->s32CurLen;
	if(s32CurLen >= s32Len) return; //increase not necessary
	adt_ary_extend(self,(int32_t)s32Len);
	//set pFillElem to all newly created array elements
	for(s32Index=s32CurLen; s32Index<s32Len; s32Index++){
		self->pFirst[s32Index]=self->pFillElem;
	}
	assert(self->s32CurLen>=s32Len);
	assert(s32Index==self->s32CurLen);
}

void  adt_ary_resize(adt_ary_t *self, int32_t s32Len)
{
   if ( (self != 0) && (s32Len >= 0) )
   {
      adt_ary_fill(self,s32Len);
      if (s32Len < self->s32CurLen)
      {
         int32_t delta = self->s32CurLen-s32Len;
         adt_ary_splice(self,s32Len,delta);
      }
   }
}

void adt_ary_clear(adt_ary_t *self){
	if(self){
		adt_ary_destroy(self);
	}
}

int32_t adt_ary_length(const adt_ary_t *self){
	if(self){
		return self->s32CurLen;
	}
	return 0;
}

//Returns nonzero if the element exists
int32_t	adt_ary_exists(const adt_ary_t *self, int32_t s32Index){
	if(self==0){
		return 0;
	}
	if(s32Index<0){
		s32Index = (-s32Index);
		if(s32Index > (self->s32CurLen) ){
			//negative index outside array bounds
			return 0;
		}
		//negative index inside array bounds
		s32Index=self->s32CurLen-s32Index;
	}
	if(s32Index<self->s32CurLen){
		return 1;
	}
	return 0;
}

void adt_ary_create(adt_ary_t *self,void (*pDestructor)(void*)){
	self->ppAlloc = (void**) 0;
	self->pFirst = (void**) 0;
	self->s32AllocLen = 0;
	self->s32CurLen = 0;
	self->pDestructor = pDestructor;
	self->pFillElem = (void*)0;
	self->destructorEnable = true;
}

void adt_ary_destroy(adt_ary_t *self){
	int32_t s32i;

	void **ppElem=self->pFirst;
	if( (self->pDestructor != 0) && (self->destructorEnable != false) ){
		for(s32i=0;s32i<(int32_t) self->s32CurLen;s32i++){
			self->pDestructor(*(ppElem++));
		}
	}
	if(self->ppAlloc != 0){
		free(self->ppAlloc);
	}
	self->ppAlloc = (void**) 0;
	self->s32AllocLen = 0;
	self->pFirst = (void**) 0;
	self->s32CurLen = 0;
}

void 	adt_ary_set_fill_elem(adt_ary_t *self,void* pFillElem){
	if(self){
		self->pFillElem = pFillElem;
	}
}
void* 	adt_ary_get_fill_elem(const adt_ary_t *self){
	if(self){
		return self->pFillElem;
	}
	return (void*)0;
}

/**
 * removes s32Len items from array starting from s32Index (s32Index can be both positive or negative)
 */
void adt_ary_splice(adt_ary_t *self,int32_t s32Index, int32_t s32Len){
   int32_t s32Source;
   int32_t s32Destination;
   int32_t s32Removed = 0;
   int32_t i;
   if( (self==0) || (s32Len <= 0) ){
      return; //invalid input argument
   }
   if(s32Index<0){
      s32Index = (-s32Index);
      if(s32Index > (self->s32CurLen) ){
         return; //negative index out of bounds
      }
      //negative index inside array bounds
      s32Index=self->s32CurLen-s32Index;
   }
   if(s32Index >= self->s32CurLen){
      return;
   }
   s32Destination = s32Index;
   s32Source = s32Index + s32Len;
   for(i=0;i<s32Len;i++){
      if(s32Source < self->s32CurLen){
         //move item
         assert(s32Destination != s32Source);
         assert(s32Destination < self->s32CurLen);
         assert(s32Source < self->s32CurLen);
         if( (self->destructorEnable != 0) && (self->pDestructor != 0) ){
            self->pDestructor(self->pFirst[s32Destination]);
         }
         s32Removed++;
         self->pFirst[s32Destination] = self->pFirst[s32Source];
         self->pFirst[s32Source] = self->pFillElem;
      }
      else if(s32Destination < self->s32CurLen){
         //remove item
         if( (self->destructorEnable != 0) && (self->pDestructor != 0) ){
            self->pDestructor(self->pFirst[s32Destination]);
         }
         s32Removed++;
         self->pFirst[s32Destination] = self->pFillElem;
      }
      s32Source++;
      s32Destination++;
   }
   //move remaining items
   while(s32Source < self->s32CurLen){
      self->pFirst[s32Destination] = self->pFirst[s32Source];
      self->pFirst[s32Source] = self->pFillElem;
      s32Source++;
      s32Destination++;
   }
   self->s32CurLen-=s32Removed;
}

/***************** Private Function Definitions *******************/
