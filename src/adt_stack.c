/*****************************************************************************
* \file      adt_stack.c
* \author    Conny Gustafsson
* \date      2013-07-21
* \brief     stack data structure
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include "adt_stack.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define ADT_STACK_DEFAULT_MIN_LEN 8

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

//Constructor/Destructor
adt_stack_t* adt_stack_new(void (*pDestructor)(void*)){
   adt_stack_t *self;
   if((self = (adt_stack_t*) malloc(sizeof(adt_stack_t))) == NULL){
      return NULL;
   }
   adt_stack_create(self,pDestructor);
   return self;
}

void adt_stack_delete(adt_stack_t *self){
   if(self){
      adt_stack_destroy(self);
      free(self);
   }
}

void adt_stack_create(adt_stack_t *self, void (*pDestructor)(void*)){
   self->ppAlloc = NULL;
   self->u32AllocLen = 0;
   self->u32CurLen = 0;
   self->u32MinLen = ADT_STACK_DEFAULT_MIN_LEN;
   self->pDestructor = pDestructor;
}

void adt_stack_destroy(adt_stack_t *self){
   uint32_t u32i;

   void **ppElem=self->ppAlloc;
   if(self->pDestructor){
      for(u32i=0;u32i<self->u32CurLen;u32i++){
         self->pDestructor(*(ppElem++));
      }
   }
   if(self->ppAlloc != NULL){
      free(self->ppAlloc);
   }
   self->ppAlloc = NULL;
   self->u32AllocLen = 0;
   self->u32CurLen = 0;
}

void adt_stack_clear(adt_stack_t *self){
   adt_stack_destroy(self);
}


//Accessors
void	adt_stack_push(adt_stack_t *self, void *pVal){
   if(!self) return;
   if(self->u32CurLen==self->u32AllocLen){
      if(self->u32AllocLen==0){
         adt_stack_resize(self,self->u32MinLen);
      }
      else{
         //default growth by doubling items available
         adt_stack_resize(self,self->u32AllocLen*2);
      }
   }
   assert(self->u32CurLen<self->u32AllocLen);
   assert(self->ppAlloc != NULL);
   self->ppAlloc[self->u32CurLen++] = pVal;
}
void* adt_stack_top(const adt_stack_t *self){
   if(self && (self->u32CurLen>0)){
      assert(self->ppAlloc);
      return self->ppAlloc[self->u32CurLen-1];
   }
   return NULL;
}

void* adt_stack_pop(adt_stack_t *self){
   if(self && (self->u32CurLen>0)){
      assert(self->ppAlloc);
      return self->ppAlloc[--self->u32CurLen];
   }
   return NULL;
}

//Utility functions
void adt_stack_reserve(adt_stack_t *self,uint32_t u32Len){
   if(self){
      self->u32MinLen = u32Len;
      if(self->u32AllocLen < self->u32MinLen){
         adt_stack_resize(self,self->u32MinLen);
      }
   }
}

void adt_stack_resize(adt_stack_t *self,uint32_t u32Len){
   void **ppAlloc = NULL;
   if(!self) return;
   if(u32Len == self->u32AllocLen) return; //nothing to do

   assert(self->u32AllocLen>=self->u32CurLen);
   if(u32Len>0){
      ppAlloc = (void**) malloc(u32Len * sizeof(void*));
      assert(ppAlloc != NULL);
   }
   if(self->ppAlloc){
      if(u32Len > self->u32AllocLen){
         //grow
         memcpy(ppAlloc,self->ppAlloc,self->u32AllocLen * sizeof(void*));
      }
      else{
         //shrink
         if( self->u32CurLen > u32Len){
            //call destructor on superfluous elements
            if(self->pDestructor){
               uint32_t u32i;
               for(u32i=u32Len; u32i < self->u32CurLen;u32i++){
                  self->pDestructor(self->ppAlloc[u32i]);
               }
            }
         }
         if(ppAlloc != NULL){
            memcpy(ppAlloc,self->ppAlloc,u32Len * sizeof(void*));
         }
      }
      free(self->ppAlloc);
   }
   self->ppAlloc = ppAlloc;
   self->u32AllocLen = u32Len;
   if (self->u32CurLen > u32Len) {
      self->u32CurLen = u32Len;
   }
}
uint32_t adt_stack_size(const adt_stack_t *self){
   if(self){
      return self->u32CurLen;
   }
   return 0;
}


//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
