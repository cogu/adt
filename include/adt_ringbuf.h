/*****************************************************************************
* \file      adt_ringbuf.h
* \author    Conny Gustafsson
* \date      2013-12-19
* \brief     Ringbuffer data structure
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_RINGBUF_H__
#define ADT_RINGBUF_H__

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * @name Ringbuffer Status / Error Codes
 * @{
 */
#ifndef BUF_E_OK
#define BUF_E_OK        0 /**< Operation completed successfully. */
#endif
#ifndef BUF_E_NOT_OK
#define BUF_E_NOT_OK    1 /**< Operation failed due to invalid arguments or allocation error. */
#endif
#ifndef BUF_E_OVERFLOW
#define BUF_E_OVERFLOW  2 /**< Ring buffer is full; element cannot be inserted. */
#endif
#ifndef BUF_E_UNDERFLOW
#define BUF_E_UNDERFLOW 3 /**< Ring buffer is empty; element cannot be removed or peeked. */
#endif
/** @} */

/**
 * Ringbuffer error status type.
 */
typedef uint8_t adt_buf_err_t;

/**
 * Static circular FIFO ring buffer for elements of fixed byte size.
 * Operates on caller-supplied memory with zero dynamic heap allocation.
 */
typedef struct adt_rbfs_tag
{
   uint8_t* u8Buffer;      /**< Pointer to caller-supplied storage buffer */
   uint8_t* u8WritePtr;    /**< Pointer to next insertion position */
   uint8_t* u8ReadPtr;     /**< Pointer to next extraction position */
   uint16_t u16MaxNumElem; /**< Maximum number of elements buffer can hold */
   uint16_t u16NumElem;    /**< Current number of elements stored */
   uint8_t u8ElemSize;     /**< Size of each element in bytes */
} adt_rbfs_t;

/**
 * Specialized embedded circular FIFO ring buffer for uint16_t values.
 * Operates on caller-supplied uint16_t array with zero dynamic heap allocation.
 */
typedef struct adt_rbfu16_tag
{
   uint16_t* u16Buffer;    /**< Pointer to caller-supplied uint16_t buffer */
   uint16_t* u16WritePtr;   /**< Pointer to next insertion position */
   uint16_t* u16ReadPtr;    /**< Pointer to next extraction position */
   uint16_t u16MaxNumElem; /**< Maximum number of uint16_t elements buffer can hold */
   uint16_t u16NumElem;    /**< Current number of uint16_t elements stored */
} adt_rbfu16_t;

#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))

#define ADT_RBFSH_MIN_NUM_ELEMS_DEFAULT 10   /**< Default initial element capacity (1-65535) */
#define ADT_RBFSH_MAX_NUM_ELEM_DEFAULT 65535 /**< Default maximum element capacity (1-65535) */

/**
 * Dynamically resizing heap-managed circular FIFO ring buffer for fixed-size elements.
 */
typedef struct adt_rbfh_tag
{
   uint8_t* u8AllocBuf;    /**< Heap-allocated storage buffer */
   uint8_t* u8WritePtr;    /**< Pointer to next insertion position */
   uint8_t* u8ReadPtr;     /**< Pointer to next extraction position */
   uint16_t u16AllocLen;   /**< Current allocated capacity in elements */
   uint16_t u16MaxNumElem; /**< Upper bound on element capacity */
   uint16_t u16NumElem;    /**< Current number of elements stored */
   uint8_t u8ElemSize;     /**< Size of each element in bytes */
} adt_rbfh_t;
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
/**
 * Initializes a static element ring buffer with caller-provided storage.
 *
 * @param rbf Pointer to the adt_rbfs_t structure to initialize.
 * @param u8Buffer Pointer to contiguous caller-allocated storage (at least u32NumElem * u8ElemSize bytes).
 * @param u32NumElem Maximum number of elements that can be stored.
 * @param u8ElemSize Size of each element in bytes.
 * @return BUF_E_OK (0) on success.
 */
uint8_t adt_rbfs_create(adt_rbfs_t* rbf, uint8_t* u8Buffer, uint16_t u32NumElem, uint8_t u8ElemSize);

/**
 * Inserts an element into the static ring buffer.
 *
 * Copies u8ElemSize bytes from u8Data into the buffer.
 *
 * @param rbf Pointer to the ring buffer.
 * @param u8Data Pointer to the element data to copy into the buffer.
 * @return BUF_E_OK (0) on success, or BUF_E_OVERFLOW (2) if the buffer is full.
 */
uint8_t adt_rbfs_insert(adt_rbfs_t* rbf, const uint8_t* u8Data);

