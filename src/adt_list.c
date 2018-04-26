#include <assert.h>
#include <errno.h>
#include <malloc.h>
#include "adt_list.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

/**************** Private Function Declarations *******************/
static void  adt_list_elem_create(adt_list_elem_t *self, void* pItem, adt_list_elem_t *pNext, adt_list_elem_t *pPrev);
static adt_list_elem_t*  adt_list_elem_new(void* pItem, adt_list_elem_t *pNext, adt_list_elem_t *pPrev);
static void  adt_list_elem_delete(adt_list_elem_t *self);


/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
void  adt_list_create(adt_list_t *self, void (*pDestructor)(void*))
{
   if (self != 0)
   {
      self->pDestructor = pDestructor;
      self->pFirst = 0;
      self->pLast = 0;
      self->pIter = 0;
   }
}

void  adt_list_destroy(adt_list_t *self)
{
   if (self != 0)
   {
      adt_list_elem_t *iter = self->pFirst; //create a local iterator, don't mess with the users' iterator located in self->pIter
      if (iter == 0)
      {
         return; //empty list
      }
      while( iter != 0 )
      {
         adt_list_elem_t *pNext = iter->pNext;
         if (self->pDestructor != 0)
         {
            self->pDestructor(iter->pItem);
         }
         adt_list_elem_delete(iter);
         iter=pNext;
      }
   }
}

adt_list_t*  adt_list_new(void (*pDestructor)(void*))
{
   adt_list_t *self = (adt_list_t*) malloc(sizeof(adt_list_t));
   if (self != 0)
   {
      adt_list_create(self,pDestructor);
   }
   else
   {
      errno = ENOMEM;
   }
   return self;

}

void  adt_list_delete(adt_list_t *self)
{
   if (self != 0)
   {
      adt_list_destroy(self);
      free(self);
   }
}

void  adt_list_vdelete(void *arg)
{
   adt_list_delete((adt_list_t*) arg);
}

/**
 * inserts at end of the list
 */
void adt_list_insert(adt_list_t *self, void *pItem)
{
   if (self != 0)
   {
      adt_list_elem_t *elem = adt_list_elem_new(pItem,0,self->pLast);
      if (elem != 0)
      {
         if (self->pLast==0)
         {
            assert(self->pFirst == 0); //list must be empty if pLast is NULL
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
void adt_list_insertBefore(adt_list_t *self, adt_list_elem_t *pIter, void *pItem)
{
   if( (self != 0) && (pIter != 0) && (pItem != 0) )
   {
      adt_list_elem_t *pElem = adt_list_elem_new(pItem,pIter, pIter->pPrev);
      if (pElem != 0)
      {
         if (pIter->pPrev != 0)
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
void adt_list_insertAfter(adt_list_t *self, adt_list_elem_t *pIter, void *pItem)
{
   if( (self != 0) && (pIter != 0) && (pItem != 0) )
   {
      adt_list_elem_t *pElem = adt_list_elem_new(pItem,pIter->pNext, pIter);
      if (pElem != 0)
      {
         if (pIter->pNext != 0)
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

void adt_list_remove(adt_list_t *self, void *pItem)
{
   if (self != 0)
   {
      adt_list_elem_t *iter = self->pFirst; //create a local iterator, we do not want to mess with the users' iterator located in self->pIter
      if (iter == 0)
      {
         return; //empty list
      }
      while( iter != 0 )
      {
         if (iter->pItem == pItem)
         {
            //disconnect element from list
            if (iter->pPrev != 0)
            {
               iter->pPrev->pNext = iter->pNext;
            }
            if (iter->pNext != 0)
            {
               iter->pNext->pPrev = iter->pPrev;
            }
            if (iter == self->pFirst)
            {
               //we are removing the first element, adjust pFirst
               self->pFirst = iter->pNext;
               if (self->pFirst == 0)
               {
                  self->pLast = 0; //if pFirst becomes 0, then pLast must also be set to 0
               }
            }
            if (iter == self->pLast)
            {
               //last element was removed adjust pLast
               self->pLast = iter->pPrev;
            }
            //sanity check, these conditions must be true at all times
            if (self->pFirst == 0) { assert (self->pLast == 0); }
            if (self->pLast == 0) { assert (self->pFirst == 0); }

            //OK, now delete element
            adt_list_elem_delete(iter);
            return;
         }
         iter=iter->pNext;
      }
   }
}

bool adt_list_is_empty(const adt_list_t *self)
{
   if (self != 0)
   {
      return (bool) ((self->pFirst == 0) && (self->pLast));
   }
   return false;
}

void adt_list_iter_init(adt_list_t *self)
{
   if (self != 0)
   {
      self->pIter=self->pFirst;
   }
}

adt_list_elem_t *adt_list_iter_next(adt_list_t *self)
{
   adt_list_elem_t *retval = 0;
   if (self != 0)
   {
      retval = self->pIter;
      if (self->pIter != 0)
      {
         self->pIter = self->pIter->pNext;
      }
   }
   return retval;
}

adt_list_elem_t *adt_list_first(const adt_list_t *self)
{
   if (self != 0)
   {
      return self->pFirst;
   }
   return 0;
}

adt_list_elem_t *adt_list_last(const adt_list_t *self)
{
   if (self != 0)
   {
      return self->pLast;
   }
   return 0;
}

int32_t adt_list_length(const adt_list_t *self)
{
   int32_t result=0;
   if (self != 0)
   {
      adt_list_elem_t *iter = self->pFirst;
      while(iter != 0)
      {
         ++result;
         iter = iter->pNext;
      }
   }
   return result;
}


/***************** Private Function Definitions *******************/
void  adt_list_elem_create(adt_list_elem_t *self, void *pItem, adt_list_elem_t *pNext, adt_list_elem_t *pPrev)
{
   if (self != 0)
   {
      self->pItem = pItem;
      self->pNext = pNext;
      self->pPrev = pPrev;
   }
}

adt_list_elem_t*  adt_list_elem_new(void *pItem, adt_list_elem_t *pNext, adt_list_elem_t *pPrev)
{
   adt_list_elem_t *self = (adt_list_elem_t*) malloc(sizeof(adt_list_elem_t));
   if (self != 0)
   {
      adt_list_elem_create(self,pItem,pNext,pPrev);
   }
   else
   {
      errno = ENOMEM;
   }
   return self;
}

void adt_list_elem_delete(adt_list_elem_t *self)
{
   if (self != 0)
   {
      free(self);
   }
}
