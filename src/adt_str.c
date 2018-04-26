/*****************************************************************************
* \file:   		cstr.c
* \author:		Conny Gustafsson
* \date:		2013-05-31
* \brief:		string container
*
* Copyright 2013 Conny Gustafsson
*
******************************************************************************/
#include "adt_str.h"
#include <string.h>
#include <limits.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


/*
 * Each string starts out being 16 bytes in length
 * When self becomes too small, it quadruples in size (multiply by 4) until it reaches 64K.
 * It then doubles until it reaches 64MB. After that it does linear growth in chunks of 64MB.
 */
#define MIN_BLOCK_SIZE 		16
#define MEDIUM_BLOCK_SIZE 	65536
#define LARGE_BLOCK_SIZE 	67108864

/**************** Private Function Declarations *******************/
static int32_t adt_str_calcLen(int32_t s32CurLen, int32_t s32NewLen);


/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
/* constructor/destructor */
adt_str_t *adt_str_new(void){
	adt_str_t *self;
	if((self = (adt_str_t*) malloc(sizeof(adt_str_t))) == 0){
		return (adt_str_t*)0;
	}
	adt_str_create(self);
	return self;
}


void adt_str_delete(adt_str_t *self){
	if(self){
		adt_str_destroy(self);
		free(self);
	}
}

void adt_str_create(adt_str_t *self){
	if(self){
		self->s32Cur = 0;
		self->s32Len = 0;
		self->pStr = (uint8_t*)0;
	}
}
void adt_str_destroy(adt_str_t *self){
	if(self){
		if(self->pStr) free(self->pStr);
		self->pStr=0;
		self->s32Cur = 0;
		self->s32Len = 0;
	}
}


/* adt_str functions */

adt_str_t *adt_str_dup(const adt_str_t* other){
	adt_str_t *self;
	self = adt_str_new();
	if(self){
		adt_str_set(self,other);
	}
	return self;
}

adt_str_t *adt_str_make(const char *pBegin, const char *pEnd){
	adt_str_t *self;
	self = adt_str_new();
	if(self){
		adt_str_copy_range(self,pBegin,pEnd);
	}
	return self;
}

int32_t adt_str_length(const adt_str_t *self){
	if(self){
		return self->s32Cur;
	}
	return 0;
}

void adt_str_clear(adt_str_t *self){
	if(self == 0) return;
	self->s32Cur = 0;
}

void adt_str_set(adt_str_t *lhs, const adt_str_t *rhs){
	if( (lhs == 0) || (rhs == 0) ) return;
	if(lhs->pStr) free(lhs->pStr);
	lhs->pStr = (uint8_t*) malloc(rhs->s32Len);
	assert(lhs->pStr);
	memcpy(lhs->pStr,rhs->pStr,rhs->s32Len);
	lhs->s32Cur = rhs->s32Cur;
	lhs->s32Len = rhs->s32Len;
}

void adt_str_append(adt_str_t *lhs, const adt_str_t *rhs)
{
	// Not implemented yet..
	assert(0);
}

void adt_str_reserve(adt_str_t *self,int32_t s32Len){
   uint8_t *pStr;
   s32Len++; //reserve 1 byte for null terminator
	if(s32Len <= self->s32Len){
		return;
	}
	self->s32Len = adt_str_calcLen(self->s32Len,s32Len);
   pStr = (uint8_t*) malloc(self->s32Len);
	assert(pStr);
	if(self->pStr){
		memcpy(pStr,self->pStr,self->s32Cur);
		free(self->pStr);
	}
	self->pStr = pStr;
}

void adt_str_push(adt_str_t *self,const int c){
	if(self == 0) return;
	assert(self->s32Cur<(INT_MAX-2));
	adt_str_reserve(self,self->s32Cur+1);
	assert(self->s32Cur < self->s32Len);
	self->pStr[self->s32Cur++] = (uint8_t) c;
	assert(self->s32Cur < self->s32Len);
}

int adt_str_pop(adt_str_t *self){
	int retval = 0;
	if(self){
		if(self->s32Cur>0){
			retval = self->pStr[--self->s32Cur];
			self->pStr[self->s32Cur] = 0;
		}
	}
	return retval;
}

int adt_str_get_char(const adt_str_t *self,int index){
	if(self){
		int32_t s32Index;
		if(index<0){
			s32Index=(int32_t) (-index);
			if(s32Index<=self->s32Cur){
				s32Index=self->s32Cur-s32Index;
			}
			else{
				return -1; //out of bounds error
			}
		}
		else{
			s32Index = (int32_t) index;
			if(s32Index>=self->s32Cur){
				return -1; //out of bounds error
			}
		}
		return (int) self->pStr[s32Index];
	}
	return -1;
}


