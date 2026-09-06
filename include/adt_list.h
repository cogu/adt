/*****************************************************************************
* \file      adt_list.h
* \author    Conny Gustafsson
* \date      2017-01-27
* \brief     Doubly-linked list data structure
*
* Copyright (c) 2017-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_LIST_H
#define ADT_LIST_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "adt_error.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

#define ADT_LIST_ELEM_HEAD(struct_name, item_type_name) \
      struct struct_name *pNext;\
      struct struct_name *pPrev;\
      item_type_name item;\

#define ADT_LIST_HEAD(type_name) \
      type_name *pFirst;\
      type_name *pLast;\


typedef struct adt_list_elem_tag
{
   struct adt_list_elem_tag *pNext;
   struct adt_list_elem_tag *pPrev;
   void *pItem;
} adt_list_elem_t;

typedef struct adt_u32List_elem_tag
{
   ADT_LIST_ELEM_HEAD(adt_u32List_elem_tag, uint32_t)
} adt_u32List_elem_t;

typedef struct adt_list_tag{
   ADT_LIST_HEAD(adt_list_elem_t)
   void (*pDestructor)(void*);
   bool destructorEnable;
} adt_list_t;

typedef struct adt_u32List_tag{
   ADT_LIST_HEAD(adt_u32List_elem_t)
} adt_u32List_t;


//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/******* adt_list API *************/

/**
 * \brief Initializes a doubly-linked list instance in place (stack or embedded allocation).
 *
 * \param self Pointer to an existing adt_list_t instance.
 * \param pDestructor Optional element destructor callback, or NULL.
 */
void  adt_list_create(adt_list_t *self, void (*pDestructor)(void*));

/**
 * \brief Destroys all elements using the configured destructor and frees all nodes. Does not free self.
 *
 * \param self Pointer to the list to destroy.
 */
void  adt_list_destroy(adt_list_t *self);

/**
 * \brief Allocates and initializes a new doubly-linked list on the heap.
 *
 * \param pDestructor Optional element destructor callback, or NULL.
 * \return Pointer to newly allocated list, or NULL on failure.
 */
adt_list_t*  adt_list_new(void (*pDestructor)(void*));

/**
 * \brief Destroys all elements using the configured destructor, frees all nodes, and frees self.
 *
 * \param self Pointer to the list to delete.
 */
void  adt_list_delete(adt_list_t *self);

/**
 * \brief Type-erased destructor wrapper for adt_list_delete.
 *
 * Suitable for use as a generic destructor callback void (*)(void*) in other ADT containers.
 *
 * \param arg Pointer to the list (cast to void*).
 */
void  adt_list_vdelete(void *arg);

/**
 * \brief Temporarily enables or disables automatic invocation of the element destructor.
 *
 * \param self Pointer to the list.
 * \param enable True to enable element destructor calls, false to suspend them.
 */
void  adt_list_destructor_enable(adt_list_t *self, bool enable);

/**
 * \brief Appends an element pointer to the end of the list.
 *
 * \param self Pointer to the list.
 * \param pItem Element pointer to append.
 * \return Error code (ADT_NO_ERROR on success, ADT_MEM_ERROR on allocation failure, ADT_INVALID_ARGUMENT_ERROR if self is NULL).
 */
adt_error_t adt_list_insert(adt_list_t *self, void *pItem);

/**
 * \brief Inserts an element pointer before the specified list node.
 *
 * \param self Pointer to the list.
 * \param pIter Pointer to existing list node to insert before.
 * \param pItem Element pointer to insert.
 * \return Error code (ADT_NO_ERROR on success, ADT_MEM_ERROR on allocation failure, ADT_INVALID_ARGUMENT_ERROR if self or pIter is NULL).
 */
adt_error_t adt_list_insert_before(adt_list_t *self, adt_list_elem_t *pIter, void *pItem);

/**
 * \brief Inserts an element pointer after the specified list node.
 *
 * \param self Pointer to the list.
 * \param pIter Pointer to existing list node to insert after.
 * \param pItem Element pointer to insert.
 * \return Error code (ADT_NO_ERROR on success, ADT_MEM_ERROR on allocation failure, ADT_INVALID_ARGUMENT_ERROR if self or pIter is NULL).
 */
