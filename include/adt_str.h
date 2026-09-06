/*****************************************************************************
* \file      adt_str.h
* \author    Conny Gustafsson
* \date      2013-06-01
* \brief     String Container
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_STR_H__
#define ADT_STR_H__

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "adt_bytearray.h"
#include "adt_error.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

typedef uint8_t adt_str_encoding_t;
#define ADT_STR_ENCODING_UNKNOWN ((adt_str_encoding_t) 0u) //Unknown/unsupported encoding
#define ADT_STR_ENCODING_ASCII   ((adt_str_encoding_t) 1u) //all characters in string are in the range 0..127
#define ADT_STR_ENCODING_UTF8    ((adt_str_encoding_t) 2u) //string is encoded using utf-8
#define ADT_STR_ENCODING_UTF16   ((adt_str_encoding_t) 3u) //string is encoded using utf-16

#define ADT_UTF8_INVALID_ARGUMENT -1
#define ADT_UTF8_INVALID_ENCODING -2

typedef struct adt_str_tag {
     int32_t s32Cur;   //Current size of the string (s32Cur<=s32Size)
     int32_t s32Size;  //Size of the allocated array
     uint8_t *pAlloc;  //Allocated array
     adt_error_t last_error;
     adt_str_encoding_t encoding;
} adt_str_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
//forward declarations
struct adt_bytes_tag;

/* constructors */

/**
 * \brief Initializes a string instance in place (stack or embedded allocation).
 *
 * \param self Pointer to an existing adt_str_t instance.
 */
void adt_str_create(adt_str_t *self);

/**
 * \brief Allocates and initializes a new empty string on the heap.
 *
 * \return Pointer to newly allocated string, or NULL on failure.
 */
adt_str_t *adt_str_new(void);

/**
 * \brief Creates an independent duplicate of an existing string.
 *
 * \param other Pointer to string to clone.
 * \return Pointer to newly allocated duplicate string, or NULL on failure.
 */
adt_str_t *adt_str_clone(const adt_str_t* other);

/**
 * \brief Creates a new string by concatenating two strings.
 *
 * \param lhs Left-hand side string.
 * \param rhs Right-hand side string.
 * \return Newly allocated combined string, or NULL on failure.
 */
adt_str_t *adt_str_concat(const adt_str_t *lhs, const adt_str_t *rhs);

/**
 * \brief Allocates a new string from a byte pointer range [pBegin, pEnd).
 *
 * \param pBegin Pointer to beginning of byte range.
 * \param pEnd Pointer to end of byte range.
 * \return Pointer to newly allocated string, or NULL on failure.
 */
adt_str_t *adt_str_new_bstr(const uint8_t *pBegin, const uint8_t *pEnd);

/**
 * \brief Allocates a new string initialized from a null-terminated C string.
 *
 * \param other Source null-terminated C string.
 * \return Pointer to newly allocated string, or NULL on failure.
 */
adt_str_t *adt_str_new_cstr(const char * other);

/**
 * \brief Allocates a new string initialized from a byte array's contents.
 *
 * \param bytearray Source byte array.
 * \return Pointer to newly allocated string, or NULL on failure.
 */
adt_str_t *adt_str_new_bytearray(adt_bytearray_t *bytearray);

/**
 * \brief Allocates a new empty string configured for UTF-8 encoding.
 *
 * \return Pointer to newly allocated string, or NULL on failure.
 */
adt_str_t *adt_str_new_utf8(void);


/* destructors */

/**
 * \brief Frees the internal string buffer. Does not free self.
 *
 * \param self Pointer to the string to destroy.
 */
void adt_str_destroy(adt_str_t *self);

/**
 * \brief Destroys the string and frees self.
 *
 * \param self Pointer to the string to delete.
 */
void adt_str_delete(adt_str_t *self);

/**
 * \brief Type-erased destructor wrapper for adt_str_delete.
 *
 * Suitable for use as a generic destructor callback void (*)(void*) in other ADT containers.
 *
 * \param arg Pointer to the string (cast to void*).
 */
void adt_str_vdelete(void *arg);

/* string manipulation */

