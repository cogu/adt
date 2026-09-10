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
#define adt_streambuffer_NUM_SLABS 3u
#define adt_streambuffer_DEFAULT_SLAB_SIZE 4096u
#define adt_streambuffer_DEFAULT_MAX_RETAINED_SIZE (16u * 1024u)

#define ADT_STREAMBUFFER_NUM_SLABS adt_streambuffer_NUM_SLABS
#define ADT_STREAMBUFFER_DEFAULT_SLAB_SIZE adt_streambuffer_DEFAULT_SLAB_SIZE
#define ADT_STREAMBUFFER_DEFAULT_MAX_RETAINED_SIZE adt_streambuffer_DEFAULT_MAX_RETAINED_SIZE

typedef struct adt_streambuffer_tag
{
   adt_bytearray_t slabs[adt_streambuffer_NUM_SLABS];
   uint32_t default_slab_size;  /**< Baseline allocation size for each slab */
   uint32_t max_retained_size;  /**< Hysteresis threshold: slabs exceeding this shrink upon drain */
   uint8_t  write_slab_idx;     /**< Index of slab currently accepting writes (0..2) */
   uint8_t  read_slab_idx;      /**< Index of slab currently being read (0..2) */
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
void adt_streambuffer_create(adt_streambuffer_t *self, uint32_t default_slab_size, uint32_t max_retained_size);

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
adt_streambuffer_t* adt_streambuffer_new(uint32_t default_slab_size, uint32_t max_retained_size);

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
 * \param pData Pointer to incoming data.
 * \param u32DataLen Number of bytes to append.
 * \return ADT_NO_ERROR on success, error code otherwise.
 */
adt_error_t adt_streambuffer_append(adt_streambuffer_t *self, const uint8_t *pData, uint32_t u32DataLen);

/**
 * \brief Provides a direct write pointer into the active write slab.
 *
 * This allows network socket functions like recv() to write directly into the stream,
 * avoiding an intermediate bounce buffer.
 *
 * \param self Pointer to streambuffer.
 * \param u32MinSpace Minimum contiguous bytes required (will grow slab if needed).
 * \param[out] pAvailSpace Contiguous space available for writing.
 * \return Pointer to write buffer, or NULL on memory allocation error.
 */
uint8_t* adt_streambuffer_write_begin(adt_streambuffer_t *self, uint32_t u32MinSpace, uint32_t *pAvailSpace);

/**
 * \brief Commits bytes actually written via write_begin.
 * \param self Pointer to streambuffer.
 * \param u32BytesWritten Number of bytes written.
 * \return ADT_NO_ERROR on success, ADT_LENGTH_ERROR if bytes exceed available space.
 */
adt_error_t adt_streambuffer_write_commit(adt_streambuffer_t *self, uint32_t u32BytesWritten);


//////////////////////////////////////////////////////////////////////////////
// CONSUMER API (DATA EXTRACTION & FRAMING)
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Retrieves a contiguous pointer to available unparsed data.
 * \param self Pointer to streambuffer.
 * \param[out] pAvailBytes Number of contiguous bytes available to read.
 * \return Contiguous pointer to unread data, or NULL if no data available.
 */
const uint8_t* adt_streambuffer_read_begin(adt_streambuffer_t *self, uint32_t *pAvailBytes);

/**
 * \brief Advances read position after consumer parses bytes.
 * \param self Pointer to streambuffer.
 * \param u32BytesConsumed Number of bytes successfully parsed.
 * \return ADT_NO_ERROR on success, ADT_LENGTH_ERROR if bytes_consumed > available.
 */
adt_error_t adt_streambuffer_read_commit(adt_streambuffer_t *self, uint32_t u32BytesConsumed);


//////////////////////////////////////////////////////////////////////////////
// QUERY API
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Returns total bytes currently buffered and waiting to be read.
 * \param self Pointer to streambuffer instance.
 * \return Total unread bytes.
 */
uint32_t adt_streambuffer_length(const adt_streambuffer_t *self);

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