adt_error_t adt_list_insert_after(adt_list_t *self, adt_list_elem_t *pIter, void *pItem);

/**
 * \brief Appends an element pointer to the end of the list only if it is not already present.
 *
 * \param self Pointer to the list.
 * \param pItem Element pointer to insert.
 * \return Error code (ADT_NO_ERROR on success or duplicate, ADT_MEM_ERROR on allocation failure, ADT_INVALID_ARGUMENT_ERROR if self is NULL).
 */
adt_error_t adt_list_insert_unique(adt_list_t *self, void *pItem);

/**
 * \brief Removes the first occurrence of pItem from the list and calls its destructor if enabled.
 *
 * \param self Pointer to the list.
 * \param pItem Element pointer to remove.
 * \return true if found and removed, false otherwise.
 */
bool adt_list_remove(adt_list_t *self, void *pItem);

/**
 * \brief Checks if the list is empty.
 *
 * \param self Pointer to the list.
 * \return true if empty or self is NULL, false otherwise.
 */
bool adt_list_is_empty(const adt_list_t *self);

/**
 * \brief Returns the element pointer stored in the first node.
 *
 * \param self Pointer to the list.
 * \return First element pointer, or NULL if list is empty.
 */
void *adt_list_first(const adt_list_t *self);

/**
 * \brief Returns the element pointer stored in the last node.
 *
 * \param self Pointer to the list.
 * \return Last element pointer, or NULL if list is empty.
 */
void *adt_list_last(const adt_list_t *self);

/**
 * \brief Returns the first iterator node of the list.
 *
 * \param self Pointer to the list.
 * \return Pointer to first node, or NULL if list is empty.
 */
adt_list_elem_t *adt_list_iter_first(adt_list_t const* self);

/**
 * \brief Returns the last iterator node of the list.
 *
 * \param self Pointer to the list.
 * \return Pointer to last node, or NULL if list is empty.
 */
adt_list_elem_t *adt_list_iter_last(adt_list_t const* self);

/**
 * \brief Advances the iterator to the next node in the list.
 *
 * \param pElem Pointer to current node.
 * \return Pointer to next node, or NULL if at the end.
 */
adt_list_elem_t *adt_list_iter_next(adt_list_elem_t *pElem);

/**
 * \brief Moves the iterator to the previous node in the list.
 *
 * \param pElem Pointer to current node.
 * \return Pointer to previous node, or NULL if at the beginning.
 */
adt_list_elem_t *adt_list_iter_prev(adt_list_elem_t *pElem);

/**
 * \brief Searches for an element pointer in the list and returns its node.
 *
 * \param self Pointer to the list.
 * \param pItem Element pointer to search for.
 * \return Pointer to the matching node, or NULL if not found.
 */
adt_list_elem_t *adt_list_find(const adt_list_t *self, void *pItem);

/**
 * \brief Removes the specified node from the list and destroys its item if the destructor is enabled.
 *
 * \param self Pointer to the list.
 * \param pElem Pointer to node to erase.
 */
void adt_list_erase(adt_list_t *self, adt_list_elem_t *pElem);

/**
 * \brief Returns the number of elements in the list.
 *
 * \param self Pointer to the list.
 * \return Number of elements, or -1 if self is NULL.
 */
int32_t adt_list_length(const adt_list_t *self);

/**
 * \brief Destroys all elements and clears the list to empty.
 *
 * \param self Pointer to the list.
 */
void adt_list_clear(adt_list_t *self);


/******* adt_u32List API *************/

/**
 * \brief Initializes a 32-bit unsigned integer list instance in place.
 *
 * \param self Pointer to an existing adt_u32List_t instance.
 */
void  adt_u32List_create(adt_u32List_t *self);

/**
 * \brief Destroys all nodes in the list. Does not free self.
 *
 * \param self Pointer to the list to destroy.
 */
void  adt_u32List_destroy(adt_u32List_t *self);

/**
 * \brief Allocates and initializes a new 32-bit integer list on the heap.
 *
 * \return Pointer to newly allocated list, or NULL on failure.
 */
