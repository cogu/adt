/*****************************************************************************
* \file      adt_streambuffer.h
* \author    Conny Gustafsson
* \date      2026-09-10
* \brief     Rolling multi-slab byte stream buffer for zero-copy I/O
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_STREAMBUFFER_H
#define ADT_STREAMBUFFER_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "adt_error.h"
#include "adt_bytearray.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define ADT_STREAMBUFFER_NUM_SLABS 2u
#define ADT_STREAMBUFFER_DEFAULT_SLAB_SIZE 4096u
#define ADT_STREAMBUFFER_DEFAULT_MAX_RETAINED_SIZE (16u * 1024u)

/**
 * \brief Rolling multi-slab stream buffer instance.
 *
 * \warning Thread Safety: adt_streambuffer_t is NOT thread-safe. It does not support
 *          concurrent or parallel reading and writing from multiple threads.
 *          If one thread acts as a producer and another acts as a consumer, callers
 *          must use an external synchronization mechanism (such as a mutex) to
 *          serialize access.
 */
typedef struct adt_streambuffer_tag
{
   adt_bytearray_t slabs[ADT_STREAMBUFFER_NUM_SLABS];
   uint32_t default_slab_size;  /**< Baseline allocation size for each slab */
   uint32_t max_retained_size;  /**< Hysteresis threshold: slabs exceeding this shrink upon drain */
   uint8_t  write_slab_idx;     /**< Index of slab currently accepting writes (0..1) */
   uint8_t  read_slab_idx;      /**< Index of slab currently being read (0..1) */
   uint32_t read_pos;           /**< Number of bytes already consumed from slabs[read_slab_idx] */
} adt_streambuffer_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// LIFECYCLE MANAGEMENT
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes a streambuffer instance on stack or embedded memory.
 * \param self Pointer to streambuffer instance.
 * \param default_slab_size Initial/baseline allocation per slab (0 for default 4096).
 * \param max_retained_size Threshold above which drained slabs shrink (0 for default 16384).
 */
void adt_streambuffer_create(adt_streambuffer_t *self, const uint32_t default_slab_size, const uint32_t max_retained_size);

/**
 * \brief Frees all internal slab allocations.
 * \param self Pointer to streambuffer instance.
 */
void adt_streambuffer_destroy(adt_streambuffer_t *self);

/**
 * \brief Heap allocates and initializes a new streambuffer.
 * \param default_slab_size Initial/baseline allocation per slab (0 for default 4096).
 * \param max_retained_size Threshold above which drained slabs shrink (0 for default 16384).
 * \return Pointer to newly allocated streambuffer, or NULL on allocation failure.
 */
adt_streambuffer_t* adt_streambuffer_new(const uint32_t default_slab_size, const uint32_t max_retained_size);

/**
 * \brief Frees a heap-allocated streambuffer.
 * \param self Pointer to streambuffer instance.
 */
void adt_streambuffer_delete(adt_streambuffer_t *self);

/**
 * \brief Type-erased destructor wrapper for generic containers.
 * \param arg Pointer to streambuffer instance (cast to void*).
 */
void adt_streambuffer_vdelete(void *arg);

/**
 * \brief Resets stream state, clearing all data without freeing baseline memory.
 * \param self Pointer to streambuffer instance.
 */
void adt_streambuffer_clear(adt_streambuffer_t *self);


//////////////////////////////////////////////////////////////////////////////
// PRODUCER API (DATA INGESTION)
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Appends raw bytes into the stream (copy-in convenience function).
 * \param self Pointer to streambuffer.
 * \param data Pointer to incoming data.
 * \param num_bytes Number of bytes to append.
 * \return ADT_NO_ERROR on success, error code otherwise.
 */
adt_error_t adt_streambuffer_append(adt_streambuffer_t *self, const uint8_t *data, const uint32_t num_bytes);

