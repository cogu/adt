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
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


/*
 * Each string starts out being 16 bytes in length
 * When this becomes too small, it quadruples in size (multiply by 4) until it reaches 64K.
 * It then doubles until it reaches 64MB. After that it does linear growth in chunks of 64MB.
 */
#define MIN_BLOCK_SIZE 		16
#define MEDIUM_BLOCK_SIZE 	65536
#define LARGE_BLOCK_SIZE 	67108864

/**************** Private Function Declarations *******************/
static int32_t adt_str_calcLen(int32_t s32CurLen, int32_t u32NewLen);


/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
/* constructor/destructor */
adt_str_t *adt_str_new(void){
	adt_str_t *this;
	if((this = (adt_str_t*) malloc(sizeof(adt_str_t))) == 0){
		return (adt_str_t*)0;
	}
	adt_str_create(this);
	return this;
}


void adt_str_delete(adt_str_t *this){
	if(this){
		adt_str_destroy(this);
		free(this);
	}
}

void adt_str_create(adt_str_t *this){
	if(this){
		this->s32Cur = 0;
		this->s32Len = 0;
		this->pStr = (uint8_t*)0;
	}
}
void adt_str_destroy(adt_str_t *this){
	if(this){
		if(this->pStr) free(this->pStr);
		this->pStr=0;
		this->s32Cur = 0;
		this->s32Len = 0;
	}
}


/* adt_str functions */

adt_str_t *adt_str_dup(adt_str_t* other){
	adt_str_t *this;
	this = adt_str_new();
	if(this){
		adt_str_set(this,other);
	}
	return this;
}

adt_str_t *adt_str_make(const char *pBegin, const char *pEnd){
	adt_str_t *this;
	this = adt_str_new();
	if(this){
		adt_str_copy_range(this,pBegin,pEnd);
	}
	return this;
}

int32_t adt_str_length(adt_str_t *this){
	if(this){
		return this->s32Cur;
	}
	return 0;
}

void adt_str_clear(adt_str_t *this){
	if(this == 0) return;
	this->s32Cur = 0;
}

void adt_str_set(adt_str_t *lhs, adt_str_t *rhs){
	if( (lhs == 0) || (rhs == 0) ) return;
	if(lhs->pStr) free(lhs->pStr);
	lhs->pStr = (uint8_t*) malloc(rhs->s32Len);
	assert(lhs->pStr);
	memcpy(lhs->pStr,rhs->pStr,rhs->s32Len);
	lhs->s32Cur = rhs->s32Cur;
	lhs->s32Len = rhs->s32Len;
}

void adt_str_append(adt_str_t *lhs,adt_str_t *rhs);

void adt_str_reserve(adt_str_t *this,int32_t s32Len){
	s32Len++; //reserve 1 byte for null terminator
	if(s32Len <= this->s32Len){
		return;
	}
	this->s32Len = adt_str_calcLen(this->s32Len,s32Len);
	uint8_t *pStr = (uint8_t*) malloc(this->s32Len);
	assert(pStr);
	if(this->pStr){
		memcpy(pStr,this->pStr,this->s32Cur);
		free(this->pStr);
	}
	this->pStr = pStr;
}

void adt_str_push(adt_str_t *this,const int c){
	if(this == 0) return;
	assert(this->s32Cur<(INT_MAX-2));
	adt_str_reserve(this,this->s32Cur+1);
	assert(this->s32Cur < this->s32Len);
	this->pStr[this->s32Cur++] = (uint8_t) c;
	assert(this->s32Cur < this->s32Len);
}

int adt_str_pop(adt_str_t *this){
	int retval = 0;
	if(this){
		if(this->s32Cur>0){
			retval = this->pStr[--this->s32Cur];
			this->pStr[this->s32Cur] = 0;
		}
	}
	return retval;
}

int adt_str_get_char(adt_str_t *this,int index){
	if(this){
		int32_t s32Index;
		if(index<0){
			s32Index=(int32_t) (-index);
			if(s32Index<=this->s32Cur){
				s32Index=this->s32Cur-s32Index;
			}
			else{
				return -1; //out of bounds error
			}
		}
		else{
			s32Index = (int32_t) index;
			if(s32Index>=this->s32Cur){
				return -1; //out of bounds error
			}
		}
		return (int) this->pStr[s32Index];
	}
	return -1;
}


