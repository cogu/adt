/*****************************************************************************
* \file      adt_range_list.h
* \author    Conny Gustafsson
* \date      2026-11-08
* \brief     Disjoint sorted range list containers (16-bit and 32-bit offsets)
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_RANGE_LIST_H
#define ADT_RANGE_LIST_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "adt_error.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief 16-bit range element representing [start, start + length).
 */
typedef struct adt_u16_range_tag
{
   uint16_t start;  /**< Start offset of range */
   uint16_t length; /**< Length of range in bytes */
} adt_u16_range_t;

/**
 * \brief Sorted, non-overlapping 16-bit range list container.
 */
typedef struct adt_u16_range_list_tag
{
   adt_u16_range_t *ranges; /**< Dynamic array of non-overlapping, sorted ranges */
   uint32_t num_ranges;     /**< Number of active ranges */
   uint32_t capacity;       /**< Allocated capacity in terms of range count */
} adt_u16_range_list_t;

/**
 * \brief 32-bit range element representing [start, start + length).
 */
typedef struct adt_u32_range_tag
{
   uint32_t start;  /**< Start offset of range */
   uint32_t length; /**< Length of range in bytes */
} adt_u32_range_t;

/**
 * \brief Sorted, non-overlapping 32-bit range list container.
 */
typedef struct adt_u32_range_list_tag
{
   adt_u32_range_t *ranges; /**< Dynamic array of non-overlapping, sorted ranges */
   uint32_t num_ranges;     /**< Number of active ranges */
   uint32_t capacity;       /**< Allocated capacity in terms of range count */
} adt_u32_range_list_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES: adt_u16_range_list_t
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes a 16-bit range list on stack or embedded memory.
 * \param self Pointer to range list instance.
 */
void adt_u16_range_list_create(adt_u16_range_list_t *self);

/**
 * \brief Frees internal range allocations.
 * \param self Pointer to range list instance.
 */
void adt_u16_range_list_destroy(adt_u16_range_list_t *self);

/**
 * \brief Allocates and initializes a new 16-bit range list on the heap.
 * \return Pointer to newly allocated range list, or NULL on memory failure.
 */
adt_u16_range_list_t* adt_u16_range_list_new(void);

/**
 * \brief Frees a heap-allocated 16-bit range list.
 * \param self Pointer to range list instance.
 */
void adt_u16_range_list_delete(adt_u16_range_list_t *self);

/**
 * \brief Type-erased destructor wrapper for generic containers.
 * \param arg Pointer to range list instance (cast to void*).
 */
void adt_u16_range_list_vdelete(void *arg);

/**
 * \brief Clears all ranges without releasing allocated memory.
 * \param self Pointer to range list instance.
 */
void adt_u16_range_list_clear(adt_u16_range_list_t *self);

/**
 * \brief Pre-allocates capacity for a known number of ranges.
 * \param self Pointer to range list instance.
 * \param capacity Number of ranges to reserve capacity for.
 * \return ADT_NO_ERROR on success, ADT_MEM_ERROR or ADT_INVALID_ARGUMENT_ERROR otherwise.
 */
adt_error_t adt_u16_range_list_reserve(adt_u16_range_list_t *self, const uint32_t capacity);

/**
 * \brief Inserts a range [start, start + length) into the list, merging with existing ranges
 *        if overlapping, adjacent, or separated by a gap <= max_gap.
 * \param self Pointer to range list instance.
 * \param start Start offset of the range.
 * \param length Length in bytes of the range. If 0, returns ADT_NO_ERROR with no change.
 * \param max_gap Maximum gap in bytes between ranges allowed to trigger a merge (0 = strictly adjacent or overlapping).
 * \return ADT_NO_ERROR on success, ADT_OVERFLOW_ERROR if start + length > UINT16_MAX,
 *         or ADT_MEM_ERROR on allocation failure.
 */
adt_error_t adt_u16_range_list_insert(adt_u16_range_list_t *self, const uint16_t start, const uint16_t length, const uint16_t max_gap);

/**
 * \brief Returns the number of distinct ranges in the list.
 * \param self Pointer to range list instance.
 * \return Number of ranges.
 */
uint32_t adt_u16_range_list_length(const adt_u16_range_list_t *self);

/**
 * \brief Checks if the range list is empty.
 * \param self Pointer to range list instance.
 * \return true if empty, false otherwise.
 */
bool adt_u16_range_list_is_empty(const adt_u16_range_list_t *self);

/**
 * \brief Retrieves the range at the specified zero-based index.
 * \param self Pointer to range list instance.
 * \param index Zero-based index (0..num_ranges-1).
 * \param[out] range Output pointer where range data will be copied.
 * \return ADT_NO_ERROR on success, ADT_INDEX_OUT_OF_BOUNDS_ERROR if index is invalid.
 */
adt_error_t adt_u16_range_list_get_range(const adt_u16_range_list_t *self, const uint32_t index, adt_u16_range_t *range);

/**
 * \brief Returns a direct pointer to the range element at index.
 * \param self Pointer to range list instance.
 * \param index Zero-based index (0..num_ranges-1).
 * \return Pointer to range element, or NULL if index is out of bounds.
 */
const adt_u16_range_t* adt_u16_range_list_at(const adt_u16_range_list_t *self, const uint32_t index);

/**
 * \brief Finds the index of the range containing the specified byte offset via binary search.
 * \param self Pointer to range list instance.
 * \param offset Byte offset to search for.
 * \return Zero-based range index if found, or -1 if offset is not inside any range.
 */