adt_u32List_t*  adt_u32List_new(void);

/**
 * \brief Destroys all nodes and frees self.
 *
 * \param self Pointer to the list to delete.
 */
void  adt_u32List_delete(adt_u32List_t *self);

/**
 * \brief Type-erased destructor wrapper for adt_u32List_delete.
 *
 * \param arg Pointer to the list (cast to void*).
 */
void  adt_u32List_vdelete(void *arg);

/**
 * \brief Appends an integer value to the end of the list.
 *
 * \param self Pointer to the list.
 * \param item Value to append.
 * \return Error code (ADT_NO_ERROR on success, ADT_MEM_ERROR on allocation failure, ADT_INVALID_ARGUMENT_ERROR if self is NULL).
 */
adt_error_t adt_u32List_insert(adt_u32List_t *self, uint32_t item);

/**
 * \brief Inserts an integer value before the specified node.
 *
 * \param self Pointer to the list.
 * \param pIter Pointer to existing node to insert before.
 * \param item Value to insert.
 * \return Error code (ADT_NO_ERROR on success, ADT_MEM_ERROR on allocation failure, ADT_INVALID_ARGUMENT_ERROR if self or pIter is NULL).
 */
adt_error_t adt_u32List_insert_before(adt_u32List_t *self, adt_u32List_elem_t *pIter, uint32_t item);

/**
 * \brief Inserts an integer value after the specified node.
 *
 * \param self Pointer to the list.
 * \param pIter Pointer to existing node to insert after.
 * \param item Value to insert.
 * \return Error code (ADT_NO_ERROR on success, ADT_MEM_ERROR on allocation failure, ADT_INVALID_ARGUMENT_ERROR if self or pIter is NULL).
 */
adt_error_t adt_u32List_insert_after(adt_u32List_t *self, adt_u32List_elem_t *pIter, uint32_t item);

/**
 * \brief Removes the specified node from the list.
 *
 * \param self Pointer to the list.
 * \param pIter Pointer to node to erase.
 */
void adt_u32List_erase(adt_u32List_t *self, adt_u32List_elem_t *pIter);

/**
 * \brief Checks if the list is empty.
 *
 * \param self Pointer to the list.
 * \return true if empty or self is NULL, false otherwise.
 */
bool adt_u32List_is_empty(const adt_u32List_t *self);

/**
 * \brief Returns the first iterator node of the list.
 *
 * \param self Pointer to the list.
 * \return Pointer to first node, or NULL if list is empty.
 */
adt_u32List_elem_t *adt_u32List_iter_first(const adt_u32List_t *self);

/**
 * \brief Returns the last iterator node of the list.
 *
 * \param self Pointer to the list.
 * \return Pointer to last node, or NULL if list is empty.
 */
adt_u32List_elem_t *adt_u32List_iter_last(const adt_u32List_t *self);

/**
 * \brief Advances the iterator to the next node.
 *
 * \param pElem Pointer to current node.
 * \return Pointer to next node, or NULL if at the end.
 */
adt_u32List_elem_t* adt_u32List_iter_next(adt_u32List_elem_t *pElem);

/**
 * \brief Moves the iterator to the previous node.
 *
 * \param pElem Pointer to current node.
 * \return Pointer to previous node, or NULL if at the beginning.
 */
adt_u32List_elem_t* adt_u32List_iter_prev(adt_u32List_elem_t *pElem);

/**
 * \brief Searches for an integer value in the list and returns its node.
 *
 * \param self Pointer to the list.
 * \param item Value to search for.
 * \return Pointer to matching node, or NULL if not found.
 */
adt_u32List_elem_t* adt_u32List_find(const adt_u32List_t *self, uint32_t item);

/**
 * \brief Returns the number of elements in the list.
 *
 * \param self Pointer to the list.
 * \return Number of elements, or -1 if self is NULL.
 */
int32_t adt_u32List_length(const adt_u32List_t *self);

/**
 * \brief Clears all nodes from the list.
 *
 * \param self Pointer to the list.
 */
void adt_u32List_clear(adt_u32List_t *self);

#endif //ADT_LIST_H