/**
 * \brief Overwrites this string with contents from another string.
 *
 * \param self Pointer to target string.
 * \param other Pointer to source string.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_str_set(adt_str_t *self, const adt_str_t* other);

/**
 * \brief Overwrites this string with data from a byte range [pBegin, pEnd).
 *
 * \param self Pointer to target string.
 * \param pBegin Pointer to start of range.
 * \param pEnd Pointer to end of range.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_str_set_bstr(adt_str_t *self, const uint8_t *pBegin, const uint8_t *pEnd);

/**
 * \brief Overwrites this string with contents from a null-terminated C string.
 *
 * \param self Pointer to target string.
 * \param cstr Source null-terminated C string.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_str_set_cstr(adt_str_t *self, const char *cstr);

/**
 * \brief Appends another string to the end of this string.
 *
 * \param self Pointer to target string.
 * \param other Pointer to string to append.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_str_append(adt_str_t *self, const adt_str_t* other);

/**
 * \brief Appends a byte range to the end of this string.
 *
 * \param self Pointer to target string.
 * \param pBegin Pointer to start of range.
 * \param pEnd Pointer to end of range.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_str_append_bstr(adt_str_t *self, const uint8_t *pBegin, const uint8_t *pEnd);

/**
 * \brief Appends a null-terminated C string to the end of this string.
 *
 * \param self Pointer to target string.
 * \param cstr Source null-terminated C string to append.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_str_append_cstr(adt_str_t *self, const char *cstr);

/**
 * \brief Prepends another string to the beginning of this string.
 *
 * \param self Pointer to target string.
 * \param other Pointer to string to prepend.
 */
void adt_str_prepend(adt_str_t *self, const adt_str_t* other);

/**
 * \brief Prepends a byte range to the beginning of this string.
 *
 * \param self Pointer to target string.
 * \param pBegin Pointer to start of range.
 * \param pEnd Pointer to end of range.
 */
void adt_str_prepend_bstr(adt_str_t *self, const uint8_t *pBegin, const uint8_t *pEnd);

/**
 * \brief Prepends a null-terminated C string to the beginning of this string.
 *
 * \param self Pointer to target string.
 * \param cstr Source null-terminated C string to prepend.
 */
void adt_str_prepend_cstr(adt_str_t *self, const char *cstr);

/**
 * \brief Appends a single character or byte to the end of the string.
 *
 * \param self Pointer to the string.
 * \param c Character or byte to append.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_str_push(adt_str_t *self, const int c);

/**
 * \brief Removes and returns the last character or byte from the string.
 *
 * \param self Pointer to the string.
 * \return The removed character code, or -1 if the string is empty or self is NULL.
 */
int adt_str_pop(adt_str_t *self);

/* getters */

/**
 * \brief Returns the character code at the specified index.
 *
 * \param self Pointer to the string.
 * \param index Zero-based character index.
 * \return Character code, or -1 if index is out of bounds.
 */
int adt_str_charAt(adt_str_t *self, int index);

/**
 * \brief Returns a pointer to a null-terminated C string representation.
 *
 * \param self Pointer to the string.
 * \return Null-terminated C string pointer, or NULL if self is NULL.
 */
const char* adt_str_cstr(adt_str_t *self);

/**
 * \brief Retrieves pointers to the beginning and end of the string buffer.
 *
 * \param self Pointer to the string.
 * \param ppBegin Pointer to receive address of first character.
 * \param ppEnd Pointer to receive address one past the last character.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_str_bstr(adt_str_t *self, const char **ppBegin, const char **ppEnd);

/**
 * \brief Creates a new heap-allocated mutable adt_bytearray_t copy of the string contents.
 *
 * \param self Pointer to the string.
 * \return Pointer to newly allocated adt_bytearray_t, or NULL on failure.
 */
adt_bytearray_t *adt_str_bytearray(adt_str_t *self);

/**
 * \brief Creates a new heap-allocated immutable adt_bytes_t copy of the string contents.
 *
 * \param self Pointer to the string.
 * \return Pointer to newly allocated adt_bytes_t, or NULL on failure.
 */
struct adt_bytes_tag *adt_str_bytes(adt_str_t *self);

/**
 * \brief Returns direct pointer to internal raw buffer.
 *
 * \param self Pointer to the string.
 * \return Pointer to buffer bytes, or NULL if empty or self is NULL.
 */
uint8_t* adt_str_data(adt_str_t const* self);