/* C string compatible functions */
adt_str_t *adt_str_dup_cstr(const char* cstr){
	if(cstr){
		adt_str_t *this = adt_str_new();
		if(this==0) return this;
		adt_str_set_cstr(this,cstr);
		return this;
	}
	return (adt_str_t*) 0;
}



void adt_str_set_cstr(adt_str_t *this, const char *str){
	int32_t s32Len = (uint32_t) strlen(str);
	adt_str_reserve(this,s32Len);
	assert(this->s32Len>s32Len);
	assert(this->pStr);
	memcpy(this->pStr,str,s32Len);
	this->s32Cur=s32Len;
}

void adt_str_append_cstr(adt_str_t *this,const char *str){
	int32_t s32Len = (int32_t) strlen(str);
	adt_str_reserve(this,this->s32Cur+s32Len);
	assert(this->s32Len>this->s32Cur+s32Len);
	memcpy(this->pStr+this->s32Cur,str,s32Len);
	this->s32Cur+=s32Len;
	this->pStr[this->s32Cur] = 0;
}

void adt_str_prepend_cstr(adt_str_t *this,const char *cstr);


void adt_str_copy_range(adt_str_t *this,const char *pBegin,const char *pEnd){
	if( (this == 0) || (pBegin == 0) || (pEnd == 0)) return;
	if(pBegin>=pEnd) return;
	int32_t s32Len = (int32_t) (pEnd-pBegin);
	adt_str_reserve(this,s32Len);
	assert(this->s32Len>s32Len);
	assert(this->pStr);
	memcpy(this->pStr,pBegin,s32Len);
	this->s32Cur = s32Len;
}

void adt_str_append_range(adt_str_t *this,const char *pBegin,const char *pEnd){
	if( (this == 0) || (pBegin == 0) || (pEnd == 0)) return;
	if(pBegin>=pEnd) return;
	int32_t s32Len = (int32_t) (pEnd-pBegin);
	int32_t s32NewLen = this->s32Len + s32Len;
	adt_str_reserve(this,s32NewLen);
	assert(this->s32Len>s32NewLen);
	assert(this->pStr);
	memcpy(this->pStr+this->s32Len,pBegin,s32Len);
	this->s32Cur = s32NewLen;
}

const char* adt_str_cstr(adt_str_t *this){
	if(this){
		if(!this->pStr){
			adt_str_reserve(this,0);
		}
		assert(this->s32Cur<this->s32Len);
		this->pStr[this->s32Cur]=0;
		return (const char*) this->pStr;
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

int32_t adt_str_calcLen(int32_t s32CurLen, int32_t u32NewLen){
	if(s32CurLen>=u32NewLen){
		return s32CurLen;
	}

	if(u32NewLen > (LONG_MAX - LARGE_BLOCK_SIZE) ){
		return (int32_t) LONG_MAX;
	}
	else if( u32NewLen>=LARGE_BLOCK_SIZE ){
		//upgrade s32CurLen to LARGE_BLOCK_SIZE
		if( s32CurLen < LARGE_BLOCK_SIZE ){
			s32CurLen = LARGE_BLOCK_SIZE;
		}
		while(s32CurLen<u32NewLen) s32CurLen+=LARGE_BLOCK_SIZE;
	}
	else if(u32NewLen >= MEDIUM_BLOCK_SIZE){
		//upgrade s32CurLen to MEDIUM_BLOCK_SIZE
		if( s32CurLen < MEDIUM_BLOCK_SIZE ){
			s32CurLen = MEDIUM_BLOCK_SIZE;
		}
		while(s32CurLen<u32NewLen) s32CurLen*=2;
	}
	else{
		//upgrade s32CurLen to MIN_BLOCK_SIZE
		if( s32CurLen < MIN_BLOCK_SIZE ){
			s32CurLen = MIN_BLOCK_SIZE;
		}
		while(s32CurLen<u32NewLen) s32CurLen*=4;
	}
	return s32CurLen;
}


