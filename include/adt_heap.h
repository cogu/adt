#ifndef ADT_PQ_H
#define ADT_PQ_H

/**
* ADT priority queue implemented as a binary min heap
* The primary purpose of this data structure is for schedulers.
* It can be used to determine the next runnable entity by just looking at
* the top element of the binary min heap.
*
* TODO: implement support for binary max heap later
*/

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "adt_ary.h"

//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
typedef struct adt_priorityHeap_elem_tag
{
   void *pItem; //a weak pointer to a user object
   uint32_t u32Value; //value used for sorting/comparing heap elements
} adt_heap_elem_t;

typedef enum adt_heap_family_tag{
   ADT_MIN_HEAP=0, //lowest element at the top of the tree
   ADT_MAX_HEAP    //highest element at the top of the tree
} adt_heap_family;

//////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/************************* heap ***************************/
adt_heap_elem_t* adt_heap_elem_new(void *pItem, uint32_t u32Value);
void adt_heap_elem_delete(adt_heap_elem_t *self);
void adt_heap_elem_vdelete(void *arg);
void adt_heap_elem_create(adt_heap_elem_t *self, void *pItem, uint32_t u32Value);
void adt_heap_elem_destroy(adt_heap_elem_t *self);
void adt_heap_sortUp(adt_ary_t *heap, int32_t childIndex, adt_heap_family heapFamily);
void adt_heap_sortDown(adt_ary_t *heap, int32_t parentIndex, adt_heap_family heapFamily);


#endif //ADT_PQ_H
