/*****************************************************************************
* \file      adt_list.c
* \author    Conny Gustafsson
* \date      2017-01-27
* \brief     Doubly-linked list data structure
*
* Copyright (c) 2017-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <malloc.h>
#include <stddef.h>
#include "adt_list.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void  adt_list_elem_create(adt_list_elem_t *self, void* pItem, adt_list_elem_t *pNext, adt_list_elem_t *pPrev);
static adt_list_elem_t*  adt_list_elem_new(void* pItem, adt_list_elem_t *pNext, adt_list_elem_t *pPrev);
static void  adt_list_elem_delete(adt_list_elem_t *self);

static void  adt_u32List_elem_create(adt_u32List_elem_t *self, uint32_t item, adt_u32List_elem_t *pNext, adt_u32List_elem_t *pPrev);
static adt_u32List_elem_t*  adt_u32List_elem_new(uint32_t item, adt_u32List_elem_t *pNext, adt_u32List_elem_t *pPrev);
static void  adt_u32List_elem_delete(adt_u32List_elem_t *self);



//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

/******* adt_list API *************/
void  adt_list_create(adt_list_t *self, void (*pDestructor)(void*))
{
   if (self != NULL)
   {
      self->pDestructor = pDestructor;
      self->destructorEnable = true;
      self->pFirst = NULL;
      self->pLast = NULL;
   }
}

void  adt_list_destroy(adt_list_t *self)
{
   if (self != NULL)
   {
      adt_list_elem_t *iter = self->pFirst;
      void (*destructor)(void*) = NULL;
      if (iter == NULL)
      {
         return; //empty list
      }
      if ( (self->destructorEnable != false) && (self->pDestructor != NULL) )
      {
         destructor = self->pDestructor;
      }
      while( iter != NULL )
      {
         adt_list_elem_t *pNext = iter->pNext;
         if (destructor != NULL)
         {
            destructor(iter->pItem);
         }
         adt_list_elem_delete(iter);
         iter=pNext;
      }
   }
}

adt_list_t*  adt_list_new(void (*pDestructor)(void*))
{
   adt_list_t *self = (adt_list_t*) malloc(sizeof(adt_list_t));
   if (self != NULL)
   {
      adt_list_create(self,pDestructor);
   }
   return self;
}

void  adt_list_delete(adt_list_t *self)
{
   if (self != NULL)
   {
      adt_list_destroy(self);
      free(self);
   }
}

void  adt_list_vdelete(void *arg)
{
   adt_list_delete((adt_list_t*) arg);
}

void adt_list_destructor_enable(adt_list_t *self, bool enable){
   if(self != NULL){
      self->destructorEnable = enable;
   }
}


/**
 * inserts at end of the list
 */
void adt_list_insert(adt_list_t *self, void *pItem)
{
   if (self != NULL)
   {
      adt_list_elem_t *elem = adt_list_elem_new(pItem, NULL, self->pLast);
      if (elem != NULL)
      {
         if (self->pLast == NULL)
         {
            assert(self->pFirst == NULL); //list must be empty if pLast is NULL
            self->pLast = elem;
            self->pFirst = elem;
         }
         else
         {
            self->pLast->pNext=elem;
            self->pLast=elem;
         }
      }
   }
}

/**
 * insert pItem before pIter
 */
void adt_list_insert_before(adt_list_t *self, adt_list_elem_t *pIter, void *pItem)
{
   if( (self != NULL) && (pIter != NULL) && (pItem != NULL) )
   {
      adt_list_elem_t *pElem = adt_list_elem_new(pItem,pIter, pIter->pPrev);
      if (pElem != NULL)
      {
         if (pIter->pPrev != NULL)
         {
            pIter->pPrev->pNext = pElem;
         }
         pIter->pPrev = pElem;
         //adjust pFirst if we are inserting at beginning of list
         if (self->pFirst == pIter)
         {
            self->pFirst = pElem;
         }
      }
   }
}

/**
 * insert pElem after pIter
 */
void adt_list_insert_after(adt_list_t *self, adt_list_elem_t *pIter, void *pItem)
{
   if( (self != NULL) && (pIter != NULL) && (pItem != NULL) )
   {
      adt_list_elem_t *pElem = adt_list_elem_new(pItem,pIter->pNext, pIter);
      if (pElem != NULL)
      {
         if (pIter->pNext != NULL)
         {
            pIter->pNext->pPrev = pElem;
         }
         pIter->pNext = pElem;
         //adjust pLast in case we are inserting at end of list
         if (pIter == self->pLast)
         {
            self->pLast = pElem;
         }
      }
   }
}