/**
 * \brief Provides a direct write pointer into the active write slab.
 *
 * This allows network socket functions like recv() to write directly into the stream,
 * avoiding an intermediate bounce buffer.
 *
 * \param self Pointer to streambuffer.
 * \param min_bytes Minimum contiguous bytes required (will grow slab if needed).
 * \param[out] avail_bytes Contiguous bytes available for writing.
 * \return Pointer to write buffer, or NULL on memory allocation error.
 */
uint8_t* adt_streambuffer_write_begin(adt_streambuffer_t *self, const uint32_t min_bytes, uint32_t *avail_bytes);

/**
 * \brief Commits bytes actually written via write_begin.
 * \param self Pointer to streambuffer.
 * \param written_bytes Number of bytes written.
 * \return ADT_NO_ERROR on success, ADT_LENGTH_ERROR if bytes exceed available space.
 */
adt_error_t adt_streambuffer_write_commit(adt_streambuffer_t *self, const uint32_t written_bytes);


//////////////////////////////////////////////////////////////////////////////
// CONSUMER API (DATA EXTRACTION & FRAMING)
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Retrieves a contiguous pointer to available unparsed data.
 * \param self Pointer to streambuffer.
 * \param[out] avail_bytes Number of contiguous bytes available to read.
 * \return Contiguous pointer to unread data, or NULL if no data available.
 */
const uint8_t* adt_streambuffer_read_begin(const adt_streambuffer_t *self, uint32_t *avail_bytes);

/**
 * \brief Advances read position after consumer parses bytes.
 * \param self Pointer to streambuffer.
 * \param consumed_bytes Number of bytes successfully parsed.
 * \return ADT_NO_ERROR on success, ADT_LENGTH_ERROR if bytes_consumed > available.
 */
adt_error_t adt_streambuffer_read_commit(adt_streambuffer_t *self, const uint32_t consumed_bytes);


//////////////////////////////////////////////////////////////////////////////
// CAPACITY MANAGEMENT
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Ensures contiguous capacity for at least min_bytes for the current message.
 *
 * This function allows callers (typically consumers that have parsed a frame header with a known
 * large payload length, or producers preparing for a large transfer) to reserve buffer space upfront.
 *
 * If read_pos == 0 and more space is needed, the active slab is expanded in place with zero copying.
 * If read_pos > 0 and the remaining space in the active slab is insufficient, a compaction rollover
 * to the next slab is performed immediately: unread bytes are moved to offset 0 of the next slab,
 * the next slab is sized to min_bytes, and the old slab is retired.
 *
 * \warning Calling this function may reallocate internal buffers or trigger a slab rollover,
 *          invalidating any raw pointers previously returned by adt_streambuffer_read_begin()
 *          or adt_streambuffer_write_begin().
 *
 * \param self Pointer to streambuffer instance.
 * \param min_bytes Desired minimum contiguous bytes required starting from read_pos.
 * \return ADT_NO_ERROR on success, ADT_MEM_ERROR on allocation failure, or ADT_INVALID_ARGUMENT_ERROR if self is NULL.
 */
adt_error_t adt_streambuffer_reserve(adt_streambuffer_t *self, const uint32_t min_bytes);


//////////////////////////////////////////////////////////////////////////////
// QUERY API
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Returns total bytes currently buffered and waiting to be read.
 * \param self Pointer to streambuffer instance.
 * \return Total unread bytes.
 */
uint32_t adt_streambuffer_size(const adt_streambuffer_t *self);

/**
 * \brief Returns total heap bytes currently allocated across all slabs.
 * \param self Pointer to streambuffer instance.
 * \return Total bytes allocated on the heap across all slabs.
 */
uint32_t adt_streambuffer_allocated_bytes(const adt_streambuffer_t *self);

/**
 * \brief Checks whether the streambuffer has no unread bytes waiting.
 * \param self Pointer to streambuffer instance.
 * \return true if empty or self is NULL, false otherwise.
 */
bool adt_streambuffer_is_empty(const adt_streambuffer_t *self);

#ifdef __cplusplus
}
#endif

#endif // ADT_STREAMBUFFER_H
