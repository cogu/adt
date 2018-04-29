/*****************************************************************************
* \file:   		adt_u16Map.h
* \author:		Conny Gustafsson
* \date:		2013-10-01
* \brief:		key/value map table using a sorted array as storage container
* \details		https://github.com/cogu/adt
*
* Copyright (c) 2013-2014 Conny Gustafsson
*
******************************************************************************/
#include "adt_u16Map.h"
#include <assert.h>
#include <malloc.h>

#define MAX_NUM_TRANSFER 20

/**************** Private Function Declarations *******************/
static adt_u16MapElem_t *adt_u16Map_binarySearchDup(adt_u16MapElem_t *pBegin, adt_u16MapElem_t *pEnd, uint16_t key);

/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
void adt_u16Map_create(adt_u16Map_t *self, adt_u16MapElem_t *pArray, uint16_t maxNumElem, void (*pDestructor)(void*)){
   if(self != 0){
      assert(maxNumElem<65535); //temporary fix for a design-flaw found in the code
      self->pBegin = pArray;
      self->pEnd = pArray;
      self->pIter = 0;
      self->maxNumElem = maxNumElem;
      self->numElem = 0;
      self->pDestructor = pDestructor;
      self->destructorEnable = 1;
   }
}

void adt_u16Map_destroy(adt_u16Map_t *self){
   adt_u16Map_clear(self);
}

adt_u16Map_t *adt_u16Map_new(uint16_t maxNumElem,void (*pDestructor)(void*) ){
   adt_u16MapElem_t *elem;
   adt_u16Map_t *self = malloc(sizeof(adt_u16Map_t));
   elem = malloc(sizeof(adt_u16MapElem_t)*maxNumElem);
   if( (self != 0) && (elem != 0)){
      adt_u16Map_create(self,elem,maxNumElem,pDestructor);
   }
   return self;
}

void adt_u16Map_delete(adt_u16Map_t *self){
   if(self != 0){
      adt_u16Map_clear(self);
      free(self->pBegin);
      free(self);
   }
}

void adt_u16Map_destructorEnable(adt_u16Map_t *self,uint8_t enable){
   if(self != 0){
      self->destructorEnable = (enable!=0)? 1 : 0;
   }
}

void adt_u16Map_clear(adt_u16Map_t *self){
   if(self != 0){
      while(self->numElem > 0){
         self->pEnd--;
         if( (self->destructorEnable != 0) && (self->pDestructor != 0) ){
            self->pDestructor(self->pEnd->val);
         }
         self->numElem--;
      }
      assert(self->pBegin == self->pEnd);
   }
}