/**
 * Removes the oldest element from the static ring buffer.
 *
 * Copies u8ElemSize bytes from the buffer into u8Data and advances the read pointer.
 *
 * @param rbf Pointer to the ring buffer.
 * @param u8Data Pointer to destination memory where the element will be copied.
 * @return BUF_E_OK (0) on success, or BUF_E_UNDERFLOW (3) if the buffer is empty.
 */
uint8_t adt_rbfs_remove(adt_rbfs_t* rbf, uint8_t* u8Data);

/**
 * Peeks at the oldest element without removing it from the static ring buffer.
 *
 * Copies u8ElemSize bytes into u8Data without advancing the read pointer.
 *
 * @param rbf Pointer to the ring buffer.
 * @param u8Data Pointer to destination memory where the element will be copied.
 * @return BUF_E_OK (0) on success, or BUF_E_UNDERFLOW (3) if the buffer is empty.
 */
uint8_t adt_rbfs_peek(const adt_rbfs_t* rbf, uint8_t* u8Data);

/**
 * Returns the number of elements currently stored in the static ring buffer.
 *
 * @param rbf Pointer to the ring buffer.
 * @return Number of stored elements.
 */
uint16_t adt_rbfs_size(const adt_rbfs_t* rbf);

/**
 * Returns the remaining capacity (free slots) in the static ring buffer.
 *
 * @param rbf Pointer to the ring buffer.
 * @return Number of additional elements that can be inserted before full.
 */
uint16_t adt_rbfs_free(const adt_rbfs_t* rbf);

/**
 * Clears the static ring buffer, resetting element count and read/write pointers.
 *
 * @param rbf Pointer to the ring buffer.
 */
void adt_rbfs_clear(adt_rbfs_t* rbf);

/**
 * Initializes an embedded uint16_t ring buffer with caller-provided storage.
 *
 * @param rbf Pointer to the adt_rbfu16_t structure to initialize.
 * @param u16Buffer Pointer to caller-allocated array of uint16_t elements.
 * @param u16NumElem Maximum number of uint16_t elements that can be stored.
 * @return BUF_E_OK (0) on success, or BUF_E_NOT_OK (1) if pointers are NULL or count is 0.
 */
uint8_t adt_rbfu16_create(adt_rbfu16_t* rbf, uint16_t* u16Buffer, uint16_t u16NumElem);

/**
 * Inserts a uint16_t value into the ring buffer.
 *
 * @param rbf Pointer to the ring buffer.
 * @param u16Data 16-bit unsigned integer value to store.
 * @return BUF_E_OK (0) on success, BUF_E_OVERFLOW (2) if full, or BUF_E_NOT_OK (1) on invalid argument.
 */
uint8_t adt_rbfu16_insert(adt_rbfu16_t* rbf, uint16_t u16Data);

/**
 * Removes the oldest uint16_t value from the ring buffer.
 *
 * @param rbf Pointer to the ring buffer.
 * @param u16Data Pointer to destination uint16_t where the removed value will be stored.
 * @return BUF_E_OK (0) on success, BUF_E_UNDERFLOW (3) if empty, or BUF_E_NOT_OK (1) on invalid argument.
 */
uint8_t adt_rbfu16_remove(adt_rbfu16_t* rbf, uint16_t* u16Data);

/**
 * Peeks at the oldest uint16_t value without removing it from the buffer.
 *
 * @param rbf Pointer to the ring buffer.
 * @param u16Data Pointer to destination uint16_t where the value will be stored.
 * @return BUF_E_OK (0) on success, BUF_E_UNDERFLOW (3) if empty, or BUF_E_NOT_OK (1) on invalid argument.
 */
uint8_t adt_rbfu16_peek(const adt_rbfu16_t* rbf, uint16_t* u16Data);

/**
 * Returns the number of uint16_t elements currently stored in the ring buffer.
 *
 * @param rbf Pointer to the ring buffer.
 * @return Number of stored elements.
 */
uint16_t adt_rbfu16_length(const adt_rbfu16_t* rbf);

#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))
/**
 * Initializes a heap-managed ring buffer with default capacity limits.
 *
 * Allocates initial memory for ADT_RBFSH_MIN_NUM_ELEMS_DEFAULT elements.
 * The buffer will automatically grow dynamically on demand up to ADT_RBFSH_MAX_NUM_ELEM_DEFAULT.
 *
 * @param self Pointer to the adt_rbfh_t structure to initialize.
 * @param u8ElemSize Size of each element in bytes.
 * @return BUF_E_OK (0) on success, or BUF_E_NOT_OK (1) on allocation failure or invalid argument.
 */
