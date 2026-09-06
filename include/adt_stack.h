/*****************************************************************************
* \file      adt_stack.h
* \author    Conny Gustafsson
* \date      2013-07-21
* \brief     stack data structure
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_STACK_H__
#define ADT_STACK_H__

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "adt_error.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

typedef struct adt_stack_tag{
 void **ppAlloc;  //array of (void*)
 uint32_t u32AllocLen; //number of elements allocated
 uint32_t u32CurLen; //number of elements currently in the stack
 uint32_t u32MinLen; //minimum (reserved) number of items in stack
 void (*pDestructor)(void*);
} adt_stack_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Allocates and initializes a new stack on the heap.
 *
 * \param pDestructor Optional destructor callback function for element cleanup, or NULL.
 * \return Pointer to newly allocated stack, or NULL on failure.
 */
adt_stack_t* adt_stack_new(void (*pDestructor)(void*));

/**
 * \brief Destroys all elements using the configured destructor, frees internal buffer, and frees self.
 *
 * \param self Pointer to the stack to delete.
 */
void    adt_stack_delete(adt_stack_t *self);

/**
 * \brief Initializes a stack instance in place (stack or embedded allocation).
 *
 * \param self Pointer to an existing adt_stack_t instance.
 * \param pDestructor Optional destructor callback function for element cleanup, or NULL.
 */
void  adt_stack_create(adt_stack_t *self, void (*pDestructor)(void*));

/**
 * \brief Destroys all elements using the configured destructor and frees the internal buffer. Does not free self.
 *
 * \param self Pointer to the stack to destroy.
 */
void  adt_stack_destroy(adt_stack_t *self);

/**
 * \brief Destroys all elements and clears the stack.
 *
 * \param self Pointer to the stack.
 */
void  adt_stack_clear(adt_stack_t *self);


//Accessors

/**
 * \brief Pushes an element pointer onto the top of the stack.
 *
 * \param self Pointer to the stack.
 * \param pVal Element pointer to push.
 * \return Error code (ADT_NO_ERROR on success, ADT_MEM_ERROR on allocation failure, ADT_INVALID_ARGUMENT_ERROR if self is NULL).
 */
adt_error_t adt_stack_push(adt_stack_t *self, void *pVal);

/**
 * \brief Inspects the element at the top of the stack without removing it.
 *
 * \param self Pointer to the stack.
 * \return Pointer to the top element, or NULL if the stack is empty or self is NULL.
 */
void* adt_stack_top(const adt_stack_t *self);

/**
 * \brief Removes and returns the element at the top of the stack without calling its destructor.
 *
 * \param self Pointer to the stack.
 * \return Pointer to the popped element, or NULL if the stack is empty or self is NULL.
 */
void* adt_stack_pop(adt_stack_t *self);

//Utility functions

/**
 * \brief Ensures that the stack has at least u32Len capacity allocated.
 *
 * \param self Pointer to the stack.
 * \param u32Len Desired minimum capacity.
 * \return Error code (ADT_NO_ERROR on success, ADT_MEM_ERROR on allocation failure, ADT_INVALID_ARGUMENT_ERROR if self is NULL).
 */
adt_error_t adt_stack_reserve(adt_stack_t *self, uint32_t u32Len);

/**
 * \brief Resizes the stack allocation to exactly u32Len elements, destroying truncated elements if shrinking.
 *
 * \param self Pointer to the stack.
 * \param u32Len New allocated capacity.
 * \return Error code (ADT_NO_ERROR on success, ADT_MEM_ERROR on allocation failure, ADT_INVALID_ARGUMENT_ERROR if self is NULL).
 */
adt_error_t adt_stack_resize(adt_stack_t *self, uint32_t u32Len);

/**
 * \brief Returns the number of elements currently stored in the stack.
 *
 * \param self Pointer to the stack.
 * \return Number of elements, or 0 if self is NULL.
 */
uint32_t adt_stack_size(const adt_stack_t *self);

#endif //ADT_STACK_H__
