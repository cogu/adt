/*****************************************************************************
* \file      adt_partition.h
* \author    Conny Gustafsson
* \date      2026-10-03
* \brief     Contiguous partition index containers (16-bit and 32-bit offsets)
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_PARTITION_H
#define ADT_PARTITION_H

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
 * \brief Contiguous partition index using 16-bit unsigned integer offsets.
 *
 * Partitions an interval [0, total_size) into contiguous, non-overlapping segments.
 * Suitable for partitioned buffers and APX port mappings up to 65535 bytes in size.
 */
typedef struct adt_u16_partition_tag
{
   uint16_t *start_offsets; /**< Dynamic array of segment boundary start offsets (size: num_segments + 1) */
   uint32_t num_segments;   /**< Number of active segments */
   uint32_t capacity;       /**< Allocated capacity in terms of segment count */
   uint16_t total_size;     /**< Total accumulated size of all partitioned segments */
} adt_u16_partition_t;

/**
 * \brief Contiguous partition index using 32-bit unsigned integer offsets.
 *
 * Partitions an interval [0, total_size) into contiguous, non-overlapping segments.
 * Supports arbitrary buffer and file sizes up to UINT32_MAX bytes.
 */
typedef struct adt_u32_partition_tag
{
   uint32_t *start_offsets; /**< Dynamic array of segment boundary start offsets (size: num_segments + 1) */
   uint32_t num_segments;   /**< Number of active segments */
   uint32_t capacity;       /**< Allocated capacity in terms of segment count */
   uint32_t total_size;     /**< Total accumulated size of all partitioned segments */
} adt_u32_partition_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES: adt_u16_partition_t
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes a 16-bit partition instance on stack or embedded memory.
 * \param self Pointer to partition instance.
 */
void adt_u16_partition_create(adt_u16_partition_t *self);

/**
 * \brief Frees internal boundary allocations.
 * \param self Pointer to partition instance.
 */
void adt_u16_partition_destroy(adt_u16_partition_t *self);

/**
 * \brief Allocates and initializes a new 16-bit partition on the heap.
 * \return Pointer to newly allocated partition, or NULL on memory failure.
 */
adt_u16_partition_t* adt_u16_partition_new(void);

/**
 * \brief Frees a heap-allocated 16-bit partition.
 * \param self Pointer to partition instance.
 */
void adt_u16_partition_delete(adt_u16_partition_t *self);

/**
 * \brief Type-erased destructor wrapper for generic containers.
 * \param arg Pointer to partition instance (cast to void*).
 */
void adt_u16_partition_vdelete(void *arg);

/**
 * \brief Clears all segments from the partition without releasing allocated memory.
 * \param self Pointer to partition instance.
 */
void adt_u16_partition_clear(adt_u16_partition_t *self);

/**
 * \brief Pre-allocates boundary capacity for a known number of segments.
 * \param self Pointer to partition instance.
 * \param capacity Number of segments to reserve capacity for.
 * \return ADT_NO_ERROR on success, ADT_MEM_ERROR or ADT_INVALID_ARGUMENT_ERROR otherwise.
 */
adt_error_t adt_u16_partition_reserve(adt_u16_partition_t *self, const uint32_t capacity);

/**
 * \brief Appends a new contiguous segment with the given length.
 * \param self Pointer to partition instance.
 * \param segment_length Length in bytes of the segment to append.
 * \return ADT_NO_ERROR on success, ADT_OVERFLOW_ERROR if total size exceeds UINT16_MAX,
 *         or ADT_MEM_ERROR on allocation failure.
 */
adt_error_t adt_u16_partition_append(adt_u16_partition_t *self, const uint16_t segment_length);

/**
 * \brief Finds the segment index containing the specified byte offset via binary search.
 * \param self Pointer to partition instance.
 * \param offset Byte offset to search for.
 * \return Zero-based segment index (0..num_segments-1) if found, or -1 if offset is out of bounds.
 */
int32_t adt_u16_partition_find(const adt_u16_partition_t *self, const uint16_t offset);

/**
 * \brief Returns the current number of segments in the partition.
 * \param self Pointer to partition instance.
 * \return Number of segments.
 */
uint32_t adt_u16_partition_length(const adt_u16_partition_t *self);

/**
 * \brief Returns the total partitioned byte size.
 * \param self Pointer to partition instance.
 * \return Accumulated size of all segments.
 */
uint16_t adt_u16_partition_total_size(const adt_u16_partition_t *self);

