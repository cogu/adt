/*****************************************************************************
* \file      adt_bytearray.h
* \author    Conny Gustafsson
* \date      2015-02-05
* \brief     A mutable byte array
*
* Copyright (c) 2015-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_BYTE_ARRAY_H
#define ADT_BYTE_ARRAY_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "adt_error.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
//forward declarations
struct adt_bytes_tag;

typedef struct adt_bytearray_tag
{
   uint8_t *pData;
   uint32_t u32CurLen;
   uint32_t u32AllocLen;
   uint32_t u32GrowSize;
} adt_bytearray_t;

//Corrected names
#define ADT_BYTEARRAY_NO_GROWTH 0u  //will malloc exactly the number of bytes it currently needs
#define ADT_BYTEARRAY_DEFAULT_GROW_SIZE ((uint32_t)8192u)
#define ADT_BYTEARRAY_MAX_GROW_SIZE ((uint32_t)32u*1024u*1024u)

//Deprecated names
#define ADT_BYTE_ARRAY_VERSION ADT_BYTEARRAY_NO_GROWTH
#define ADT_BYTE_ARRAY_NO_GROWTH ADT_BYTEARRAY_NO_GROWTH
#define ADT_BYTE_ARRAY_DEFAULT_GROW_SIZE ADT_BYTEARRAY_DEFAULT_GROW_SIZE
#define ADT_BYTE_ARRAY_MAX_GROW_SIZE ADT_BYTEARRAY_MAX_GROW_SIZE


//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes a byte array instance.
 *
 * \param self Pointer to an existing adt_bytearray_t instance.
 * \param u32GrowSize Growth chunk size in bytes (0 for exact allocation).
 */
void adt_bytearray_create(adt_bytearray_t *self, uint32_t u32GrowSize);

/**
 * \brief Frees the internal buffer allocated by the byte array. Does not free the self pointer itself.
 *
 * \param self Pointer to the byte array to destroy.
 */
void adt_bytearray_destroy(adt_bytearray_t *self);

/**
 * \brief Allocates and initializes a new byte array on the heap.
 *
 * \param u32GrowSize Growth chunk size in bytes (0 for exact allocation).
 * \return Pointer to newly allocated byte array, or NULL on allocation failure.
 */
adt_bytearray_t *adt_bytearray_new(uint32_t u32GrowSize);

/**
 * \brief Creates a new heap-allocated byte array initialized with a copy of raw data.
 *
 * \param pData Pointer to source byte buffer.
 * \param u32DataLen Number of bytes to copy.
 * \param u32GrowSize Growth chunk size in bytes.
 * \return Newly allocated byte array, or NULL on failure.
 */
adt_bytearray_t *adt_bytearray_make(const uint8_t *pData, uint32_t u32DataLen, uint32_t u32GrowSize);

/**
 * \brief Creates a new heap-allocated byte array initialized with the contents of a null-terminated C string (excluding the null terminator).
 *
 * \param cstr Source null-terminated C string.
 * \param u32GrowSize Growth chunk size in bytes.
 * \return Newly allocated byte array, or NULL on failure.
 */
adt_bytearray_t *adt_bytearray_make_cstr(const char *cstr, uint32_t u32GrowSize);

/**
 * \brief Creates an independent duplicate of an existing byte array.
 *
 * \param other Pointer to byte array to clone.
 * \param u32GrowSize Growth chunk size for the clone.
 * \return Newly allocated duplicate byte array, or NULL on failure.
 */
adt_bytearray_t *adt_bytearray_clone(const adt_bytearray_t *other, uint32_t u32GrowSize);

/**
 * \brief Destroys the internal buffer and frees the self pointer.
 *
 * \param self Pointer to the byte array to delete.
 */
void adt_bytearray_delete(adt_bytearray_t *self);

/**
 * \brief Type-erased destructor wrapper for adt_bytearray_delete.
 *
 * Suitable for use as a generic destructor callback void (*)(void*) in other ADT containers.
 *
 * \param arg Pointer to the byte array (cast to void*).
 */
void adt_bytearray_vdelete(void *arg);

/**
 * \brief Updates the growth chunk size for subsequent allocations.
 *
 * \param self Pointer to the byte array.
 * \param u32GrowSize Growth chunk size in bytes (maximum allowed is 32 MB).
 */
