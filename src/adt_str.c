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
#include "CMemLeak.h"

/*
 * Each string starts out being 16 bytes in length
 * When this becomes too small, it quadruples in size (multiply by 4) until it reaches 64K.
 * It then doubles until it reaches 64MB. After that it does linear growth in chunks of 64MB.
 */
#define MIN_BLOCK_SIZE 		16
#define MEDIUM_BLOCK_SIZE 	65536
#define LARGE_BLOCK_SIZE 	67108864

/**************** Private Function Declarations *******************/
static int32_t adt_str_calcLen(int32_t u32CurLen, int32_t u32NewLen);


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
		this->u32Cur = 0;
		this->u32Len = 0;
		this->pStr = (uint8_t*)0;
	}
}
void adt_str_destroy(adt_str_t *this){
	if(this){
		if(this->pStr) free(this->pStr);
		this->pStr=0;
		this->u32Cur = 0;
		this->u32Len = 0;
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

int32_t adt_str_len(adt_str_t *this){
	if(this){
		return this->u32Cur;
	}
	return 0;
}

void adt_str_clear(adt_str_t *this){
	if(this == 0) return;
	this->u32Cur = 0;
}

void adt_str_set(adt_str_t *lhs, adt_str_t *rhs){
	if( (lhs == 0) || (rhs == 0) ) return;
	if(lhs->pStr) free(lhs->pStr);
	lhs->pStr = (uint8_t*) malloc(rhs->u32Len);
	assert(lhs->pStr);
	memcpy(lhs->pStr,rhs->pStr,rhs->u32Len);
	lhs->u32Cur = rhs->u32Cur;
	lhs->u32Len = rhs->u32Len;
}

void adt_str_append(adt_str_t *lhs,adt_str_t *rhs);

void adt_str_reserve(adt_str_t *this,uint32_t u32Len){
	u32Len++; //reserve 1 byte for null terminator
	if(u32Len <= this->u32Len){
		return;
	}
	this->u32Len = adt_str_calcLen(this->u32Len,u32Len);
	uint8_t *pStr = (uint8_t*) malloc(this->u32Len);
	assert(pStr);
	if(this->pStr){
		memcpy(pStr,this->pStr,this->u32Cur);
		free(this->pStr);
	}
	this->pStr = pStr;
}

void adt_str_push(adt_str_t *this,const int c){
	if(this == 0) return;
	assert(this->u32Cur<(UINT_MAX-2));
	adt_str_reserve(this,this->u32Cur+1);
	assert(this->u32Cur < this->u32Len);
	this->pStr[this->u32Cur++] = (uint8_t) c;
	assert(this->u32Cur < this->u32Len);
}

int adt_str_chop(adt_str_t *this){
	int retval = 0;
	if(this){
		if(this->u32Cur>0){
			retval = this->pStr[--this->u32Cur];
			this->pStr[this->u32Cur] = 0;
		}
	}
	return retval;
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
	uint32_t u32Len = (uint32_t) strlen(str);
	adt_str_reserve(this,u32Len);
	assert(this->u32Len>u32Len);
	assert(this->pStr);
	memcpy(this->pStr,str,u32Len);
	this->u32Cur=u32Len;
}

void adt_str_append_cstr(adt_str_t *this,const char *str){
	uint32_t u32Len = (uint32_t) strlen(str);
	adt_str_reserve(this,this->u32Cur+u32Len);
	assert(this->u32Len>this->u32Cur+u32Len);
	memcpy(this->pStr+this->u32Cur,str,u32Len);
	this->u32Cur+=u32Len;
	this->pStr[this->u32Cur] = 0;
}

void adt_str_prepend_cstr(adt_str_t *this,const char *cstr);


void adt_str_copy_range(adt_str_t *this,const char *pBegin,const char *pEnd){
	if( (this == 0) || (pBegin == 0) || (pEnd == 0)) return;
	if(pBegin>=pEnd) return;
	uint32_t u32Len = (uint32_t) (pEnd-pBegin);
	adt_str_reserve(this,u32Len);
	assert(this->u32Len>u32Len);
	assert(this->pStr);
	memcpy(this->pStr,pBegin,u32Len);
	this->u32Cur = u32Len;
}

void adt_str_append_range(adt_str_t *this,const char *pBegin,const char *pEnd){
	if( (this == 0) || (pBegin == 0) || (pEnd == 0)) return;
	if(pBegin>=pEnd) return;
	uint32_t u32Len = (uint32_t) (pEnd-pBegin);
	uint32_t u32NewLen = this->u32Len + u32Len;
	adt_str_reserve(this,u32NewLen);
	assert(this->u32Len>u32NewLen);
	assert(this->pStr);
	memcpy(this->pStr+this->u32Len,pBegin,u32Len);
	this->u32Cur = u32NewLen;
}

const char* adt_str_cstr(adt_str_t *this){
	if(this){
		if(!this->pStr){
			adt_str_reserve(this,0);
		}
		assert(this->u32Cur<this->u32Len);
		this->pStr[this->u32Cur]=0;
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

int32_t adt_str_calcLen(int32_t u32CurLen, int32_t u32NewLen){
	if(u32CurLen>=u32NewLen){
		return u32CurLen;
	}

	if(u32NewLen > (ULONG_MAX - LARGE_BLOCK_SIZE) ){
		return ULONG_MAX;
	}
	else if( u32NewLen>=LARGE_BLOCK_SIZE ){
		//upgrade u32CurLen to LARGE_BLOCK_SIZE
		if( u32CurLen < LARGE_BLOCK_SIZE ){
			u32CurLen = LARGE_BLOCK_SIZE;
		}
		while(u32CurLen<u32NewLen) u32CurLen+=LARGE_BLOCK_SIZE;
	}
	else if(u32NewLen >= MEDIUM_BLOCK_SIZE){
		//upgrade u32CurLen to MEDIUM_BLOCK_SIZE
		if( u32CurLen < MEDIUM_BLOCK_SIZE ){
			u32CurLen = MEDIUM_BLOCK_SIZE;
		}
		while(u32CurLen<u32NewLen) u32CurLen*=2;
	}
	else{
		//upgrade u32CurLen to MIN_BLOCK_SIZE
		if( u32CurLen < MIN_BLOCK_SIZE ){
			u32CurLen = MIN_BLOCK_SIZE;
		}
		while(u32CurLen<u32NewLen) u32CurLen*=4;
	}
	return u32CurLen;
}