void adt_u16Map_insert(adt_u16Map_t *self, uint16_t key, void *val){
   uint16_t i;
   adt_u16MapElem_t *elem;
   if(self->pBegin == 0){
      return;
   }
   elem = self->pBegin;
   if(self->numElem >= self->maxNumElem){
      //new item won't fit in map
      return;
   }
   for(i=0;i < self->numElem; i++){
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
   if(i < self->numElem){
      uint16_t j;
      //make room for elem
      for(j=self->numElem;j > i ; j--){
         self->pBegin[j]=self->pBegin[j-1];
      }
      assert(i==j);
   }
   assert(i<self->maxNumElem);
   assert(&self->pBegin[i] == elem);
   elem->key = key;
   elem->val = val;
   self->numElem++;
   self->pEnd++;
}

void adt_u16Map_remove(adt_u16Map_t *self, const adt_u16MapElem_t *pElem){
   if( (self==0) || (self->pBegin==0) || (pElem == 0) ){
      return;
   }
   if( (pElem >= self->pBegin) && (pElem < self->pEnd) ){
      uint32_t i;
      uint32_t j;
      i = (uint32_t) (pElem-self->pBegin);
      assert( i < self->numElem);
      //remove element by copy data left
      for(j=i+1;j < self->numElem ; j++){
         self->pBegin[j-1]=self->pBegin[j];
      }
      assert(j == self->numElem);
      self->numElem--;
      self->pEnd--;
   }
}

adt_u16MapElem_t* adt_u16Map_find(adt_u16Map_t *self, uint16_t key){
   adt_u16MapElem_t *it = adt_u16Map_binarySearchDup(self->pBegin,self->pEnd, key);
   if(it != 0){
      (void) adt_u16Map_iterInit(self,it);
   }
   return it;
}

adt_u16MapElem_t* adt_u16Map_findExact(adt_u16Map_t *self, uint16_t key, const void *val){
   adt_u16MapElem_t *it = adt_u16Map_find(self,key);
   if(it != 0){
      it = adt_u16Map_iterInit(self,it);
      while( (it!=0) && (it->key == key) ){
         if(it->val == val ){
            return it;
         }
         it=adt_u16Map_iterNext(self);
      }
   }
   return (adt_u16MapElem_t*) 0;
}

uint16_t adt_u16Map_size(const adt_u16Map_t *self){
   if(self != 0){
      return self->numElem;
   }
   return 0;
}

adt_u16MapElem_t* adt_u16Map_iterInit(adt_u16Map_t *self, const adt_u16MapElem_t *pElem){
   if(self->pBegin < self->pEnd){
      if( (pElem >= self->pBegin) && (pElem < self->pEnd) ){
         self->pIter = pElem;
      }
      else{
         self->pIter = self->pBegin;
      }
      return self->pIter;
   }
   return (adt_u16MapElem_t*)0;
}

adt_u16MapElem_t* adt_u16Map_iterNext(adt_u16Map_t *self){
   if(self->pIter < self->pEnd){
      ++self->pIter;
      if(self->pIter < self->pEnd){
         return self->pIter;
      }
   }
   return (adt_u16MapElem_t*)0;
}

/**
 * move all items matching key \param key from \param src to \param dest
 */
uint16_t adt_u16Map_moveElem(adt_u16Map_t *dest, adt_u16Map_t *src, uint16_t key){
   adt_u16MapElem_t *itemsToMove[MAX_NUM_TRANSFER];
   adt_u16MapElem_t dataToMove[MAX_NUM_TRANSFER];
   adt_u16MapElem_t **pCursor;
   adt_u16MapElem_t *pData;
   uint16_t numItems = 0;
   uint16_t totalItems = 0;

   if( (dest == 0) || (dest->pBegin==0) || (src==0) || (src->pBegin==0) ){
      return 0;
   }

   while(1){
      adt_u16MapElem_t *it;
      pCursor = itemsToMove;
      pData = dataToMove;
      it = adt_u16Map_find(src,key);
      if(it != (adt_u16MapElem_t*) 0){
         it=adt_u16Map_iterInit(src,it);
         while(numItems < MAX_NUM_TRANSFER){
            if( (it == (adt_u16MapElem_t*) 0) || (it->key != key) ){
               break; //no more items
            }
            else{
               //mark items for removal, copy data
               *(pCursor++) = it;
               *(pData++) = *it;
               numItems++;
            }
            it = adt_u16Map_iterNext(src);
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

void adt_u16Map_removeVal(adt_u16Map_t *self, const void *val){
   uint16_t tmp[MAX_NUM_TRANSFER];
   uint16_t tmpLen = 0;
   adt_u16MapElem_t *it;
   while(1){
      tmpLen = 0;
      it = adt_u16Map_iterInit(self,0);
      while((tmpLen < MAX_NUM_TRANSFER) && it){
         if(it->val == val){
            tmp[tmpLen++] = it->key;
         }
         it = adt_u16Map_iterNext(self);
      }
      if(tmpLen>0){
         uint16_t i;
         for(i=0;i<tmpLen;i++){
            it = adt_u16Map_findExact(self,tmp[i],val);
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


/***************** Private Function Definitions *******************/
adt_u16MapElem_t *adt_u16Map_binarySearchDup(const adt_u16MapElem_t *pBegin, const adt_u16MapElem_t *pEnd, uint16_t key){
   const adt_u16MapElem_t *pMid;
   const adt_u16MapElem_t *pLow = pBegin;
   const adt_u16MapElem_t *pHigh = pEnd;

   uint32_t numElem;
   while(1){
      assert(pHigh >= pLow);
      numElem = pHigh-pLow;
      //perform a linear search if there is 3 items or less
      if(numElem <= 3){
         uint16_t i;
         for(i=0;i<numElem;i++){
            if( (pLow[i].key == key) ){
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
         pMid = pLow+(numElem/2);
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
   return (adt_u16MapElem_t *) 0;
}