adt_buf_err_t adt_rbfh_create(adt_rbfh_t* self, uint8_t u8ElemSize);

/**
 * Initializes a heap-managed ring buffer with custom lower and upper capacity limits.
 *
 * @param self Pointer to the adt_rbfh_t structure to initialize.
 * @param u8ElemSize Size of each element in bytes.
 * @param u16MinNumElems Initial minimum number of elements allocated.
 * @param u16MaxNumElems Maximum element capacity before growth is capped (0 for no upper limit up to 65535).
 * @return BUF_E_OK (0) on success, or BUF_E_NOT_OK (1) on allocation failure or invalid argument.
 */
adt_buf_err_t adt_rbfh_create_with_params(adt_rbfh_t* self, uint8_t u8ElemSize, uint16_t u16MinNumElems, uint16_t u16MaxNumElems);

/**
 * Releases heap memory allocated by the ring buffer's internal storage.
 *
 * @param self Pointer to the ring buffer.
 */
void adt_rbfh_destroy(adt_rbfh_t* self);

/**
 * Allocates and initializes a new heap-managed ring buffer with default capacity limits.
 *
 * @param u8ElemSize Size of each element in bytes.
 * @return Pointer to newly allocated adt_rbfh_t, or NULL on allocation failure.
 */
adt_rbfh_t *adt_rbfh_new(uint8_t u8ElemSize);

/**
 * Allocates and initializes a new heap-managed ring buffer with custom capacity limits.
 *
 * @param u8ElemSize Size of each element in bytes.
 * @param u16MinNumElems Initial minimum number of elements allocated.
 * @param u16MaxNumElems Maximum element capacity (0 for unlimited up to 65535).
 * @return Pointer to newly allocated adt_rbfh_t, or NULL on allocation failure.
 */
adt_rbfh_t *adt_rbfh_new_with_params(uint8_t u8ElemSize, uint16_t u16MinNumElems, uint16_t u16MaxNumElems);

/**
 * Releases internal storage and frees the heap-allocated ring buffer object itself.
 *
 * @param self Pointer to ring buffer previously created with adt_rbfh_new or adt_rbfh_new_with_params.
 */
void adt_rbfh_delete(adt_rbfh_t* self);

/**
 * Inserts an element into the heap-managed ring buffer, growing internal storage if needed.
 *
 * Copies u8ElemSize bytes from u8Data. If capacity is reached, automatically resizes the internal
 * heap buffer up to u16MaxNumElem.
 *
 * @param self Pointer to the ring buffer.
 * @param u8Data Pointer to element data to insert.
 * @return BUF_E_OK (0) on success, BUF_E_OVERFLOW (2) if maximum capacity reached, or BUF_E_NOT_OK (1) on failure.
 */
adt_buf_err_t adt_rbfh_insert(adt_rbfh_t* self, const uint8_t* u8Data);

/**
 * Removes the oldest element from the heap-managed ring buffer.
 *
 * Copies u8ElemSize bytes into u8Data and advances the read pointer.
 *
 * @param self Pointer to the ring buffer.
 * @param u8Data Pointer to destination buffer where element will be copied.
 * @return BUF_E_OK (0) on success, or BUF_E_UNDERFLOW (3) if empty.
 */
adt_buf_err_t adt_rbfh_remove(adt_rbfh_t* self, uint8_t* u8Data);

/**
 * Peeks at the oldest element without removing it from the heap-managed ring buffer.
 *
 * @param self Pointer to the ring buffer.
 * @param u8Data Pointer to destination buffer where element will be copied.
 * @return BUF_E_OK (0) on success, or BUF_E_UNDERFLOW (3) if empty.
 */
adt_buf_err_t adt_rbfh_peek(const adt_rbfh_t* self, uint8_t* u8Data);

/**
 * Returns the number of elements currently stored in the ring buffer.
 *
 * @param self Pointer to the ring buffer.
 * @return Number of stored elements.
 */
uint16_t adt_rbfh_length(const adt_rbfh_t* self);

/**
 * Returns how many additional elements can be inserted before maximum capacity is exhausted.
 *
 * @param self Pointer to the ring buffer.
 * @return Remaining element capacity.
 */
uint16_t adt_rbfh_free(const adt_rbfh_t* self);

/**
 * Resets the ring buffer, clearing all elements and rewinding read/write pointers.
 *
 * @param self Pointer to the ring buffer.
 */
void adt_rbfh_clear(adt_rbfh_t* self);

# ifdef UNIT_TEST
uint16_t adt_rbfh_nextLen(uint16_t wanted);
# endif
#endif //ADT_RBFH_ENABLE

#endif //ADT_RINGBUF_H__