/* C string compatible functions */
adt_str_t *adt_str_dup_cstr(const char* cstr){
	if(cstr){
		adt_str_t *self = adt_str_new();
		if(self==0) return self;
		adt_str_set_cstr(self,cstr);
		return self;
	}
	return (adt_str_t*) 0;
}



void adt_str_set_cstr(adt_str_t *self, const char *str){
	int32_t s32Len = (uint32_t) strlen(str);
	adt_str_reserve(self,s32Len);
	assert(self->s32Len>s32Len);
	assert(self->pStr);
	memcpy(self->pStr,str,s32Len);
	self->s32Cur=s32Len;
}

void adt_str_append_cstr(adt_str_t *self,const char *str){
	int32_t s32Len = (int32_t) strlen(str);
	adt_str_reserve(self,self->s32Cur+s32Len);
	assert(self->s32Len>self->s32Cur+s32Len);
	memcpy(self->pStr+self->s32Cur,str,s32Len);
	self->s32Cur+=s32Len;
	self->pStr[self->s32Cur] = 0;
}

void adt_str_prepend_cstr(adt_str_t *self,const char *cstr);


void adt_str_copy_range(adt_str_t *self,const char *pBegin,const char *pEnd){
   int32_t s32Len;
   if( (self == 0) || (pBegin == 0) || (pEnd == 0)) return;
	if(pBegin>=pEnd) return;
   s32Len = (int32_t) (pEnd-pBegin);
	adt_str_reserve(self,s32Len);
	assert(self->s32Len>s32Len);
	assert(self->pStr);
	memcpy(self->pStr,pBegin,s32Len);
	self->s32Cur = s32Len;
}

void adt_str_append_range(adt_str_t *self,const char *pBegin,const char *pEnd){
   int32_t s32Len;
   int32_t s32NewLen;
   if ((self == 0) || (pBegin == 0) || (pEnd == 0)) return;
	if(pBegin>=pEnd) return;
   s32Len = (int32_t) (pEnd-pBegin);
   s32NewLen = self->s32Len + s32Len;
	adt_str_reserve(self,s32NewLen);
	assert(self->s32Len>s32NewLen);
	assert(self->pStr);
	memcpy(self->pStr+self->s32Len,pBegin,s32Len);
	self->s32Cur = s32NewLen;
}

const char* adt_str_cstr(adt_str_t *self){
	if(self){
		if(!self->pStr){
			adt_str_reserve(self,0);
		}
		assert(self->s32Cur<self->s32Len);
		self->pStr[self->s32Cur]=0;
		return (const char*) self->pStr;
	}
	return 0;
}









void adt_str_delete_void(void *ptr){
	if(ptr){
		adt_str_delete((adt_str_t*) ptr);
	}
}

void free_void(void *ptr){
	if(ptr){
		free(ptr);
	}
}

/***************** Private Function Definitions *******************/

int32_t adt_str_calcLen(int32_t s32CurLen, int32_t s32NewLen){
	if(s32CurLen>=s32NewLen){
		return s32CurLen;
	}

	if(s32NewLen > (INT_MAX - LARGE_BLOCK_SIZE) ){
		return INT_MAX;
	}
	else if( s32NewLen>=LARGE_BLOCK_SIZE ){
		//upgrade s32CurLen to LARGE_BLOCK_SIZE
		if( s32CurLen < LARGE_BLOCK_SIZE ){
			s32CurLen = LARGE_BLOCK_SIZE;
		}
		while(s32CurLen<s32NewLen) s32CurLen+=LARGE_BLOCK_SIZE;
	}
	else if(s32NewLen >= MEDIUM_BLOCK_SIZE){
		//upgrade s32CurLen to MEDIUM_BLOCK_SIZE
		if( s32CurLen < MEDIUM_BLOCK_SIZE ){
			s32CurLen = MEDIUM_BLOCK_SIZE;
		}
		while(s32CurLen<s32NewLen) s32CurLen*=2;
	}
	else{
		//upgrade s32CurLen to MIN_BLOCK_SIZE
		if( s32CurLen < MIN_BLOCK_SIZE ){
			s32CurLen = MIN_BLOCK_SIZE;
		}
		while(s32CurLen<s32NewLen) s32CurLen*=4;
	}
	return s32CurLen;
}


