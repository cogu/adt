/*****************************************************************************
* \file      adt_str.h
* \author    Conny Gustafsson
* \date      2013-05-09
* \brief     String Container
*
* Copyright 2013-2019 Conny Gustafsson
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
#ifndef ADT_STR_H__
#define ADT_STR_H__

#define ADT_STR_VERSION 0.1.2

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "adt_bytearray.h"
#include "adt_error.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

typedef struct adt_str_tag {
     int32_t s32Cur; //Size of the string
     int32_t s32Len; //Size of the allocated array
     uint8_t *pStr;  //Array of bytes
     adt_error_t lastError;
} adt_str_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
/* constructors */
void adt_str_create(adt_str_t *self);
adt_str_t *adt_str_new(void);
adt_str_t *adt_str_clone(const adt_str_t* other);
adt_str_t *adt_str_new_bstr(const char *pBegin, const char *pEnd);
adt_str_t *adt_str_new_cstr(const char * other);
adt_str_t *adt_str_new_byterray(adt_bytearray_t *bytearray);
adt_str_t *adt_str_concat(const adt_str_t *lhs, const adt_str_t *rhs);

/* destructors */
void adt_str_destroy(adt_str_t *self);
void adt_str_delete(adt_str_t *self);
void adt_str_vdelete(void *arg);

/* string manipulation */
adt_error_t adt_str_set(adt_str_t *self, const adt_str_t* other);
adt_error_t adt_str_set_bstr(adt_str_t *self, const char *pBegin, const char *pEnd);
adt_error_t adt_str_set_cstr(adt_str_t *self, const char *cstr);

adt_error_t adt_str_append(adt_str_t *self, const adt_str_t* other);
adt_error_t adt_str_append_bstr(adt_str_t *self, const char *pBegin, const char *pEnd);
adt_error_t adt_str_append_cstr(adt_str_t *self, const char *cstr);

void adt_str_prepend(adt_str_t *self, const adt_str_t* other);
void adt_str_prepend_bstr(adt_str_t *self, const char *pBegin, const char *pEnd);
void adt_str_prepend_cstr(adt_str_t *self, const char *cstr);

adt_error_t adt_str_push(adt_str_t *self, const int c);
int adt_str_pop(adt_str_t *self);

/* getters */
int adt_str_charAt(adt_str_t *self, int index);
const char* adt_str_cstr(adt_str_t *self);
adt_error_t adt_str_bstr(adt_str_t *self, const char **ppBegin, const char **ppEnd);
adt_bytearray_t *adt_str_bytearray(adt_str_t *self);

/* utility */
int32_t adt_str_length(const adt_str_t *self);
int32_t adt_str_size(const adt_str_t *self);
adt_error_t adt_str_reserve(adt_str_t *self, int32_t s32NewLen);
void adt_str_clear(adt_str_t *self);
adt_error_t adt_str_getLastError(adt_str_t *self);

bool adt_str_equal(const adt_str_t *self, const adt_str_t* other);
bool adt_str_equal_bstr(const adt_str_t *self, const char *pBegin, const char *pEnd);
bool adt_str_equal_cstr(const adt_str_t *self, const char *cstr);
void free_void(void *ptr);

/* function aliases */
#define adt_str_delete_void adt_str_vdelete
#define adt_str_dup adt_str_new_cstr
#define adt_str_make adt_str_new_bstr
#define adt_str_reset adt_str_destroy


#endif //ADT_STR_H__