/* utility */

/**
 * \brief Configures the character encoding of the string.
 *
 * \param self Pointer to the string.
 * \param newEncoding Encoding identifier (e.g. ADT_STR_ENCODING_ASCII, ADT_STR_ENCODING_UTF8).
 */
void adt_str_setEncoding(adt_str_t *self, adt_str_encoding_t newEncoding);

/**
 * \brief Returns the current encoding of the string.
 *
 * \param self Pointer to the string.
 * \return Encoding identifier.
 */
adt_str_encoding_t adt_str_getEncoding(adt_str_t *self);

/**
 * \brief Checks if the string is empty.
 *
 * \param self Pointer to the string.
 * \return true if length is 0 or self is NULL, false otherwise.
 */
bool adt_str_is_empty(const adt_str_t* self);

/**
 * \brief Returns the number of characters / codepoints in the string according to its encoding.
 *
 * \param self Pointer to the string.
 * \return Number of characters/codepoints, or -1 on error.
 */
int32_t adt_str_length(const adt_str_t *self);

/**
 * \brief Returns the number of raw bytes currently in the string (excluding null terminator).
 *
 * \param self Pointer to the string.
 * \return Byte count, or -1 if self is NULL.
 */
int32_t adt_str_size(const adt_str_t *self);

/**
 * \brief Ensures allocated capacity is at least s32NewLen bytes.
 *
 * \param self Pointer to the string.
 * \param s32NewLen Minimum required capacity in bytes.
 * \return ADT_NO_ERROR on success, or an error code on failure.
 */
adt_error_t adt_str_reserve(adt_str_t *self, int32_t s32NewLen);

/**
 * \brief Resets string length to 0 without freeing the allocated buffer.
 *
 * \param self Pointer to the string.
 */
void adt_str_clear(adt_str_t *self);

/**
 * \brief Returns the most recent error recorded on this string.
 *
 * \param self Pointer to the string.
 * \return Error code, or ADT_INVALID_ARGUMENT_ERROR if self is NULL.
 */
adt_error_t adt_str_get_last_error(const adt_str_t *self);

/**
 * \brief Compares two strings for exact equality.
 *
 * \param self Pointer to first string.
 * \param other Pointer to second string.
 * \return true if byte contents and size match, false otherwise.
 */
bool adt_str_equal(const adt_str_t *self, const adt_str_t* other);

/**
 * \brief Compares string content with a byte range [pBegin, pEnd).
 *
 * \param self Pointer to string.
 * \param pBegin Pointer to start of range.
 * \param pEnd Pointer to end of range.
 * \return true if content matches range, false otherwise.
 */
bool adt_str_equal_bstr(const adt_str_t *self, const char *pBegin, const char *pEnd);

/**
 * \brief Compares string content with a null-terminated C string.
 *
 * \param self Pointer to string.
 * \param cstr Null-terminated C string.
 * \return true if contents match, false otherwise.
 */
bool adt_str_equal_cstr(const adt_str_t *self, const char *cstr);

/**
 * \brief Lexicographically compares two strings.
 *
 * \param self Pointer to first string.
 * \param other Pointer to second string.
 * \return Positive if self < other, 0 if equal, negative if self > other.
 */
int adt_str_lt(const adt_str_t *self, const adt_str_t *other);

/**
 * \brief Type-erased comparator helper for sorting arrays of string pointers.
 *
 * \param a Pointer to first string pointer.
 * \param b Pointer to second string pointer.
 * \return Positive if *a < *b, 0 if equal, negative if *a > *b.
 */
int adt_str_vlt(const void *a, const void *b);


/* function aliases */
#define adt_str_delete_void adt_str_vdelete
#define adt_str_dup adt_str_new_cstr
#define adt_str_make adt_str_new_bstr
#define adt_str_reset adt_str_destroy
#define adt_str_getLastError adt_str_get_last_error

#ifdef UNIT_TEST
adt_str_encoding_t adt_utf8_checkEncodingAndSize(const uint8_t *strBuf, int32_t maxBufLen, int32_t *strLen);
int32_t adt_utf8_readCodePoint(const uint8_t *strBuf, int32_t bufLen, int *codePoint);
#endif

#ifdef __cplusplus
}
#endif

#endif //ADT_STR_H__
