/*****************************************************************************
* \file      adt_heap.h
* \author    Conny Gustafsson
* \date      2017-06-07
* \brief     Binary heap / priority queue implementation on adt_ary
*
* Copyright (c) 2017-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_PQ_H
#define ADT_PQ_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "adt_ary.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Element stored in a binary heap.
 */
typedef struct adt_priorityHeap_elem_tag
{
   void *pItem;       /**< Weak pointer to user-defined data or object. */
   uint32_t u32Value; /**< Priority / sort key value. */
} adt_heap_elem_t;

/**
 * \brief Ordering policy for binary heap algorithms.
 */
typedef enum adt_heap_family_tag{
   ADT_MIN_HEAP=0, /**< Min-heap: lowest value at the root (index 0). */
   ADT_MAX_HEAP    /**< Max-heap: highest value at the root (index 0). */
} adt_heap_family;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Allocates and initializes a new heap element on the heap.
 *
 * \param pItem User data pointer (weak reference).
 * \param u32Value Priority or sort key value.
 * \return Pointer to newly allocated heap element, or NULL on failure.
 */
adt_heap_elem_t* adt_heap_elem_new(void *pItem, uint32_t u32Value);

/**
 * \brief Destroys and frees a heap element.
 *
 * \param self Pointer to the element to delete.
 */
void adt_heap_elem_delete(adt_heap_elem_t *self);

/**
 * \brief Type-erased destructor wrapper for adt_heap_elem_delete.
 *
 * Suitable for use as the destructor callback in the backing adt_ary_t container.
 *
 * \param arg Pointer to the element (cast to void*).
 */
void adt_heap_elem_vdelete(void *arg);

/**
 * \brief Initializes a heap element in place (stack allocation).
 *
 * \param self Pointer to an existing adt_heap_elem_t instance.
 * \param pItem User data pointer.
 * \param u32Value Priority or sort key value.
 */
void adt_heap_elem_create(adt_heap_elem_t *self, void *pItem, uint32_t u32Value);

/**
 * \brief Destroys a heap element. Does not free self.
 *
 * \param self Pointer to the element to destroy.
 */
void adt_heap_elem_destroy(adt_heap_elem_t *self);

/**
 * \brief Sifts an element upward toward the root to restore heap invariants.
 *
 * Call this after appending a new element to the end of the backing array.
 *
 * \param heap Pointer to the backing adt_ary_t array holding adt_heap_elem_t pointers.
 * \param childIndex Zero-based index of the element to bubble up.
 * \param heapFamily Ordering policy (ADT_MIN_HEAP or ADT_MAX_HEAP).
 */
void adt_heap_sortUp(adt_ary_t *heap, int32_t childIndex, adt_heap_family heapFamily);

/**
 * \brief Sifts an element downward toward the leaves to restore heap invariants.
 *
 * Call this after removing or replacing the root element (index 0).
 *
 * \param heap Pointer to the backing adt_ary_t array holding adt_heap_elem_t pointers.
 * \param parentIndex Zero-based index of the element to trickle down.
 * \param heapFamily Ordering policy (ADT_MIN_HEAP or ADT_MAX_HEAP).
 */
void adt_heap_sortDown(adt_ary_t *heap, int32_t parentIndex, adt_heap_family heapFamily);

#ifdef __cplusplus
}
#endif

#endif //ADT_PQ_H