int32_t adt_u16_range_list_find(const adt_u16_range_list_t *self, const uint16_t offset);

/**
 * \brief Checks if the specified byte offset is contained in any range.
 * \param self Pointer to range list instance.
 * \param offset Byte offset to check.
 * \return true if covered by a range, false otherwise.
 */
bool adt_u16_range_list_contains(const adt_u16_range_list_t *self, const uint16_t offset);

/**
 * \brief Calculates the total accumulated length of all ranges in bytes.
 * \param self Pointer to range list instance.
 * \return Total length.
 */
uint32_t adt_u16_range_list_total_length(const adt_u16_range_list_t *self);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES: adt_u32_range_list_t
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes a 32-bit range list on stack or embedded memory.
 * \param self Pointer to range list instance.
 */
void adt_u32_range_list_create(adt_u32_range_list_t *self);

/**
 * \brief Frees internal range allocations.
 * \param self Pointer to range list instance.
 */
void adt_u32_range_list_destroy(adt_u32_range_list_t *self);

/**
 * \brief Allocates and initializes a new 32-bit range list on the heap.
 * \return Pointer to newly allocated range list, or NULL on memory failure.
 */
adt_u32_range_list_t* adt_u32_range_list_new(void);

/**
 * \brief Frees a heap-allocated 32-bit range list.
 * \param self Pointer to range list instance.
 */
void adt_u32_range_list_delete(adt_u32_range_list_t *self);

/**
 * \brief Type-erased destructor wrapper for generic containers.
 * \param arg Pointer to range list instance (cast to void*).
 */
void adt_u32_range_list_vdelete(void *arg);

/**
 * \brief Clears all ranges without releasing allocated memory.
 * \param self Pointer to range list instance.
 */
void adt_u32_range_list_clear(adt_u32_range_list_t *self);

/**
 * \brief Pre-allocates capacity for a known number of ranges.
 * \param self Pointer to range list instance.
 * \param capacity Number of ranges to reserve capacity for.
 * \return ADT_NO_ERROR on success, ADT_MEM_ERROR or ADT_INVALID_ARGUMENT_ERROR otherwise.
 */
adt_error_t adt_u32_range_list_reserve(adt_u32_range_list_t *self, const uint32_t capacity);

/**
 * \brief Inserts a range [start, start + length) into the list, merging with existing ranges
 *        if overlapping, adjacent, or separated by a gap <= max_gap.
 * \param self Pointer to range list instance.
 * \param start Start offset of the range.
 * \param length Length in bytes of the range. If 0, returns ADT_NO_ERROR with no change.
 * \param max_gap Maximum gap in bytes between ranges allowed to trigger a merge (0 = strictly adjacent or overlapping).
 * \return ADT_NO_ERROR on success, ADT_OVERFLOW_ERROR if start + length > UINT32_MAX,
 *         or ADT_MEM_ERROR on allocation failure.
 */
adt_error_t adt_u32_range_list_insert(adt_u32_range_list_t *self, const uint32_t start, const uint32_t length, const uint32_t max_gap);

/**
 * \brief Returns the number of distinct ranges in the list.
 * \param self Pointer to range list instance.
 * \return Number of ranges.
 */
uint32_t adt_u32_range_list_length(const adt_u32_range_list_t *self);

/**
 * \brief Checks if the range list is empty.
 * \param self Pointer to range list instance.
 * \return true if empty, false otherwise.
 */
bool adt_u32_range_list_is_empty(const adt_u32_range_list_t *self);

/**
 * \brief Retrieves the range at the specified zero-based index.
 * \param self Pointer to range list instance.
 * \param index Zero-based index (0..num_ranges-1).
 * \param[out] range Output pointer where range data will be copied.
 * \return ADT_NO_ERROR on success, ADT_INDEX_OUT_OF_BOUNDS_ERROR if index is invalid.
 */
adt_error_t adt_u32_range_list_get_range(const adt_u32_range_list_t *self, const uint32_t index, adt_u32_range_t *range);

/**
 * \brief Returns a direct pointer to the range element at index.
 * \param self Pointer to range list instance.
 * \param index Zero-based index (0..num_ranges-1).
 * \return Pointer to range element, or NULL if index is out of bounds.
 */
const adt_u32_range_t* adt_u32_range_list_at(const adt_u32_range_list_t *self, const uint32_t index);

/**
 * \brief Finds the index of the range containing the specified byte offset via binary search.
 * \param self Pointer to range list instance.
 * \param offset Byte offset to search for.
 * \return Zero-based range index if found, or -1 if offset is not inside any range.
 */
int32_t adt_u32_range_list_find(const adt_u32_range_list_t *self, const uint32_t offset);

/**
 * \brief Checks if the specified byte offset is contained in any range.
 * \param self Pointer to range list instance.
 * \param offset Byte offset to check.
 * \return true if covered by a range, false otherwise.
 */
bool adt_u32_range_list_contains(const adt_u32_range_list_t *self, const uint32_t offset);

/**
 * \brief Calculates the total accumulated length of all ranges in bytes.
 * \param self Pointer to range list instance.
 * \return Total length.
 */
uint64_t adt_u32_range_list_total_length(const adt_u32_range_list_t *self);

#ifdef __cplusplus
}
#endif

#endif // ADT_RANGE_LIST_H

