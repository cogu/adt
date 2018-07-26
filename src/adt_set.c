//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <malloc.h>
#include <errno.h>
#include "adt_set.h"
#ifdef MEM_LEAK_CHECK
# include "CMemLeak.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void adt_u32Set_create(adt_u32Set_t *self)
{
   if (self != 0)
   {
      adt_u32List_create(&self->list);
   }
}

void adt_u32Set_destroy(adt_u32Set_t *self)
{
   if (self != 0)
   {
      adt_u32List_destroy(&self->list);
   }
}

adt_u32Set_t*  adt_u32Set_new(void)
{
   adt_u32Set_t *self = (adt_u32Set_t*) malloc(sizeof(adt_u32Set_t));
   if (self != 0)
   {
      adt_u32Set_create(self);
   }
   else
   {
      errno = ENOMEM;
   }
   return self;
}

void  adt_u32Set_delete(adt_u32Set_t *self)
{
   if (self != 0)
   {
      adt_u32Set_destroy(self);
      free(self);
   }
}

void  adt_u32Set_vdelete(void *arg)
{
   adt_u32Set_delete((adt_u32Set_t*) arg);
}

void adt_u32Set_clear(adt_u32Set_t *self)
{
   if (self != 0)
   {
      adt_u32List_clear(&self->list);
   }
}

int32_t adt_u32Set_length(adt_u32Set_t *self)
{
   if (self != 0)
   {
      return adt_u32List_length(&self->list);
   }
   errno=EINVAL;
   return -1;
}

/**
 * inserts val into the internal list. The internal list is automatically sorted
 */
void adt_u32Set_insert(adt_u32Set_t *self, uint32_t val)
{
   if (self != 0)
   {
      adt_u32List_elem_t *iter = adt_u32List_iter_last(&self->list);
      if ( (iter == 0) ||  (val > iter->item) )
      {
         adt_u32List_insert(&self->list, val);
      }
      else if (val != iter->item)
      {
         iter = adt_u32List_iter_first(&self->list);
         while (iter != 0)
         {
            if (val == iter->item)
            {
               break; //prevent duplicate
            }
            else if (val < iter->item)
            {
               adt_u32List_insert_before(&self->list, iter, val);
               break;
            }
            iter = adt_u32List_iter_next(iter);
         }
      }
   }
}

bool adt_u32Set_remove(adt_u32Set_t *self, uint32_t val)
{
   bool retval = false;
   if (self != 0)
   {
      adt_u32List_elem_t *iter = adt_u32List_iter_last(&self->list);
      if (iter != 0)
      {
         if ( val == iter->item )
         {
            retval = true;
         }
         else if (val < iter->item)
         {
            iter = adt_u32List_iter_first(&self->list);
            while (iter != 0)
            {
               if ( val == iter->item )
               {
                  retval = true;
                  break;
               }
               else if (val < iter->item)
               {
                  break;
               }
               iter = adt_u32List_iter_next(iter);
            }
         }
      }
      if (retval == true)
      {
         adt_u32List_erase(&self->list, iter);
      }
   }
   return retval;
}

bool adt_u32Set_contains(adt_u32Set_t *self, uint32_t val)
{
   bool retval = false;
   if (self != 0)
   {
      adt_u32List_elem_t *iter = adt_u32List_iter_last(&self->list);
      if (iter != 0)
      {
         if ( val == iter->item )
         {
            retval = true;
         }
         else if (val < iter->item)
         {
            iter = adt_u32List_iter_first(&self->list);
            while (iter != 0)
            {
               if ( val == iter->item )
               {
                  retval = true;
                  break;
               }
               else if (val < iter->item)
               {
                  break;
               }
               iter = adt_u32List_iter_next(iter);
            }
         }
      }
   }
   return retval;
}

bool adt_u32Set_is_empty(const adt_u32Set_t *self)
{
   if (self != 0)
   {
      return adt_u32List_is_empty(&self->list);
   }
   return false;
}


