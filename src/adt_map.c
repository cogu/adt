/*****************************************************************************
* \file      adt_map.c
* \author    Conny Gustafsson
* \date      2013-10-01
* \brief     key/value map table using a sorted array as storage container
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_map.h"
#include <assert.h>
#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
#include <stdlib.h>
#endif
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static uint32_t adt_u16Map_lower_bound(const adt_u16MapElem_t *pBegin, uint32_t num_elem, uint16_t key);
static adt_u16MapElem_t *adt_u16Map_binary_search(adt_u16MapElem_t *pBegin, adt_u16MapElem_t *pEnd, uint16_t key);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void adt_u16Map_create(adt_u16Map_t *self, adt_u16MapElem_t *pArray, uint32_t max_num_elem, void (*pDestructor)(void*)){
   if(self != NULL){
      self->pBegin = pArray;
      self->pEnd = pArray;
      self->pIter = NULL;
      self->max_num_elem = max_num_elem;
      self->num_elem = 0;
      self->pDestructor = pDestructor;
      self->destructor_enable = 1;
   }
}

void adt_u16Map_destroy(adt_u16Map_t *self){
   adt_u16Map_clear(self);
}

#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
adt_u16Map_t *adt_u16Map_new(uint32_t max_num_elem, void (*pDestructor)(void*)){
   adt_u16Map_t *self = (adt_u16Map_t*) malloc(sizeof(adt_u16Map_t));
   if(self != NULL){
      adt_u16MapElem_t *elem = (adt_u16MapElem_t*) malloc(sizeof(adt_u16MapElem_t) * max_num_elem);
      if(elem == NULL){
         free(self);
         return NULL;
      }
      adt_u16Map_create(self, elem, max_num_elem, pDestructor);
   }
   return self;
}

void adt_u16Map_delete(adt_u16Map_t *self){
   if(self != NULL){
      adt_u16Map_clear(self);
      free(self->pBegin);
      free(self);
   }
}
#endif

void adt_u16Map_destructor_enable(adt_u16Map_t *self,uint8_t enable){
   if(self != NULL){
      self->destructor_enable = (enable!=0)? 1 : 0;
   }
}

void adt_u16Map_clear(adt_u16Map_t *self){
   if(self != NULL){
      while(self->num_elem > 0){
         self->pEnd--;
         if( (self->destructor_enable != false) && (self->pDestructor != NULL) ){
            self->pDestructor(self->pEnd->val);
         }
         self->num_elem--;
      }
      assert(self->pBegin == self->pEnd);
   }
}

adt_error_t adt_u16Map_insert(adt_u16Map_t *self, uint16_t key, void *val){
   if ((self == NULL) || (self->pBegin == NULL)) {
      return ADT_INVALID_ARGUMENT_ERROR;
   }

   uint32_t i;

   // Fast path for append: map is empty or key is strictly greater than the last key
   if ((self->num_elem == 0) || (key > self->pBegin[self->num_elem - 1].key)) {
      i = self->num_elem;
   }
   else {
      // Find first occurrence with elem->key >= key using lower_bound
      uint32_t low = adt_u16Map_lower_bound(self->pBegin, self->num_elem, key);
      i = low;
      while ((i < self->num_elem) && (self->pBegin[i].key == key)) {
         if (self->pBegin[i].val == val) {
            // Exact key/val duplicate already in map; ignore insert request
            return ADT_NO_ERROR;
         }
         i++;
      }
   }

   if (self->num_elem >= self->max_num_elem) {
      return ADT_OVERFLOW_ERROR;
   }

   // Make room for elem if inserting before the end
   if (i < self->num_elem) {
      memmove(&self->pBegin[i + 1], &self->pBegin[i], (size_t)(self->num_elem - i) * sizeof(adt_u16MapElem_t));
   }

   self->pBegin[i].key = key;
   self->pBegin[i].val = val;
   self->num_elem++;
   self->pEnd++;
   return ADT_NO_ERROR;
}

void adt_u16Map_remove(adt_u16Map_t *self, const adt_u16MapElem_t *pElem){
   if( (self == NULL) || (self->pBegin == NULL) || (pElem == NULL) ){
      return;
   }
   if( (pElem >= self->pBegin) && (pElem < self->pEnd) ){
      uint32_t i = (uint32_t) (pElem - self->pBegin);
      assert( i < self->num_elem);
      uint32_t num_to_move = self->num_elem - (i + 1);
      if (num_to_move > 0) {
         memmove(&self->pBegin[i], &self->pBegin[i + 1], (size_t)num_to_move * sizeof(adt_u16MapElem_t));
      }
      self->num_elem--;
      self->pEnd--;
   }
}

adt_u16MapElem_t* adt_u16Map_find(adt_u16Map_t *self, uint16_t key){
   adt_u16MapElem_t *it = adt_u16Map_binary_search(self->pBegin,self->pEnd, key);
   if(it != NULL){
      (void) adt_u16Map_iter_init(self,it);
   }
   return it;
}

adt_u16MapElem_t* adt_u16Map_find_exact(adt_u16Map_t *self, uint16_t key, const void *val){
   adt_u16MapElem_t *it = adt_u16Map_find(self,key);
   if(it != NULL){
      it = adt_u16Map_iter_init(self,it);
      while( (it != NULL) && (it->key == key) ){
         if(it->val == val ){
            return it;
         }
         it=adt_u16Map_iter_next(self);
      }
   }
   return NULL;
}

uint32_t adt_u16Map_size(const adt_u16Map_t *self){
   if(self != NULL){
      return self->num_elem;
   }
   return 0;
}

adt_u16MapElem_t* adt_u16Map_iter_init(adt_u16Map_t *self, adt_u16MapElem_t *pElem){
   if(self->pBegin < self->pEnd){
      if( (pElem >= self->pBegin) && (pElem < self->pEnd) ){
         self->pIter = pElem;
      }
      else{
         self->pIter = self->pBegin;
      }
      return self->pIter;
   }
   return NULL;
}

adt_u16MapElem_t* adt_u16Map_iter_next(adt_u16Map_t *self){
   if(self->pIter < self->pEnd){
      ++self->pIter;
      if(self->pIter < self->pEnd){
         return self->pIter;
      }
   }
   return NULL;
}

/**
 * move all items matching key \param key from \param src to \param dest
 */
