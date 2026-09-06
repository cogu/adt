/*****************************************************************************
* \file      adt_ary.h
* \author    Conny Gustafsson
* \date      2013-06-03
* \brief     array data structure
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_ARY_H
#define ADT_ARY_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "adt_error.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
typedef struct adt_ary_tag{
   void **ppAlloc;             //array of (void*)
   void **pFirst;              //pointer to first array element
   int32_t s32AllocLen;        //number of elements allocated
   int32_t s32CurLen;          //number of elements currently in the array
   void (*pDestructor)(void*); //optional destructor function (typically vdelete functions from other data structures)
   void *pFillElem;            //optional fill element for new elements (defaults to NULL)
   bool destructorEnable;      //Temporarily disables use of element pDestructor
} adt_ary_t;

typedef int (adt_vlt_func_t)(const void *a, const void *b); //lt = less-than

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Allocates and initializes a new array on the heap.
 *
 * \param pDestructor Optional element destructor function callback (e.g. free or a *_vdelete function), or NULL for non-owning references.
 * \return Pointer to newly allocated array, or NULL on failure.
 */
adt_ary_t* adt_ary_new(void (*pDestructor)(void*));

/**
 * \brief Allocates and initializes a new array containing a shallow copy of an existing pointer buffer.
 *
 * \param ppElem Array of element pointers to copy.
 * \param s32Len Number of elements to copy.
 * \param pDestructor Optional element destructor function callback, or NULL.
 * \return Pointer to newly allocated array, or NULL on failure.
 */
adt_ary_t* adt_ary_make(void** ppElem, int32_t s32Len, void (*pDestructor)(void*));

/**
 * \brief Destroys the array and frees the self pointer.
 *
 * \param self Pointer to the array to delete.
 */
void adt_ary_delete(adt_ary_t *self);

/**
 * \brief Type-erased destructor wrapper for adt_ary_delete.
 *
 * Suitable for use as a generic destructor callback void (*)(void*) in other ADT containers.
 *
 * \param arg Pointer to the array (cast to void*).
 */
void adt_ary_vdelete(void *arg);

/**
 * \brief Initializes an array instance in place (stack or embedded allocation).
 *
 * \param self Pointer to an existing adt_ary_t instance.
 * \param pDestructor Optional element destructor function callback, or NULL.
 */
void adt_ary_create(adt_ary_t *self, void (*pDestructor)(void*));

/**
 * \brief Destroys all elements using the configured destructor and frees the internal buffer. Does not free self.
 *
 * \param self Pointer to the array to destroy.
 */
void adt_ary_destroy(adt_ary_t *self);

/**
 * \brief Temporarily enables or disables automatic invocation of the element destructor.
 *
 * \param self Pointer to the array.
 * \param enable True to enable element destructor calls, false to suspend them.
 */
void adt_ary_destructor_enable(adt_ary_t *self, bool enable);

/**
 * \brief Checks if the array has a destructor callback configured.
 *
 * \param self Pointer to the array.
 * \return True if a destructor callback is set, false if NULL or self is NULL.
 */
bool adt_ary_has_destructor(const adt_ary_t *self);

/**
 * \brief Checks if automatic invocation of the element destructor is currently active.
 *
 * \param self Pointer to the array.
 * \return True if a destructor callback is set and enabled, false otherwise or if self is NULL.
 */
bool adt_ary_destructor_is_enabled(const adt_ary_t *self);


//Accessors

/**
 * \brief Sets the element pointer at the given index, automatically extending the array if necessary.
 *
 * \param self Pointer to the array.
 * \param s32Index Zero-based or negative index (e.g. -1 for last element).
 * \param pElem Pointer value to store.
 * \return Pointer to the stored slot (void**), or NULL on out-of-bounds error.
 */
void** adt_ary_set(adt_ary_t *self, int32_t s32Index, void *pElem);

/**
 * \brief Retrieves a pointer to the slot at the given index, automatically extending the array if necessary.
 *
 * \param self Pointer to the array.
 * \param s32Index Zero-based or negative index.
 * \return Pointer to the stored slot (void**), or NULL on out-of-bounds error.
 */
void** adt_ary_get(adt_ary_t *self, int32_t s32Index);

/**
 * \brief Appends an element pointer to the end of the array.
 *
 * \param self Pointer to the array.
 * \param pElem Element pointer to append.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_ary_push(adt_ary_t *self, void *pElem);

/**
 * \brief Appends an element pointer to the end of the array only if it is not already present.
 *
 * \param self Pointer to the array.
 * \param pElem Element pointer to append.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_ary_push_unique(adt_ary_t *self, void *pElem);

/**
 * \brief Removes and returns the last element from the array without invoking its destructor.
 *
 * \param self Pointer to the array.
 * \return The removed element pointer, or NULL if the array is empty.
 */
