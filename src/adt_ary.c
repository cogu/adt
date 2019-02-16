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
static void adt_block_memmove(void *pDest, void *pSrc, uint32_t u32Remain);
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

void adt_ary_destructor_enable(adt_ary_t *self, bool enable){
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

/**
 * Removes the first occurrence of pElem from the array
 */
adt_error_t adt_ary_remove(adt_ary_t *self, void *pElem)
{
   if (self != 0)
   {
      int32_t s32Index;
      adt_error_t result = ADT_NO_ERROR;
      for(s32Index = 0; s32Index < self->s32CurLen; s32Index++ )
      {
         if (self->pFirst[s32Index] == pElem){
            result = adt_ary_splice(self, s32Index, 1);
            break;
         }
      }
      return result;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

/**
 * Appends pElem to the end of the array
 */
adt_error_t	adt_ary_push(adt_ary_t *self, void *pElem){
   if (self != 0) {
      int32_t s32Index;
      adt_error_t result;
      assert( (self != 0) && (pElem != 0));
      s32Index = self->s32CurLen;
      assert(self->s32CurLen < INT32_MAX);
      result = adt_ary_extend(self,((int32_t) s32Index+1));
      if (result == ADT_NO_ERROR) {
         self->pFirst[s32Index]=pElem;
      }
      return result;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

/**
 * Same as adt_ary_push but skips adding if it already exists.
 * Runs in linear time.
 */
adt_error_t adt_ary_push_unique(adt_ary_t *self, void *pElem){
   if (self != 0)
   {
      int32_t s32Index;
      for(s32Index = 0; s32Index < self->s32CurLen; s32Index++ )
      {
         if (self->pFirst[s32Index] == pElem){
            return ADT_NO_ERROR;
         }
      }
      return adt_ary_push(self, pElem);
   }
   return ADT_INVALID_ARGUMENT_ERROR;
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

/**
 * Removes the first element from the array
 */
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

/**
 * Appends pElem to the beginning of the array, shifting all other elements one step forward
 */
adt_error_t	adt_ary_unshift(adt_ary_t *self, void *pElem){
   if ( self != 0 ) {
      if (self->s32CurLen == INT32_MAX) {
         return ADT_LENGTH_ERROR; //cannot fit any more elements into the array
      }
      if(self->pFirst > self->ppAlloc){
         //room for one more element at the beginning
         *(--self->pFirst)=pElem;
         self->s32CurLen++;
         return ADT_NO_ERROR;
      }
      else {
         //no room at beginning of array, move all array data forward by one
         uint8_t *pBegin,*pEnd;
         adt_error_t result;
         uint32_t u32ElemSize = sizeof(void**);
         result = adt_ary_extend(self,(int32_t) (self->s32CurLen+1));
         if (result == ADT_NO_ERROR) {
            uint32_t u32Remain;
            pBegin = (uint8_t*) self->pFirst+u32ElemSize;
            pEnd = ((uint8_t*) &self->pFirst[self->s32CurLen]);
            u32Remain = (uint32_t) (pEnd-pBegin);
            adt_block_memmove(pBegin, self->pFirst, u32Remain);
            self->pFirst[0]=pElem;
         }
         return result;
      }
   }
	return ADT_INVALID_ARGUMENT_ERROR;
}


//Utility functions
adt_error_t	adt_ary_extend(adt_ary_t *self, int32_t s32Len){
   if (self != 0) {
      void **ppAlloc;
      //check if current length is greater than requested length
      if( self->s32CurLen>=s32Len ) return ADT_NO_ERROR;

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
            return ADT_LENGTH_ERROR;
         }
         ppAlloc = (void**) malloc(ELEM_LEN*((unsigned int)s32Len));
         if (ppAlloc == 0)
         {
            return ADT_MEM_ERROR;
         }
         if(self->ppAlloc){
            size_t numNewElems = (size_t) (s32Len - self->s32CurLen);
            memset(ppAlloc+self->s32CurLen, 0,  numNewElems * ELEM_LEN);
            memcpy(ppAlloc,self->pFirst, ((unsigned int)self->s32CurLen) * ELEM_LEN);
            free(self->ppAlloc);
         }
         self->ppAlloc = self->pFirst = ppAlloc;
         self->s32AllocLen = self->s32CurLen = s32Len;
      }
      return ADT_NO_ERROR;
   }
	return ADT_INVALID_ARGUMENT_ERROR;
}

adt_error_t adt_ary_fill(adt_ary_t *self, int32_t s32Len){
   if (self != 0) {
      int32_t s32Index;
      int32_t s32CurLen = self->s32CurLen;
      adt_error_t result;
      if(s32CurLen >= s32Len) {
         return ADT_NO_ERROR; //increase not necessary
      }
      result = adt_ary_extend(self,(int32_t)s32Len);
      if (result != ADT_NO_ERROR) {
         return result;
      }
      //set pFillElem to all newly created array elements
      for(s32Index=s32CurLen; s32Index<s32Len; s32Index++){
         self->pFirst[s32Index]=self->pFillElem;
      }
      assert(self->s32CurLen>=s32Len);
      assert(s32Index==self->s32CurLen);
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

adt_error_t adt_ary_resize(adt_ary_t *self, int32_t s32Len)
{
   if ( (self != 0) && (s32Len >= 0) ) {
      adt_error_t result = adt_ary_fill(self,s32Len);
      if (result != ADT_NO_ERROR) {
         return result;
      }
      if (s32Len < self->s32CurLen) {
         int32_t delta = self->s32CurLen-s32Len;
         result = adt_ary_splice(self,s32Len,delta);
      }
      return result;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
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
	return -1;
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
	return -1;
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

void adt_ary_set_fill_elem(adt_ary_t *self,void* pFillElem){
	if(self){
		self->pFillElem = pFillElem;
	}
}
void* adt_ary_get_fill_elem(const adt_ary_t *self){
	if(self){
		return self->pFillElem;
	}
	return (void*)0;
}

/**
 * removes s32Len items from array starting from s32Index (s32Index can be both positive or negative)
 */
adt_error_t adt_ary_splice(adt_ary_t *self,int32_t s32Index, int32_t s32Len){
   if ((self!=0) && (s32Len > 0)) {
      int32_t s32Source;
      int32_t s32Destination;
      int32_t i;
      int32_t s32ElemsRemain;


      if(s32Index<0){
         s32Index = (-s32Index);
         if(s32Index > (self->s32CurLen) ){
            return ADT_INDEX_OUT_OF_BOUNDS_ERROR;
         }
         s32Index=self->s32CurLen-s32Index;
      }
      else if(s32Index >= self->s32CurLen){
         return ADT_INDEX_OUT_OF_BOUNDS_ERROR;
      }

      s32Destination = s32Index;
      s32Source = s32Index + s32Len;

      if (s32Source > self->s32CurLen) {
         return ADT_LENGTH_ERROR; //s32Len is too large
      }

      //call destructor on elements about to be removed
      if( (self->destructorEnable != 0) && (self->pDestructor != 0) ){
         for (i=0;i<s32Len; i++) {
            self->pDestructor(self->pFirst[s32Destination+i]);
         }
      }
      s32ElemsRemain = self->s32CurLen-s32Source;
      if (s32ElemsRemain > 0) {
         uint32_t u32BytesRemain;
         u32BytesRemain = ((uint32_t)s32ElemsRemain)*((uint32_t) sizeof(void*));
         adt_block_memmove(&self->pFirst[s32Destination], &self->pFirst[s32Source], u32BytesRemain);
      }
      self->s32CurLen-=s32Len;
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;

}

/***************** Private Function Definitions *******************/
/**
 * CG: I had some serious issues with some Microsoft compilers not handling large memmoves.
 * To mitigate this potential problem I use this function to transform one large memmoves into a series of smaller memmoves.
 */
static void adt_block_memmove(void *pDest, void *pSrc, uint32_t u32Remain){
   while(u32Remain>0){
      uint32_t u32Size = (u32Remain>DATA_BLOCK_MAX)? DATA_BLOCK_MAX : u32Remain;
      memmove(pDest, pSrc, u32Size);
      u32Remain-=u32Size;
      pDest+=u32Size;
      pSrc+=u32Size;
   }
}
