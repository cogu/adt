/*****************************************************************************
* \file      adt_bytes.h
* \author    Conny Gustafsson
* \date      2019-08-13
* \brief     An immutable byte array
*
* Copyright (c) 2019-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_BYTES_H
#define ADT_BYTES_H

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
struct adt_bytearray_tag;


typedef struct adt_bytes_tag
{
   uint32_t dataLen;
   uint8_t *dataBuf;
} adt_bytes_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes a bytes instance with a copy of a raw byte buffer.
 *
 * \param self Pointer to an existing adt_bytes_t instance.
 * \param dataBuf Pointer to source byte buffer to copy (may be NULL if dataLen is 0).
 * \param dataLen Number of bytes to copy.
 * \return ADT_NO_ERROR on success, ADT_MEM_ERROR on allocation failure, or ADT_INVALID_ARGUMENT_ERROR if self is NULL.
 */
adt_error_t adt_bytes_create(adt_bytes_t *self, const uint8_t *dataBuf, uint32_t dataLen);

/**
 * \brief Frees the internal buffer allocated by the bytes object. Does not free the self pointer.
 *
 * \param self Pointer to the bytes object to destroy.
 */
void adt_bytes_destroy(adt_bytes_t *self);

/**
 * \brief Allocates and initializes a new bytes object on the heap with a copy of raw data.
 *
 * \param dataBuf Pointer to source byte buffer to copy.
 * \param dataLen Number of bytes to copy.
 * \return Pointer to newly allocated bytes object, or NULL on failure.
 */
adt_bytes_t *adt_bytes_new(const uint8_t *dataBuf, uint32_t dataLen);

/**
 * \brief Allocates and initializes a new bytes object from a null-terminated C string.
 *
 * \param cstr Source null-terminated C string (excluding null terminator from stored data).
 * \return Pointer to newly allocated bytes object, or NULL on failure.
 */
adt_bytes_t *adt_bytes_new_cstr(const char *cstr);

/**
 * \brief Creates an independent duplicate of an existing bytes object.
 *
 * \param other Pointer to bytes object to clone.
 * \return Pointer to newly allocated duplicate bytes object, or NULL on failure.
 */
adt_bytes_t *adt_bytes_clone(const adt_bytes_t* other);

/**
 * \brief Destroys the internal buffer and frees the self pointer.
 *
 * \param self Pointer to the bytes object to delete.
 */
void adt_bytes_delete(adt_bytes_t *self);

/**
 * \brief Type-erased destructor wrapper for adt_bytes_delete.
 *
 * Suitable for use as a generic destructor callback void (*)(void*) in other ADT containers.
 *
 * \param arg Pointer to the bytes object (cast to void*).
 */
void adt_bytes_vdelete(void *arg);

/**
 * \brief Returns the number of bytes stored in the object.
 *
 * \param self Pointer to the bytes object.
 * \return Number of bytes, or 0 if self is NULL.
 */
uint32_t adt_bytes_length(const adt_bytes_t *self);

/**
 * \brief Returns a read-only pointer to the underlying byte buffer.
 *
 * \param self Pointer to the bytes object.
 * \return Const pointer to internal byte buffer, or NULL if empty or self is NULL.
 */
const uint8_t* adt_bytes_const_data(const adt_bytes_t *self);

/**
 * \brief Converts the immutable bytes object into a mutable adt_bytearray_t on the heap.
 *
 * \param self Pointer to the bytes object.
 * \return Pointer to newly allocated adt_bytearray_t, or NULL on failure.
 */
struct adt_bytearray_tag *adt_bytes_bytearray(const adt_bytes_t *self);

/**
 * \brief Compares two bytes objects for equality (equal length and identical byte content).
 *
 * \param self Pointer to the first bytes object.
 * \param other Pointer to the second bytes object.
 * \return true if contents and length match, false otherwise.
 */
bool adt_bytes_equals(const adt_bytes_t *self, const adt_bytes_t *other);

#endif //ADT_BYTES_H
