//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_heap.h"
#include <stdbool.h>
#include <assert.h>
#include <malloc.h>
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
// GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// LOCAL VARIABLES
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
adt_heap_elem_t* adt_heap_elem_new(void *pItem, uint32_t u32Value){
   adt_heap_elem_t *self;
   self = (adt_heap_elem_t*) malloc(sizeof(adt_heap_elem_t));
   if ( self != (adt_heap_elem_t*) 0 ){
      adt_heap_elem_create(self, pItem, u32Value);
   }
   return self;
}

void adt_heap_elem_delete(adt_heap_elem_t *self){
   if (self != 0){
      adt_heap_elem_destroy(self);
      free(self);
   }
}

void adt_heap_elem_vdelete(void *arg){
   adt_heap_elem_delete((adt_heap_elem_t*) arg);
}

void adt_heap_elem_create(adt_heap_elem_t *self, void *pItem, uint32_t u32Value){
   if (self != 0){
      self->pItem = pItem;
      self->u32Value = u32Value;
   }
}

void adt_heap_elem_destroy(adt_heap_elem_t *self){
   (void) self;
   //nothing to do (self->pItem is a weak reference)
}


/**
 * resorts the heap starting at startIndex. It propagates upwards (bubble up) toward the root of the tree.
 */
void adt_heap_sortUp(adt_ary_t *heap, int32_t childIndex, adt_heap_family heapFamily){
   if ( (heap != 0) && (childIndex>0) ){
      bool swapItems = false;
      void **ppChild, **ppParent;
      adt_heap_elem_t *pChildElem, *pParentElem;
      //parent index is at floor((childIndex-1)/2)
      int32_t parentIndex = (childIndex-1)>>1;

      ppChild = adt_ary_get(heap, childIndex);
      ppParent = adt_ary_get(heap, parentIndex);
      assert ( (ppChild != 0) && (ppParent != 0) );
      pChildElem = *ppChild;
      pParentElem = *ppParent;
      if (heapFamily == ADT_MIN_HEAP){
         if (pChildElem->u32Value < pParentElem->u32Value){
            swapItems = true;
         }
      }
      else {
         //implicit ADT_MAX_HEAP
         if (pChildElem->u32Value > pParentElem->u32Value){
            swapItems = true;
         }
      }
      if (swapItems == true){
         //swap parent and child
         void *tmp;
         tmp = *ppChild;
         *ppChild = *ppParent;
         *ppParent = tmp;

         //call recursively on parent (bubble up)
         if (parentIndex > 0) {
            adt_heap_sortUp(heap, parentIndex, heapFamily);
         }
      }
   }
}


/**
 * resorts the list starting at startIndex. It propagates downwards toward the leaf elements of the tree.
 */
void adt_heap_sortDown(adt_ary_t *heap, int32_t parentIndex, adt_heap_family heapFamily){
   if ( (heap != 0) && (parentIndex>=0) ){
      int32_t curLen = adt_ary_length(heap);
      if (parentIndex < curLen ){
         bool swapLeft = false, swapRight = false;
         void **ppLeft = 0, **ppRight = 0, **ppParent;
         adt_heap_elem_t *pParentElem;
         int32_t leftChildIndex, rightChildIndex;
         uint32_t leftChildValue = 0;

         ppParent = adt_ary_get(heap, parentIndex);
         assert(ppParent != 0);
         pParentElem = *ppParent;
         //children indices at 2i+1, 2i+2
         leftChildIndex = (parentIndex<<1) + 1;
         rightChildIndex = leftChildIndex + 1;
         if(leftChildIndex < curLen){
            adt_heap_elem_t *pLeftElem;
            ppLeft = adt_ary_get(heap, leftChildIndex);
            assert(ppLeft != 0);
            pLeftElem = *ppLeft;
            leftChildValue = pLeftElem->u32Value;
            if (heapFamily == ADT_MIN_HEAP){
               if (leftChildValue < pParentElem->u32Value){
                  swapLeft = true;
               }
            }
            else {
               //implicit ADT_MAX_HEAP
               if (leftChildValue > pParentElem->u32Value){
                  swapLeft = true;
               }
            }
         }
         if( rightChildIndex < curLen ){
            adt_heap_elem_t *pRightElem;
            ppRight = adt_ary_get(heap, rightChildIndex);
            assert(ppRight != 0);
            pRightElem = *ppRight;
            if (heapFamily == ADT_MIN_HEAP){
               if (pRightElem->u32Value < pParentElem->u32Value){
                  swapRight = true;
               }
            }
            else {
               //implicit ADT_MAX_HEAP
               if (pRightElem->u32Value > pParentElem->u32Value){
                  swapRight = true;
               }
            }
            if ( (swapLeft == true) && (swapRight == true) ){
               //compare which of the two children has the lowest priority
               if (heapFamily == ADT_MIN_HEAP){
                  if (pRightElem->u32Value < leftChildValue){
                     swapLeft = false;
                  }
               }
               else {
                  //implicit ADT_MAX_HEAP
                  if (pRightElem->u32Value > leftChildValue){
                     swapLeft = false;
                  }
               }
            }
         }
         if (swapLeft == true){
            //swap parent and left child
            void *tmp;
            tmp = *ppLeft;
            *ppLeft = *ppParent;
            *ppParent = tmp;

            //call recursively on left child (trickle down)
            if ( ( (leftChildIndex<<1) + 1) < curLen) {
               adt_heap_sortDown(heap, leftChildIndex, heapFamily);
            }
         }
         else if (swapRight == true){
            //swap parent and left child
            void *tmp;
            tmp = *ppRight;
            *ppRight = *ppParent;
            *ppParent = tmp;

            //call recursively on right child (trickle down)
            if ( ( (rightChildIndex<<1) + 1) < curLen) {
               adt_heap_sortDown(heap, rightChildIndex, heapFamily);
            }
         }
         else{
            //do nothing
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////