/**
 * Same as adt_list_insert with the exception that it prevents pItem from getting added twice to the list
 */
void adt_list_insert_unique(adt_list_t *self, void *pItem)
{
   if (self != NULL)
   {
      adt_list_elem_t *pIter = self->pFirst; //create a local iterator
      while( pIter != NULL )
      {
         adt_list_elem_t *pNext = pIter->pNext;
         if (pIter->pItem == pItem)
         {
            return; //pItem already in list
         }
         pIter=pNext;
      }
      adt_list_insert(self, pItem);
   }
}

/**
 * returns true if item is found, false otherwise
 */
bool adt_list_remove(adt_list_t *self, void *pItem)
{
   if (self != NULL)
   {
      adt_list_elem_t *iter = adt_list_find(self, pItem);
      if (iter == NULL)
      {
         return false;
      }
      else
      {
         adt_list_erase(self, iter);
         return true;
      }
   }
   return false;
}

bool adt_list_is_empty(const adt_list_t *self)
{
   if (self != NULL)
   {
#ifdef UNIT_TEST
      if (self->pFirst == NULL) { assert (self->pLast == NULL); }
      if (self->pLast == NULL) { assert (self->pFirst == NULL); }
#endif
      return (bool) (self->pFirst == NULL);
   }
   return false;
}


void *adt_list_first(const adt_list_t *self)
{
   if ( (self != NULL) && (self->pFirst != NULL))
   {
      return self->pFirst->pItem;
   }
   return NULL;
}

void* adt_list_last(const adt_list_t *self)
{
   if ( (self != NULL) && (self->pLast != NULL) )
   {
      return self->pLast->pItem;
   }
   return NULL;
}

adt_list_elem_t *adt_list_iter_first(adt_list_t const* self)
{
   if (self != NULL)
   {
      return self->pFirst;
   }
   return NULL;
}

adt_list_elem_t *adt_list_iter_last(adt_list_t const* self)
{
   if (self != NULL)
   {
      return self->pLast;
   }
   return NULL;
}

adt_list_elem_t *adt_list_iter_next(adt_list_elem_t *pElem)
{
   if (pElem != NULL)
   {
      return pElem->pNext;
   }
   return NULL;
}

adt_list_elem_t *adt_list_iter_prev(adt_list_elem_t *pElem)
{
   if (pElem != NULL)
   {
      return pElem->pPrev;
   }
   return NULL;
}

/**
 * Finds an item in the list and returns an iterator if found.
 */
adt_list_elem_t *adt_list_find(const adt_list_t *self, void *pItem)
{
   if (self != NULL)
   {
      adt_list_elem_t *iter = self->pFirst;
      if (iter == NULL)
      {
         return NULL; //empty list
      }
      while(iter != NULL )
      {
         if (iter->pItem == pItem)
         {
            return iter;
         }
         iter=iter->pNext;
      }
   }
   return NULL;
}

/**
 * Removes the item pointed to by the iterator. The iterator must be pointing to an element in this list.
 */
void adt_list_erase(adt_list_t *self, adt_list_elem_t *pIter)
{
   if ( (self != NULL) && (pIter != NULL) )
   {
      //disconnect element from list
      if (pIter->pPrev != NULL)
      {
         pIter->pPrev->pNext = pIter->pNext;
      }
      if (pIter->pNext != NULL)
      {
         pIter->pNext->pPrev = pIter->pPrev;
      }
      if (pIter == self->pFirst)
      {
         //we are removing the first element, adjust pFirst
         self->pFirst = pIter->pNext;
         if (self->pFirst == NULL)
         {
            self->pLast = NULL; //if pFirst becomes 0, then pLast must also be set to 0
         }
      }
      if (pIter == self->pLast)
      {
         //last element was removed adjust pLast
         self->pLast = pIter->pPrev;
      }
#ifdef UNIT_TEST
      //sanity check, these conditions must be true at all times
      if (self->pFirst == NULL) { assert (self->pLast == NULL); }
      if (self->pLast == NULL) { assert (self->pFirst == NULL); }
#endif
      adt_list_elem_delete(pIter);
   }
}

