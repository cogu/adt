/*****************************************************************************
* @file:   		adt_str.h
* @author:		Conny Gustafsson
* @date:		2013-05-09
* @brief:		String Container
*
* Copyright 2013 Conny Gustafsson
*
******************************************************************************/
#ifndef ADT_STR_H__
#define ADT_STR_H__
#include <stdint.h>

#define ADT_STR_VERSION 0.1.1

typedef struct {
	  int32_t s32Cur;
	  int32_t s32Len;
	  uint8_t *pStr;
}adt_str_t;

/***************** Public Function Declarations *******************/

/* constructor/destructor */

/**
* creates new adt_str object
*/
adt_str_t *adt_str_new(void);


/**
 * deletes an adt_str object
 * \param[in]	self	object instance
 */
void adt_str_delete(adt_str_t *self);

/**
 * constructor
 * \param[in]	self	object instance
 */
void adt_str_create(adt_str_t *self);

/**
 * destructor
 * \param[in]	self	object instance
 */
void adt_str_destroy(adt_str_t *self);


/* adt_str functions */

/**
 * copy constructor
 * @param[in]	other	object instance to copy from
 */
adt_str_t *adt_str_dup(const adt_str_t* other);
adt_str_t *adt_str_make(const char *pBegin, const char *pEnd);
int32_t adt_str_length(const adt_str_t *self);
void adt_str_clear(adt_str_t *self);
void adt_str_set(adt_str_t *lhs, const adt_str_t *rhs);
void adt_str_append(adt_str_t *lhs, const adt_str_t *rhs);
void adt_str_reserve(adt_str_t *self,int32_t s32Len);
void adt_str_push(adt_str_t *self,const int c);
int adt_str_pop(adt_str_t *self);
int adt_str_get_char(const adt_str_t *self,int index);

/* C string compatible functions */
adt_str_t *adt_str_dup_cstr(const char* cstr); //constructor
void adt_str_set_cstr(adt_str_t *self, const char *cstr);
void adt_str_append_cstr(adt_str_t *self,const char *cstr);
void adt_str_prepend_cstr(adt_str_t *self,const char *cstr);
void adt_str_copy_range(adt_str_t *self,const char *pBegin,const char *pEnd);
void adt_str_append_range(adt_str_t *self,const char *pBegin,const char *pEnd);
const char* adt_str_cstr(adt_str_t *self);






/* utility functions */
void adt_str_delete_void(void *ptr);
void free_void(void *ptr);

#endif //ADT_STR_H__
