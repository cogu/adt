/*****************************************************************************
* \file      adt_error.h
* \author    Conny Gustafsson
* \date      2013-06-03
* \brief     array data structure
* \details   https://github.com/cogu/adt
*
* Copyright (c) 2013-2018 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/
#ifndef ADT_ARY_H
#define ADT_ARY_H
#include <stdint.h>
#if defined(_MSC_PLATFORM_TOOLSET) && (_MSC_PLATFORM_TOOLSET<=110)
#ifndef _MSC_BOOL_DEFINED
#define _MSC_BOOL_DEFINED
#define false 0
#define true 1
typedef uint8_t bool;
#endif
#else
#include <stdbool.h>
#endif
#include "adt_error.h"

#define ADT_ARRAY_VERSION 0.1.3

typedef struct adt_ary_tag{
	void **ppAlloc;		//array of (void*)
	void **pFirst;		   //pointer to first array element
	int32_t s32AllocLen;	//number of elements allocated
	int32_t s32CurLen;	//number of elements currently in the array
	void (*pDestructor)(void*); //optional destructor function (typically vdelete functions from other data structures)
	void *pFillElem;            //optional fill element for new elements (defaults to NULL)
	bool destructorEnable;      //Temporarily disables use of element pDestructor
} adt_ary_t;

typedef int (adt_vlt_func_t)(const void *a, const void *b); //lt = less-than

/***************** Public Function Declarations *******************/
//Constructor/Destructor
adt_ary_t*	adt_ary_new(void (*pDestructor)(void*));
adt_ary_t*	adt_ary_make(void** ppElem, int32_t s32Len,void (*pDestructor)(void*));
void	      adt_ary_delete(adt_ary_t *self);
void        adt_ary_vdelete(void *arg);
void 	      adt_ary_create(adt_ary_t *self, void (*pDestructor)(void*));
void 	      adt_ary_destroy(adt_ary_t *self);
void        adt_ary_destructor_enable(adt_ary_t *self, bool enable);


//Accessors
void**      adt_ary_set(adt_ary_t *self, int32_t s32Index, void *pElem);
void**      adt_ary_get(adt_ary_t *self, int32_t s32Index);
adt_error_t	adt_ary_push(adt_ary_t *self, void *pElem);
adt_error_t adt_ary_push_unique(adt_ary_t *self, void *pElem);
void*	      adt_ary_pop(adt_ary_t *self);
void*	      adt_ary_shift(adt_ary_t *self);
adt_error_t	adt_ary_unshift(adt_ary_t *self, void *pElem);
void*       adt_ary_value(const adt_ary_t *self, int32_t s32Index);
adt_error_t adt_ary_remove(adt_ary_t *self, void *pElem);

//Utility functions
adt_error_t	adt_ary_extend(adt_ary_t *self, int32_t s32Len);
adt_error_t	adt_ary_fill(adt_ary_t *self, int32_t s32Len);
adt_error_t adt_ary_resize(adt_ary_t *self, int32_t s32Len);
void	      adt_ary_clear(adt_ary_t *self);
int32_t     adt_ary_length(const adt_ary_t *self);
void 	      adt_ary_set_fill_elem(adt_ary_t *self, void* pFillElem);
void* 	   adt_ary_get_fill_elem(const adt_ary_t *self);
adt_error_t adt_ary_splice(adt_ary_t *self, int32_t s32Index, int32_t s32Len);
int32_t	   adt_ary_exists(const adt_ary_t *self, int32_t s32Index);
adt_error_t adt_ary_sort(adt_ary_t *self, adt_vlt_func_t *key, bool reverse);

//built-in lt functions (for sorting)
int adt_i32_vlt(const void *a, const void *b);
int adt_u32_vlt(const void *a, const void *b);

#endif //ADT_ARY_H