uint32_t adt_u16Map_move_elem(adt_u16Map_t *dest, adt_u16Map_t *src, uint16_t key){
   if( (dest == NULL) || (dest->pBegin == NULL) || (src == NULL) || (src->pBegin == NULL) ){
      return 0;
   }

   uint32_t start = adt_u16Map_lower_bound(src->pBegin, src->num_elem, key);
   if ((start >= src->num_elem) || (src->pBegin[start].key != key)) {
      return 0;
   }

   uint32_t end = start;
   while ((end < src->num_elem) && (src->pBegin[end].key == key)) {
      end++;
   }

   uint32_t num_items = end - start;

   for (uint32_t i = start; i < end; i++) {
      adt_u16Map_insert(dest, src->pBegin[i].key, src->pBegin[i].val);
   }

   uint32_t remaining = src->num_elem - end;
   if (remaining > 0) {
      memmove(&src->pBegin[start], &src->pBegin[end], (size_t)remaining * sizeof(adt_u16MapElem_t));
   }
   src->num_elem -= num_items;
   src->pEnd -= num_items;

   return num_items;
}

void adt_u16Map_remove_val(adt_u16Map_t *self, const void *val){
   if ((self == NULL) || (self->pBegin == NULL) || (self->num_elem == 0)) {
      return;
   }

   uint32_t write_idx = 0;
   for (uint32_t read_idx = 0; read_idx < self->num_elem; read_idx++) {
      if (self->pBegin[read_idx].val != val) {
         if (write_idx != read_idx) {
            self->pBegin[write_idx] = self->pBegin[read_idx];
         }
         write_idx++;
      }
   }

   self->num_elem = write_idx;
   self->pEnd = self->pBegin + write_idx;
}


//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static uint32_t adt_u16Map_lower_bound(const adt_u16MapElem_t *pBegin, uint32_t num_elem, uint16_t key)
{
   uint32_t low = 0;
   uint32_t high = num_elem;

   while (low < high) {
      uint32_t mid = low + ((high - low) / 2);
      if (pBegin[mid].key < key) {
         low = mid + 1;
      }
      else {
         high = mid;
      }
   }
   return low;
}

static adt_u16MapElem_t *adt_u16Map_binary_search(adt_u16MapElem_t *pBegin, adt_u16MapElem_t *pEnd, uint16_t key){
   if ((pBegin == NULL) || (pEnd <= pBegin)) {
      return NULL;
   }
   uint32_t num_elem = (uint32_t)(pEnd - pBegin);
   uint32_t idx = adt_u16Map_lower_bound(pBegin, num_elem, key);
   if ((idx < num_elem) && (pBegin[idx].key == key)) {
      return &pBegin[idx];
   }
   return NULL;
}