void* adt_ary_pop(adt_ary_t *self);

/**
 * \brief Removes and returns the first element from the array without invoking its destructor.
 *
 * \param self Pointer to the array.
 * \return The removed element pointer, or NULL if the array is empty.
 */
void* adt_ary_shift(adt_ary_t *self);

/**
 * \brief Inserts an element pointer at the beginning of the array, shifting all existing elements forward.
 *
 * \param self Pointer to the array.
 * \param pElem Element pointer to insert.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_ary_unshift(adt_ary_t *self, void *pElem);

/**
 * \brief Convenience function returning the element pointer at s32Index directly.
 *
 * \param self Pointer to the array.
 * \param s32Index Zero-based or negative index.
 * \return The stored element pointer, or NULL if self is NULL or index is invalid.
 */
void* adt_ary_value(const adt_ary_t *self, int32_t s32Index);

/**
 * \brief Removes the first occurrence of pElem from the array and calls its destructor if enabled.
 *
 * \param self Pointer to the array.
 * \param pElem Element pointer to remove.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_ary_remove(adt_ary_t *self, void *pElem);

//Utility functions

/**
 * \brief Ensures allocated capacity is at least s32Len elements.
 *
 * \param self Pointer to the array.
 * \param s32Len Desired capacity.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_ary_extend(adt_ary_t *self, int32_t s32Len);

/**
 * \brief Extends array to s32Len elements and populates newly allocated slots with the default fill element.
 *
 * \param self Pointer to the array.
 * \param s32Len Desired length.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_ary_fill(adt_ary_t *self, int32_t s32Len);

/**
 * \brief Resizes the array to exactly s32Len elements, destroying any truncated elements if shrinking.
 *
 * \param self Pointer to the array.
 * \param s32Len New length.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_ary_resize(adt_ary_t *self, int32_t s32Len);

/**
 * \brief Destroys all elements and resets array length to 0.
 *
 * \param self Pointer to the array.
 */
void adt_ary_clear(adt_ary_t *self);

/**
 * \brief Returns the current number of elements in the array.
 *
 * \param self Pointer to the array.
 * \return Number of elements, or -1 if self is NULL.
 */
int32_t adt_ary_length(const adt_ary_t *self);

/**
 * \brief Checks if the array is empty.
 *
 * \param self Pointer to the array.
 * \return true if length is 0 or self is NULL, false otherwise.
 */
bool adt_ary_is_empty(const adt_ary_t* self);

/**
 * \brief Sets the fill element pointer used to populate uninitialized slots during fill operations.
 *
 * \param self Pointer to the array.
 * \param pFillElem Default element pointer (defaults to NULL).
 */
void adt_ary_set_fill_elem(adt_ary_t *self, void* pFillElem);

/**
 * \brief Returns the current fill element pointer.
 *
 * \param self Pointer to the array.
 * \return Pointer to default fill element.
 */
void* adt_ary_get_fill_elem(const adt_ary_t *self);

/**
 * \brief Removes s32Len elements starting from s32Index, invoking destructors on removed elements.
 *
 * \param self Pointer to the array.
 * \param s32Index Starting index.
 * \param s32Len Number of elements to remove.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_ary_splice(adt_ary_t *self, int32_t s32Index, int32_t s32Len);

/**
 * \brief Checks if an element exists at the specified index.
 *
 * \param self Pointer to the array.
 * \param s32Index Zero-based or negative index to check.
 * \return 1 if index exists, 0 if out of bounds, or -1 if invalid.
 */
int32_t adt_ary_exists(const adt_ary_t *self, int32_t s32Index);

/**
 * \brief Sorts the array using the provided comparator function.
 *
 * \param self Pointer to the array.
 * \param key Comparison function.
 * \param reverse True for descending order, false for ascending order.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_ary_sort(adt_ary_t *self, adt_vlt_func_t *key, bool reverse);

/**
 * \brief Searches for the first occurrence of pElem and returns its index.
 *
 * \param self Pointer to the array.
 * \param pElem Element pointer to search for.
 * \return Zero-based index of the element, or -1 if not found.
 */
int32_t adt_ary_index_of(adt_ary_t *self, void *pElem);

//built-in lt functions (for sorting)

/**
 * \brief Comparator helper for sorting pointers to int32_t values.
 *
 * \param a Pointer to first int32_t.
 * \param b Pointer to second int32_t.
 * \return Nonzero if *a < *b, 0 otherwise.
 */
int adt_i32_vlt(const void *a, const void *b);

/**
 * \brief Comparator helper for sorting pointers to uint32_t values.
 *
 * \param a Pointer to first uint32_t.
 * \param b Pointer to second uint32_t.
 * \return Nonzero if *a < *b, 0 otherwise.
 */
int adt_u32_vlt(const void *a, const void *b);

#endif //ADT_ARY_H
