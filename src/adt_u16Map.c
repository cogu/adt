/*****************************************************************************
* \file      adt_u16Map.c
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
#include "adt_u16Map.h"
#include <assert.h>
#include <malloc.h>
#include <stddef.h>
#include <stdbool.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define MAX_NUM_TRANSFER 20

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static adt_u16MapElem_t *adt_u16Map_binarySearchDup(adt_u16MapElem_t *pBegin, adt_u16MapElem_t *pEnd, uint16_t key);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void adt_u16Map_create(adt_u16Map_t *self, adt_u16MapElem_t *pArray, uint16_t max_num_elem, void (*pDestructor)(void*)){
   if(self != NULL){
      assert(max_num_elem<65535); //temporary fix for a design-flaw found in the code
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

adt_u16Map_t *adt_u16Map_new(uint16_t max_num_elem, void (*pDestructor)(void*)){
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

void adt_u16Map_insert(adt_u16Map_t *self, uint16_t key, void *val){
   uint16_t i;
   adt_u16MapElem_t *elem;
   if(self->pBegin == NULL){
      return;
   }
   elem = self->pBegin;
   if(self->num_elem >= self->max_num_elem){
      //new item won't fit in map
      return;
   }
   for(i=0;i < self->num_elem; i++){
      if(elem->key == key){
         if(elem->val == val){
            //element already in map, ignore insert request
            return;
         }
      }
      else if(elem->key > key){
         break;
      }
      elem++;
   }
   if(i < self->num_elem){
      uint16_t j;
      //make room for elem
      for(j=self->num_elem;j > i ; j--){
         self->pBegin[j]=self->pBegin[j-1];
      }
      assert(i==j);
   }
   assert(i<self->max_num_elem);
   assert(&self->pBegin[i] == elem);
   elem->key = key;
   elem->val = val;
   self->num_elem++;
   self->pEnd++;
}

void adt_u16Map_remove(adt_u16Map_t *self, const adt_u16MapElem_t *pElem){
   if( (self == NULL) || (self->pBegin == NULL) || (pElem == NULL) ){
      return;
   }
   if( (pElem >= self->pBegin) && (pElem < self->pEnd) ){
      uint32_t i;
      uint32_t j;
      i = (uint32_t) (pElem-self->pBegin);
      assert( i < self->num_elem);
      //remove element by copy data left
      for(j=i+1;j < self->num_elem ; j++){
         self->pBegin[j-1]=self->pBegin[j];
      }
      assert(j == self->num_elem);
      self->num_elem--;
      self->pEnd--;
   }
}

adt_u16MapElem_t* adt_u16Map_find(adt_u16Map_t *self, uint16_t key){
   adt_u16MapElem_t *it = adt_u16Map_binarySearchDup(self->pBegin,self->pEnd, key);
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

uint16_t adt_u16Map_size(const adt_u16Map_t *self){
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
uint16_t adt_u16Map_move_elem(adt_u16Map_t *dest, adt_u16Map_t *src, uint16_t key){
   adt_u16MapElem_t *itemsToMove[MAX_NUM_TRANSFER];
   adt_u16MapElem_t dataToMove[MAX_NUM_TRANSFER];
   adt_u16MapElem_t **pCursor;
   adt_u16MapElem_t *pData;
   uint16_t numItems = 0;
   uint16_t totalItems = 0;

   if( (dest == NULL) || (dest->pBegin == NULL) || (src == NULL) || (src->pBegin == NULL) ){
      return 0;
   }

   while(1){
      adt_u16MapElem_t *it;
      pCursor = itemsToMove;
      pData = dataToMove;
      it = adt_u16Map_find(src,key);
      if(it != NULL){
         it=adt_u16Map_iter_init(src,it);
         while(numItems < MAX_NUM_TRANSFER){
            if( (it == NULL) || (it->key != key) ){
               break; //no more items
            }
            else{
               //mark items for removal, copy data
               *(pCursor++) = it;
               *(pData++) = *it;
               numItems++;
            }
            it = adt_u16Map_iter_next(src);
         }
         pData = dataToMove;
         totalItems+=numItems;
         while(numItems){
            //All iterators with higher address will be destroyed when item is removed.
            //Therefore we remove items in reverse order (higher address to lower).
            //Data is added in sorted order (more efficient inserts).
            it = *(--pCursor);
            adt_u16Map_remove(src,it);
            adt_u16Map_insert(dest,pData->key, pData->val);
            pData++;
            numItems--;
         }
      }
      else{
         return totalItems; //done
      }
   }
   return 0;
}

void adt_u16Map_remove_val(adt_u16Map_t *self, const void *val){
   uint16_t tmp[MAX_NUM_TRANSFER];
   uint16_t tmpLen = 0;
   adt_u16MapElem_t *it;
   while(1){
      tmpLen = 0;
      it = adt_u16Map_iter_init(self,0);
      while((tmpLen < MAX_NUM_TRANSFER) && it){
         if(it->val == val){
            tmp[tmpLen++] = it->key;
         }
         it = adt_u16Map_iter_next(self);
      }
      if(tmpLen>0){
         uint16_t i;
         for(i=0;i<tmpLen;i++){
            it = adt_u16Map_find_exact(self,tmp[i],val);
            if(it){
               adt_u16Map_remove(self,it);
            }
         }
         if(tmpLen < MAX_NUM_TRANSFER){
            //last loop
            break;
         }
      }
      else{
         //no items to delete
         break;
      }
   }
}


//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
adt_u16MapElem_t *adt_u16Map_binarySearchDup(adt_u16MapElem_t *pBegin, adt_u16MapElem_t *pEnd, uint16_t key){
   adt_u16MapElem_t *pMid;
   adt_u16MapElem_t *pLow = pBegin;
   adt_u16MapElem_t *pHigh = pEnd;

   uint32_t num_elem;
   while(1){
      assert(pHigh >= pLow);
      num_elem = (uint32_t)(pHigh - pLow);
      //perform a linear search if there is 3 items or less
      if(num_elem <= 3){
         uint32_t i;
         for(i=0;i<num_elem;i++){
            if (pLow[i].key == key) {
               if(i==0){
                  //decrease pointer if key is duplicated
                  while((pLow>pBegin) && (pLow[-1].key == key)) pLow--;
                  return pLow;
               }
               return &pLow[i];
            }
         }
         break;
      }
      else{
         pMid = pLow+(num_elem/2);
         assert( (pMid>=pLow) && (pMid<pHigh));
         if(pMid->key < key){
            pLow = pMid;
         }
         else if(pMid->key > key){
            pHigh = pMid;
         }
         else{
            pLow = pMid;
            pHigh = pMid+1;
         }
      }
   }
   return NULL;
}