int32_t adt_list_length(const adt_list_t *self)
{
   int32_t result=0;
   if (self != NULL)
   {
      adt_list_elem_t *iter = self->pFirst;
      while(iter != NULL)
      {
         ++result;
         iter = iter->pNext;
      }
   }
   return result;
}

void adt_list_clear(adt_list_t *self)
{
   if (self != NULL)
   {
      adt_list_destroy(self);
      self->pFirst = NULL;
      self->pLast = NULL;
   }
}

/******* adt_u32List API *************/
void  adt_u32List_create(adt_u32List_t *self)
{
   if (self != NULL)
   {
      self->pFirst = NULL;
      self->pLast = NULL;
   }
}

void  adt_u32List_destroy(adt_u32List_t *self)
{
   if (self != NULL)
   {
      adt_u32List_elem_t *iter = self->pFirst;
      if (iter == NULL)
      {
         return; //empty list
      }
      while( iter != NULL )
      {
         adt_u32List_elem_t *pNext = iter->pNext;
         adt_u32List_elem_delete(iter);
         iter=pNext;
      }
   }
}

adt_u32List_t* adt_u32List_new(void)
{
   adt_u32List_t *self = (adt_u32List_t*) malloc(sizeof(adt_u32List_t));
   if (self != NULL)
   {
      adt_u32List_create(self);
   }
   return self;
}

void adt_u32List_delete(adt_u32List_t *self)
{
   if (self != NULL)
   {
      adt_u32List_destroy(self);
      free(self);
   }
}

void  adt_u32List_vdelete(void *arg)
{
   adt_u32List_delete((adt_u32List_t*) arg);
}

/**
 * inserts \param item at end of list
 */
void adt_u32List_insert(adt_u32List_t *self, uint32_t item)
{
   if (self != NULL)
   {
      adt_u32List_elem_t *elem = adt_u32List_elem_new(item, NULL, self->pLast);
      if (elem != NULL)
      {
         if (self->pLast == NULL)
         {
            assert(self->pFirst == NULL); //list must be empty if pLast is NULL
            self->pLast = self->pFirst = elem;
         }
         else
         {
            self->pLast->pNext=elem;
            self->pLast=elem;
         }
      }
   }
}

void adt_u32List_insert_before(adt_u32List_t *self, adt_u32List_elem_t *pIter, uint32_t item)
{
   if( (self != NULL) && (pIter != NULL) )
   {
      adt_u32List_elem_t *pElem = adt_u32List_elem_new(item, pIter, pIter->pPrev);
      if (pElem != NULL)
      {
         if (pIter->pPrev != NULL)
         {
            pIter->pPrev->pNext = pElem;
         }
         pIter->pPrev = pElem;
         //adjust pFirst if we are inserting at beginning of list
         if (self->pFirst == pIter)
         {
            self->pFirst = pElem;
         }
      }
   }
}

void adt_u32List_insert_after(adt_u32List_t *self, adt_u32List_elem_t *pIter, uint32_t item)
{
   if( (self != NULL) && (pIter != NULL) )
   {
      adt_u32List_elem_t *pElem = adt_u32List_elem_new(item,pIter->pNext, pIter);
      if (pElem != NULL)
      {
         if (pIter->pNext != NULL)
         {
            pIter->pNext->pPrev = pElem;
         }
         pIter->pNext = pElem;
         //adjust pLast in case we are inserting at end of list
         if (pIter == self->pLast)
         {
            self->pLast = pElem;
         }
      }
   }
}

/**
 * Removes the item pointed to by the iterator. The iterator must be pointing to an element in this list (obviously).
 */
void adt_u32List_erase(adt_u32List_t *self, adt_u32List_elem_t *pIter)
{
   if ( (self != NULL) && (pIter != NULL) )
   {
      //disconnect element from list
      if (pIter->pPrev != NULL)
      {
         pIter->pPrev->pNext = pIter->pNext;
      }
      if (pIter->pNext != NULL)
      {
         pIter->pNext->pPrev = pIter->pPrev;
      }
      if (pIter == self->pFirst)
      {
         //we are removing the first element, adjust pFirst
         self->pFirst = pIter->pNext;
         if (self->pFirst == NULL)
         {
            self->pLast = NULL; //if pFirst becomes 0, then pLast must also be set to 0
         }
      }
      if (pIter == self->pLast)
      {
         //last element was removed adjust pLast
         self->pLast = pIter->pPrev;
      }
#ifdef UNIT_TEST
      //sanity check, these conditions must be true at all times
      if (self->pFirst == NULL) { assert (self->pLast == NULL); }
      if (self->pLast == NULL) { assert (self->pFirst == NULL); }
#endif
      adt_u32List_elem_delete(pIter);
   }
}

