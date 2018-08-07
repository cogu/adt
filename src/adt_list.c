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

static void  adt_u32List_elem_create(adt_u32List_elem_t *self, uint32_t item, adt_u32List_elem_t *pNext, adt_u32List_elem_t *pPrev);
static adt_u32List_elem_t*  adt_u32List_elem_new(uint32_t item, adt_u32List_elem_t *pNext, adt_u32List_elem_t *pPrev);
static void  adt_u32List_elem_delete(adt_u32List_elem_t *self);



/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/

/******* adt_list API *************/
void  adt_list_create(adt_list_t *self, void (*pDestructor)(void*))
{
   if (self != 0)
   {
      self->pDestructor = pDestructor;
      self->destructorEnable = true;
      self->pFirst = 0;
      self->pLast = 0;
   }
}

void  adt_list_destroy(adt_list_t *self)
{
   if (self != 0)
   {
      adt_list_elem_t *iter = self->pFirst;
      void (*destructor)(void*) = (void (*)(void*)) 0;
      if (iter == 0)
      {
         return; //empty list
      }
      if ( (self->destructorEnable != false) && (self->pDestructor != 0) )
      {
         destructor = self->pDestructor;
      }
      while( iter != 0 )
      {
         adt_list_elem_t *pNext = iter->pNext;
         if (destructor != 0)
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

void adt_list_destructor_enable(adt_list_t *self, bool enable){
   if(self != 0){
      self->destructorEnable = enable;
   }
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
void adt_list_insert_before(adt_list_t *self, adt_list_elem_t *pIter, void *pItem)
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
void adt_list_insert_after(adt_list_t *self, adt_list_elem_t *pIter, void *pItem)
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

/**
 * Same as adt_list_insert with the exception that it prevents pItem from getting added twice to the list
 */
void adt_list_insert_unique(adt_list_t *self, void *pItem)
{
   if (self != 0)
   {
      adt_list_elem_t *pIter = self->pFirst; //create a local iterator
      while( pIter != 0 )
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
   if (self != 0)
   {
      adt_list_elem_t *iter = self->pFirst; //create a local iterator, we do not want to mess with the users' iterator located in self->pIter
      if (iter == 0)
      {
         return false; //empty list
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
#ifdef UNIT_TEST
            //sanity check, these conditions must be true at all times
            if (self->pFirst == 0) { assert (self->pLast == 0); }
            if (self->pLast == 0) { assert (self->pFirst == 0); }
#endif
            //OK, now delete element
            adt_list_elem_delete(iter);
            return true;
         }
         iter=iter->pNext;
      }
   }
   return false;
}

bool adt_list_is_empty(const adt_list_t *self)
{
   if (self != 0)
   {
#ifdef UNIT_TEST
      if (self->pFirst == 0) { assert (self->pLast == 0); }
      if (self->pLast == 0) { assert (self->pFirst == 0); }
#endif
      return (bool) (self->pFirst == 0);
   }
   return false;
}


void *adt_list_first(const adt_list_t *self)
{
   if ( (self != 0) && (self->pFirst != 0))
   {
      return self->pFirst->pItem;
   }
   return 0;
}

void* adt_list_last(const adt_list_t *self)
{
   if ( (self != 0) && (self->pLast != 0) )
   {
      return self->pLast->pItem;
   }
   return 0;
}

adt_list_elem_t *adt_list_iter_first(adt_list_t *self)
{
   if (self != 0)
   {
      return self->pFirst;
   }
   return 0;
}

adt_list_elem_t *adt_list_iter_last(adt_list_t *self)
{
   if (self != 0)
   {
      return self->pLast;
   }
   return 0;
}

adt_list_elem_t *adt_list_iter_next(adt_list_elem_t *pElem)
{
   if (pElem != 0)
   {
      return pElem->pNext;
   }
   return 0;
}

adt_list_elem_t *adt_list_iter_prev(adt_list_elem_t *pElem)
{
   if (pElem != 0)
   {
      return pElem->pPrev;
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

void adt_list_clear(adt_list_t *self)
{
   if (self != 0)
   {
      adt_list_destroy(self);
      self->pFirst = 0;
      self->pLast = 0;
   }
}

/******* adt_u32List API *************/
void  adt_u32List_create(adt_u32List_t *self)
{
   if (self != 0)
   {
      self->pFirst = 0;
      self->pLast = 0;
   }
}

void  adt_u32List_destroy(adt_u32List_t *self)
{
   if (self != 0)
   {
      adt_u32List_elem_t *iter = self->pFirst;
      if (iter == 0)
      {
         return; //empty list
      }
      while( iter != 0 )
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
   if (self != 0)
   {
      adt_u32List_create(self);
   }
   else
   {
      errno = ENOMEM;
   }
   return self;
}

void adt_u32List_delete(adt_u32List_t *self)
{
   if (self != 0)
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
   if (self != 0)
   {
      adt_u32List_elem_t *elem = adt_u32List_elem_new(item, 0, self->pLast);
      if (elem != 0)
      {
         if (self->pLast==0)
         {
            assert(self->pFirst == 0); //list must be empty if pLast is NULL
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
   if( (self != 0) && (pIter != 0) )
   {
      adt_u32List_elem_t *pElem = adt_u32List_elem_new(item, pIter, pIter->pPrev);
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

void adt_u32List_insert_after(adt_u32List_t *self, adt_u32List_elem_t *pIter, uint32_t item)
{
   if( (self != 0) && (pIter != 0) )
   {
      adt_u32List_elem_t *pElem = adt_u32List_elem_new(item,pIter->pNext, pIter);
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

/**
 * Removes the item pointed to by the iterator. The iterator must be pointing to an element in this list (obviously).
 */
void adt_u32List_erase(adt_u32List_t *self, adt_u32List_elem_t *pIter)
{
   if ( (self != 0) && (pIter != 0) )
   {
      //disconnect element from list
      if (pIter->pPrev != 0)
      {
         pIter->pPrev->pNext = pIter->pNext;
      }
      if (pIter->pNext != 0)
      {
         pIter->pNext->pPrev = pIter->pPrev;
      }
      if (pIter == self->pFirst)
      {
         //we are removing the first element, adjust pFirst
         self->pFirst = pIter->pNext;
         if (self->pFirst == 0)
         {
            self->pLast = 0; //if pFirst becomes 0, then pLast must also be set to 0
         }
      }
      if (pIter == self->pLast)
      {
         //last element was removed adjust pLast
         self->pLast = pIter->pPrev;
      }
#ifdef UNIT_TEST
      //sanity check, these conditions must be true at all times
      if (self->pFirst == 0) { assert (self->pLast == 0); }
      if (self->pLast == 0) { assert (self->pFirst == 0); }
#endif
      adt_u32List_elem_delete(pIter);
   }
}

bool adt_u32List_is_empty(const adt_u32List_t *self)
{
   if (self != 0)
   {
#ifdef UNIT_TEST
      if (self->pFirst == 0) { assert (self->pLast == 0); }
      if (self->pLast == 0) { assert (self->pFirst == 0); }
#endif
      return (bool) (self->pFirst == 0);
   }
   return false;
}

adt_u32List_elem_t *adt_u32List_iter_first(const adt_u32List_t *self)
{
   if (self != 0)
   {
      return self->pFirst;
   }
   return 0;
}

adt_u32List_elem_t *adt_u32List_iter_last(const adt_u32List_t *self)
{
   if (self != 0)
   {
      return self->pLast;
   }
   return 0;
}

adt_u32List_elem_t* adt_u32List_iter_next(adt_u32List_elem_t *pElem)
{
   if (pElem != 0)
   {
      return pElem->pNext;
   }
   return 0;
}

adt_u32List_elem_t* adt_u32List_iter_prev(adt_u32List_elem_t *pElem)
{
   if (pElem != 0)
   {
      return pElem->pPrev;
   }
   return 0;
}

int32_t adt_u32List_length(const adt_u32List_t *self)
{
   int32_t result=0;
   if (self != 0)
   {
      adt_u32List_elem_t *iter = self->pFirst;
      while(iter != 0)
      {
         ++result;
         iter = iter->pNext;
      }
   }
   return result;
}

void adt_u32List_clear(adt_u32List_t *self)
{
   if (self != 0)
   {
      adt_u32List_destroy(self);
      self->pFirst = 0;
      self->pLast = 0;
   }
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

static void  adt_u32List_elem_create(adt_u32List_elem_t *self, uint32_t item, adt_u32List_elem_t *pNext, adt_u32List_elem_t *pPrev)
{
   if (self != 0)
   {
      self->item = item;
      self->pNext = pNext;
      self->pPrev = pPrev;
   }
}

static adt_u32List_elem_t*  adt_u32List_elem_new(uint32_t item, adt_u32List_elem_t *pNext, adt_u32List_elem_t *pPrev)
{
   adt_u32List_elem_t *self = (adt_u32List_elem_t*) malloc(sizeof(adt_u32List_elem_t));
   if (self != 0)
   {
      adt_u32List_elem_create(self,item,pNext,pPrev);
   }
   else
   {
      errno = ENOMEM;
   }
   return self;
}

static void  adt_u32List_elem_delete(adt_u32List_elem_t *self)
{
   if (self != 0)
   {
      free(self);
   }
}
