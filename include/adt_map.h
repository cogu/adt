/*****************************************************************************
* \file      adt_map.h
* \author    Conny Gustafsson
* \date      2013-10-01
* \brief     key/value map table using a sorted array as storage container
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_MAP_H
#define ADT_MAP_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * Key/value element pair for adt_u16Map.
 */
typedef struct u16MapElem_tag{
   uint16_t key; /**< 16-bit unsigned integer key */
   void *val;    /**< User-defined payload pointer */
} adt_u16MapElem_t;

/**
 * Sorted-array key/value map container with 16-bit unsigned integer keys.
 */
typedef struct u16Map_tag {
   adt_u16MapElem_t *pBegin;       /**< Pointer to start of element array */
   adt_u16MapElem_t *pEnd;         /**< Pointer to one past last active element */
   adt_u16MapElem_t *pIter;        /**< Internal iterator cursor */
   uint32_t max_num_elem;          /**< Maximum number of elements array can hold */
   uint32_t num_elem;              /**< Current number of elements stored */
   void (*pDestructor)(void*);     /**< Optional virtual destructor callback for values */
   uint8_t destructor_enable;      /**< Non-zero if destructor invocation is enabled */
} adt_u16Map_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
/**
 * Initializes a u16Map using caller-provided storage for zero dynamic memory allocation.
 *
 * @param self Pointer to the adt_u16Map_t structure.
 * @param pArray Pointer to caller-allocated array of adt_u16MapElem_t elements.
 * @param max_num_elem Maximum number of elements that can fit in pArray.
 * @param pDestructor Optional element value destructor callback (or NULL).
 */
void adt_u16Map_create(adt_u16Map_t *self, adt_u16MapElem_t *pArray, uint32_t max_num_elem, void (*pDestructor)(void*));

/**
 * Cleans up the map and invokes the destructor on remaining values if enabled.
 *
 * Does not free pArray or self (intended for stack/static structures initialized via adt_u16Map_create).
 *
 * @param self Pointer to the map.
 */
void adt_u16Map_destroy(adt_u16Map_t *self);

/**
 * Dynamically allocates and initializes a new u16Map on the heap.
 *
 * @param max_num_elem Maximum number of elements to allocate storage for.
 * @param pDestructor Optional element value destructor callback (or NULL).
 * @return Pointer to newly allocated adt_u16Map_t, or NULL on allocation failure.
 */
adt_u16Map_t *adt_u16Map_new(uint32_t max_num_elem, void (*pDestructor)(void*));

/**
 * Cleans up map entries, invokes the destructor on remaining values, and frees heap memory.
 *
 * @param self Pointer to map previously created with adt_u16Map_new.
 */
void adt_u16Map_delete(adt_u16Map_t *self);

/**
 * Enables or disables automatic destructor invocation when removing elements or destroying the map.
 *
 * @param self Pointer to the map.
 * @param enable Non-zero (1) to enable, 0 to disable.
 */
void adt_u16Map_destructor_enable(adt_u16Map_t *self, uint8_t enable);

/**
 * Removes all elements from the map, invoking destructor on each element value if enabled.
 *
 * @param self Pointer to the map.
 */
void adt_u16Map_clear(adt_u16Map_t *self);

/**
 * Inserts a key/value pair in ascending sorted order by key.
 *
 * Duplicate keys are permitted with different values. If both key and val match an existing entry,
 * the duplicate insertion is ignored.
 *
 * @param self Pointer to the map.
 * @param key 16-bit unsigned integer key.
 * @param val Pointer value associated with the key.
 */
void adt_u16Map_insert(adt_u16Map_t *self, uint16_t key, void *val);

/**
 * Removes a specific element pointer from the map by shifting remaining elements left.
 *
 * Does not call the destructor on the value.
 *
 * @param self Pointer to the map.
 * @param pElem Pointer to the adt_u16MapElem_t inside the map to remove.
 */
void adt_u16Map_remove(adt_u16Map_t *self, const adt_u16MapElem_t *pElem);

/**
 * Finds an element matching the given key using binary search.
 *
 * If found, sets internal iterator to the matching element and returns it.
 * If duplicate keys exist, returns the first occurrence.
 *
 * @param self Pointer to the map.
 * @param key 16-bit unsigned key to search for.
 * @return Pointer to matching adt_u16MapElem_t, or NULL if not found.
 */
adt_u16MapElem_t* adt_u16Map_find(adt_u16Map_t *self, uint16_t key);

/**
 * Finds an element matching both the key and the exact pointer value.
 *
 * Useful when duplicate keys exist and a specific key/value pair is sought.
 *
 * @param self Pointer to the map.
 * @param key 16-bit unsigned key to match.
 * @param val Value pointer to match.
 * @return Pointer to matching adt_u16MapElem_t, or NULL if not found.
 */
adt_u16MapElem_t* adt_u16Map_find_exact(adt_u16Map_t *self, uint16_t key, const void *val);

/**
 * Returns the current number of elements stored in the map.
 *
 * @param self Pointer to the map.
 * @return Number of elements.
 */
uint32_t adt_u16Map_size(const adt_u16Map_t *self);

/**
 * Initializes the internal iterator at the specified element (or the first element if NULL/out of bounds).
 *
 * @param self Pointer to the map.
 * @param pElem Starting element pointer, or NULL to start from the beginning.
 * @return Pointer to the current element, or NULL if the map is empty.
 */
adt_u16MapElem_t* adt_u16Map_iter_init(adt_u16Map_t *self, adt_u16MapElem_t *pElem);

/**
 * Advances the iterator to the next element in ascending key order.
 *
 * @param self Pointer to the map.
 * @return Pointer to next adt_u16MapElem_t, or NULL when iteration finishes.
 */
adt_u16MapElem_t* adt_u16Map_iter_next(adt_u16Map_t *self);

/**
 * Moves all elements matching key from src map to dest map in sorted order.
 *
 * @param dest Destination map.
 * @param src Source map.
 * @param key 16-bit unsigned key of elements to transfer.
 * @return Total number of elements moved.
 */
uint32_t adt_u16Map_move_elem(adt_u16Map_t *dest, adt_u16Map_t *src, uint16_t key);

/**
 * Searches the entire map and removes all entries matching the given value pointer.
 *
 * @param self Pointer to the map.
 * @param val Value pointer to remove.
 */
void adt_u16Map_remove_val(adt_u16Map_t *self, const void *val);

#endif //ADT_MAP_H