bool adt_u32List_is_empty(const adt_u32List_t *self)
{
   if (self != NULL)
   {
#ifdef UNIT_TEST
      if (self->pFirst == NULL) { assert (self->pLast == NULL); }
      if (self->pLast == NULL) { assert (self->pFirst == NULL); }
#endif
      return (bool) (self->pFirst == NULL);
   }
   return false;
}

adt_u32List_elem_t *adt_u32List_iter_first(const adt_u32List_t *self)
{
   if (self != NULL)
   {
      return self->pFirst;
   }
   return NULL;
}

adt_u32List_elem_t *adt_u32List_iter_last(const adt_u32List_t *self)
{
   if (self != NULL)
   {
      return self->pLast;
   }
   return NULL;
}

adt_u32List_elem_t* adt_u32List_iter_next(adt_u32List_elem_t *pElem)
{
   if (pElem != NULL)
   {
      return pElem->pNext;
   }
   return NULL;
}

adt_u32List_elem_t* adt_u32List_iter_prev(adt_u32List_elem_t *pElem)
{
   if (pElem != NULL)
   {
      return pElem->pPrev;
   }
   return NULL;
}

/**
 * Finds an item in the list and returns an iterator if found.
 */
adt_u32List_elem_t* adt_u32List_find(const adt_u32List_t *self, uint32_t item)
{
   if (self != NULL)
   {
      adt_u32List_elem_t *iter = self->pFirst;
      if (iter == NULL)
      {
         return NULL; //empty list
      }
      while(iter != NULL )
      {
         if (iter->item == item)
         {
            return iter;
         }
         iter=iter->pNext;
      }
   }
   return NULL;
}

int32_t adt_u32List_length(const adt_u32List_t *self)
{
   int32_t result=0;
   if (self != NULL)
   {
      adt_u32List_elem_t *iter = self->pFirst;
      while(iter != NULL)
      {
         ++result;
         iter = iter->pNext;
      }
   }
   return result;
}

void adt_u32List_clear(adt_u32List_t *self)
{
   if (self != NULL)
   {
      adt_u32List_destroy(self);
      self->pFirst = NULL;
      self->pLast = NULL;
   }
}


//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void  adt_list_elem_create(adt_list_elem_t *self, void *pItem, adt_list_elem_t *pNext, adt_list_elem_t *pPrev)
{
   if (self != NULL)
   {
      self->pItem = pItem;
      self->pNext = pNext;
      self->pPrev = pPrev;
   }
}

adt_list_elem_t*  adt_list_elem_new(void *pItem, adt_list_elem_t *pNext, adt_list_elem_t *pPrev)
{
   adt_list_elem_t *self = (adt_list_elem_t*) malloc(sizeof(adt_list_elem_t));
   if (self != NULL)
   {
      adt_list_elem_create(self,pItem,pNext,pPrev);
   }
   return self;
}

void adt_list_elem_delete(adt_list_elem_t *self)
{
   if (self != NULL)
   {
      free(self);
   }
}

static void  adt_u32List_elem_create(adt_u32List_elem_t *self, uint32_t item, adt_u32List_elem_t *pNext, adt_u32List_elem_t *pPrev)
{
   if (self != NULL)
   {
      self->item = item;
      self->pNext = pNext;
      self->pPrev = pPrev;
   }
}

static adt_u32List_elem_t*  adt_u32List_elem_new(uint32_t item, adt_u32List_elem_t *pNext, adt_u32List_elem_t *pPrev)
{
   adt_u32List_elem_t *self = (adt_u32List_elem_t*) malloc(sizeof(adt_u32List_elem_t));
   if (self != NULL)
   {
      adt_u32List_elem_create(self,item,pNext,pPrev);
   }
   return self;
}

static void  adt_u32List_elem_delete(adt_u32List_elem_t *self)
{
   if (self != NULL)
   {
      free(self);
   }
}
