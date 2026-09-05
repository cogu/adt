/*****************************************************************************
* \file      adt_set.h
* \author    Conny Gustafsson
* \date      2018-07-27
* \brief     Set container for 32-bit unsigned integers
*
* Copyright (c) 2018-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_SET_H
#define ADT_SET_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "adt_list.h"

//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * Set container for 32-bit unsigned integers (uint32_t).
 *
 * Stores unique values in sorted ascending order.
 */
typedef struct adt_u32Set_tag
{
   ///TODO: Investigate if another data structure can be used for increased performance
   adt_u32List_t list; /**< Internal backing list storing elements */
} adt_u32Set_t;

//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * Initializes a set on the stack or in pre-allocated memory.
 *
 * @param self Pointer to the adt_u32Set_t structure to initialize.
 */
void adt_u32Set_create(adt_u32Set_t *self);

/**
 * Destroys a set and releases all its internal node memory.
 *
 * @param self Pointer to the set to destroy.
 */
void adt_u32Set_destroy(adt_u32Set_t *self);

/**
 * Allocates and initializes a new set on the heap.
 *
 * @return Pointer to newly allocated adt_u32Set_t, or NULL if memory allocation fails.
 */
adt_u32Set_t* adt_u32Set_new(void);

/**
 * Destroys the set and frees the set container memory.
 *
 * @param self Pointer to set previously created with adt_u32Set_new.
 */
void adt_u32Set_delete(adt_u32Set_t *self);

/**
 * Virtual destructor callback suitable for container destruction callbacks.
 *
 * Casts arg to adt_u32Set_t* and calls adt_u32Set_delete.
 *
 * @param arg Pointer to adt_u32Set_t instance (as void*).
 */
void adt_u32Set_vdelete(void *arg);

/**
 * Returns the number of unique elements in the set.
 *
 * @param self Pointer to the set.
 * @return Number of elements, or -1 if self is NULL.
 */
int32_t adt_u32Set_length(adt_u32Set_t *self);

/**
 * Removes all elements from the set.
 *
 * @param self Pointer to the set.
 */
void adt_u32Set_clear(adt_u32Set_t *self);

/**
 * Inserts a 32-bit unsigned integer into the set.
 *
 * If val is already present in the set, the insertion is ignored to preserve uniqueness.
 * Values are maintained in ascending sorted order.
 *
 * @param self Pointer to the set.
 * @param val Value to insert.
 */
void adt_u32Set_insert(adt_u32Set_t *self, uint32_t val);

/**
 * Removes a 32-bit unsigned integer from the set.
 *
 * @param self Pointer to the set.
 * @param val Value to remove.
 * @return true if the value was found and removed, false otherwise.
 */
bool adt_u32Set_remove(adt_u32Set_t *self, uint32_t val);

/**
 * Tests whether a 32-bit unsigned integer is present in the set.
 *
 * @param self Pointer to the set.
 * @param val Value to search for.
 * @return true if the value is found, false otherwise.
 */
bool adt_u32Set_contains(adt_u32Set_t *self, uint32_t val);

/**
 * Checks whether the set contains zero elements.
 *
 * @param self Pointer to the set.
 * @return true if empty (length == 0), false otherwise.
 */
bool adt_u32Set_is_empty(const adt_u32Set_t *self);

#endif //ADT_SET_H