void adt_bytearray_set_growth_size(adt_bytearray_t *self, uint32_t u32GrowSize);

/**
 * \brief Ensures that capacity is at least u32NewLen bytes without changing the logical length of the array.
 *
 * \param self Pointer to the byte array.
 * \param u32NewLen Required minimum allocated capacity in bytes.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_bytearray_reserve(adt_bytearray_t *self, uint32_t u32NewLen);

/**
 * \brief Grows the allocated capacity to at least u32MinLen using the configured growth policy.
 *
 * \param self Pointer to the byte array.
 * \param u32MinLen Minimum desired allocated capacity.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_bytearray_grow(adt_bytearray_t *self, uint32_t u32MinLen);

/**
 * \brief Appends raw bytes to the end of the array, automatically expanding capacity if needed.
 *
 * \param self Pointer to the byte array.
 * \param pData Pointer to data to append.
 * \param u32DataLen Number of bytes to append.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_bytearray_append(adt_bytearray_t *self, const uint8_t *pData, uint32_t u32DataLen);

/**
 * \brief Discards all bytes preceding pSrc, shifting the remaining bytes to the beginning of the array.
 *
 * \param self Pointer to the byte array.
 * \param pSrc Pointer to a byte within the array buffer.
 * \return ADT_NO_ERROR on success, or ADT_INVALID_ARGUMENT_ERROR if pSrc is out of bounds.
 */
adt_error_t adt_bytearray_trim_left(adt_bytearray_t *self, const uint8_t *pSrc);

/**
 * \brief Changes the size of the array to u32NewLen.
 *
 * If the new size is larger than current capacity, memory is allocated.
 *
 * \param self Pointer to the byte array.
 * \param u32NewLen New size in bytes.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_bytearray_resize(adt_bytearray_t *self, uint32_t u32NewLen);

/**
 * \brief Appends a single byte to the end of the array.
 *
 * \param self Pointer to the byte array.
 * \param value Byte to append.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_bytearray_push(adt_bytearray_t *self, uint8_t value);

/**
 * \brief Returns a direct pointer to the underlying mutable byte buffer.
 *
 * \param self Pointer to the byte array.
 * \return Pointer to internal byte buffer, or NULL if empty or self is NULL.
 */
uint8_t *adt_bytearray_data(const adt_bytearray_t *self);

/**
 * \brief Returns a read-only pointer to the underlying byte buffer.
 *
 * \param self Pointer to the byte array.
 * \return Const pointer to internal byte buffer, or NULL if empty or self is NULL.
 */
uint8_t const* adt_bytearray_const_data(adt_bytearray_t const* self);

/**
 * \brief Returns the current number of bytes stored in the array.
 *
 * \param self Pointer to the byte array.
 * \return Number of bytes, or 0 if self is NULL.
 */
uint32_t adt_bytearray_length(const adt_bytearray_t *self);

/**
 * \brief Resets the length of the array to 0 without deallocating the underlying buffer.
 *
 * \param self Pointer to the byte array.
 */
void adt_bytearray_clear(adt_bytearray_t *self);

/**
 * \brief Compares two byte arrays for equality (equal length and identical byte content).
 *
 * \param lhs Left-hand side byte array.
 * \param rhs Right-hand side byte array.
 * \return true if contents and length match, false otherwise.
 */
bool adt_bytearray_equals(const adt_bytearray_t *lhs, const adt_bytearray_t *rhs);

/**
 * \brief Compares the byte array content with an external raw buffer.
 *
 * \param self Pointer to the byte array.
 * \param data Pointer to external byte buffer.
 * \param dataLen Length of external buffer in bytes.
 * \return true if array length equals dataLen and byte contents match, false otherwise.
 */
bool adt_bytearray_data_equals(const adt_bytearray_t* self, const uint8_t* data, uint32_t dataLen);

/**
 * \brief Wraps the current byte array contents in a new immutable adt_bytes_t object.
 *
 * \param self Pointer to the byte array.
 * \return Pointer to newly allocated adt_bytes_t object.
 */
struct adt_bytes_tag* adt_bytearray_bytes(const adt_bytearray_t *self);

#endif //ADT_BYTE_ARRAY_H