/**
 * \brief Checks if the partition contains no segments.
 * \param self Pointer to partition instance.
 * \return true if empty, false otherwise.
 */
bool adt_u16_partition_is_empty(const adt_u16_partition_t *self);

/**
 * \brief Retrieves the start offset and length of the segment at the specified index.
 * \param self Pointer to partition instance.
 * \param index Zero-based segment index.
 * \param[out] start_offset Output pointer for start offset (optional, can be NULL).
 * \param[out] length Output pointer for segment length (optional, can be NULL).
 * \return ADT_NO_ERROR on success, ADT_INDEX_OUT_OF_BOUNDS_ERROR if index is invalid.
 */
adt_error_t adt_u16_partition_get_segment(const adt_u16_partition_t *self, const uint32_t index, uint16_t *start_offset, uint16_t *length);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES: adt_u32_partition_t
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes a 32-bit partition instance on stack or embedded memory.
 * \param self Pointer to partition instance.
 */
void adt_u32_partition_create(adt_u32_partition_t *self);

/**
 * \brief Frees internal boundary allocations.
 * \param self Pointer to partition instance.
 */
void adt_u32_partition_destroy(adt_u32_partition_t *self);

/**
 * \brief Allocates and initializes a new 32-bit partition on the heap.
 * \return Pointer to newly allocated partition, or NULL on memory failure.
 */
adt_u32_partition_t* adt_u32_partition_new(void);

/**
 * \brief Frees a heap-allocated 32-bit partition.
 * \param self Pointer to partition instance.
 */
void adt_u32_partition_delete(adt_u32_partition_t *self);

/**
 * \brief Type-erased destructor wrapper for generic containers.
 * \param arg Pointer to partition instance (cast to void*).
 */
void adt_u32_partition_vdelete(void *arg);

/**
 * \brief Clears all segments from the partition without releasing allocated memory.
 * \param self Pointer to partition instance.
 */
void adt_u32_partition_clear(adt_u32_partition_t *self);

/**
 * \brief Pre-allocates boundary capacity for a known number of segments.
 * \param self Pointer to partition instance.
 * \param capacity Number of segments to reserve capacity for.
 * \return ADT_NO_ERROR on success, ADT_MEM_ERROR or ADT_INVALID_ARGUMENT_ERROR otherwise.
 */
adt_error_t adt_u32_partition_reserve(adt_u32_partition_t *self, const uint32_t capacity);

/**
 * \brief Appends a new contiguous segment with the given length.
 * \param self Pointer to partition instance.
 * \param segment_length Length in bytes of the segment to append.
 * \return ADT_NO_ERROR on success, ADT_OVERFLOW_ERROR if total size exceeds UINT32_MAX,
 *         or ADT_MEM_ERROR on allocation failure.
 */
adt_error_t adt_u32_partition_append(adt_u32_partition_t *self, const uint32_t segment_length);

/**
 * \brief Finds the segment index containing the specified byte offset via binary search.
 * \param self Pointer to partition instance.
 * \param offset Byte offset to search for.
 * \return Zero-based segment index (0..num_segments-1) if found, or -1 if offset is out of bounds.
 */
int32_t adt_u32_partition_find(const adt_u32_partition_t *self, const uint32_t offset);

/**
 * \brief Returns the current number of segments in the partition.
 * \param self Pointer to partition instance.
 * \return Number of segments.
 */
uint32_t adt_u32_partition_length(const adt_u32_partition_t *self);

/**
 * \brief Returns the total partitioned byte size.
 * \param self Pointer to partition instance.
 * \return Accumulated size of all segments.
 */
uint32_t adt_u32_partition_total_size(const adt_u32_partition_t *self);

/**
 * \brief Checks if the partition contains no segments.
 * \param self Pointer to partition instance.
 * \return true if empty, false otherwise.
 */
bool adt_u32_partition_is_empty(const adt_u32_partition_t *self);

/**
 * \brief Retrieves the start offset and length of the segment at the specified index.
 * \param self Pointer to partition instance.
 * \param index Zero-based segment index.
 * \param[out] start_offset Output pointer for start offset (optional, can be NULL).
 * \param[out] length Output pointer for segment length (optional, can be NULL).
 * \return ADT_NO_ERROR on success, ADT_INDEX_OUT_OF_BOUNDS_ERROR if index is invalid.
 */
adt_error_t adt_u32_partition_get_segment(const adt_u32_partition_t *self, const uint32_t index, uint32_t *start_offset, uint32_t *length);

#ifdef __cplusplus
}
#endif

#endif // ADT_PARTITION_H
