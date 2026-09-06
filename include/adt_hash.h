/*****************************************************************************
* \file      adt_hash.h
* \author    Conny Gustafsson
* \date      2013-06-08
* \brief     hash data structure
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef ADT_HASH_H__
#define ADT_HASH_H__

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>

#include "adt_error.h"
#include "adt_ary.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

#define ADT_HASH_MAX_DEPTH 8

/*
 * ADT-hash is implemented as a tree of nodes containing items. Each item contains a (unique) hash value+linked
 * list of key/value pairs (for keys that have the same hash value).
 *
 * One node can contain up to 8 items (hash values).
 * when the 9th item is to be inserted the node is transformed in the following way:
 * 1. 16 new child nodes are created
 * 2. the 8 previous key/value pairs are distributed into the 16 nodes based on their hash_key and the nodes depth
 * in the tree.
 * example for depth 0 (root node):
 * if hash_key & 0xF = 0: insert the key/value pair into child[0]
 * if hash_key & 0xF = 1: insert the key/value pair into child[1]
 * if hash_key & 0xF = 2: insert the key/value pair into child[2]
 * ...
 * if hash_key & 0xF = 0xF: insert the key/value pair into child[15]
 *
 * 3. the 9th key/value pair is now inserted into the child node which matches its hash_key & 0xF
 * 4. when one of the child nodes becomes full (8 key/value pairs) the child node splits into 16 new child nodes
 * 5. for each descendant node, the hash_key pattern is shifted 4 bits to the left:
 *  depth 0: 0xF (root node)
 *  depth 1: 0xF0
 *  depth 2: 0xF00
 *  depth 3: 0xF000
 *  depth 4: 0xF0000
 *  depth 5: 0xF00000
 *  depth 6: 0xF000000
 *  depth 7: 0xF0000000
 *  depth 8: N/A (most descendant node)
 *
 *  in order to find the item in the tree with matching hash value:
 *  Start on the root node (depth 0) and go to the child node at index ((hash_key & 0xF) >> 0)
 *  on the child node go to the grand child at index ((hash_key & 0xF0) >> 4)
 *  This pattern continues until we reach a node with 8 items or less in which case it performs a linear search
 *  of these 1-8 items to find the matching hash key.
 *
 *  Since a 32-bit value can contain at most 8 nibbles, the maximum depth is thus 8. The performance of the algorithm
 *  should be at most 8 shift/and operations O(1) plus linear search of up to 8 hash value comparisons O(n).
 *
 *  Once the item with matching hash value has been found it continues with another linear search of the keys with
 *  that exact hash value (again O(n), if there are any). Once the key has been matched the value belonging to that key
 *  has been found.
 *
 */

typedef struct adt_hkey_tag{
 char *key;
 void *val;
 struct adt_hkey_tag *next;
} adt_hkey_t;

typedef struct adt_hmatch_tag{
 uint32_t u32Hash;
 adt_hkey_t *key;
} adt_hmatch_t;

typedef struct adt_hnode_tag{
 uint8_t u8Width;
 uint8_t u8Cur;
 uint8_t u8Depth;
 union {
  adt_hmatch_t *match; //1,2,4 or 8 HMatch_t structures
  struct adt_hnode_tag *node; //16 HNode_t structures
 } child;
} adt_hnode_t;

typedef struct adt_hit_frame_tag{
 adt_hnode_t *pNode; //pointer to current node
 uint8_t u8Cur;      //0-8 or 0-15 depending on node type
} adt_hit_frame_t;

typedef struct adt_hash_tag{
 int32_t u32Size;  //number of elements in hash
 adt_hnode_t *root;  //root node
 void (*pDestructor)(void*); //element destructor
 adt_hit_frame_t iter_frames[ADT_HASH_MAX_DEPTH + 1];
 int8_t iter_depth;
 adt_error_t last_error;
 adt_hkey_t *iter_hkey;
} adt_hash_t;



//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Allocates and initializes a new hash table on the heap.
 *
 * \param pDestructor Optional element destructor callback for value cleanup, or NULL.
 * \return Pointer to newly allocated hash table, or NULL on failure.
 */
adt_hash_t* adt_hash_new(void (*pDestructor)(void*));

/**
 * \brief Destroys all stored values using the configured destructor, frees table nodes, and frees self.
 *
 * \param self Pointer to the hash table to delete.
 */
void adt_hash_delete(adt_hash_t *self);

/**
 * \brief Type-erased destructor wrapper for adt_hash_delete.
 *
 * Suitable for use as a generic destructor callback void (*)(void*) in other ADT containers.
 *
 * \param arg Pointer to adt_hash_t instance to delete (cast from void*).
 */
void adt_hash_vdelete(void *arg);

/**
 * \brief Initializes a hash table instance in place (stack or embedded allocation).
 *
 * \param self Pointer to an existing adt_hash_t instance.
 * \param pDestructor Optional element destructor callback for value cleanup, or NULL.
 */
void adt_hash_create(adt_hash_t *self, void (*pDestructor)(void*));

/**
 * \brief Destroys all stored values using the configured destructor and frees internal nodes. Does not free self.
 *
 * \param self Pointer to the hash table to destroy.
 */
void adt_hash_destroy(adt_hash_t *self);

/**
 * \brief Removes all elements from the hash table, invoking the destructor on each stored value if configured.
 *
 * \param self Pointer to the hash table.
 */
void adt_hash_clear(adt_hash_t *self);


//Accessors

/**
 * \brief Inserts or updates a key-value pair in the hash table.
 *
 * \param self Pointer to the hash table.
 * \param pKey Null-terminated string key.
 * \param pVal Pointer to value to associate with key.
 */
void adt_hash_set(adt_hash_t *self, const char *pKey,  void *pVal);

/**
 * \brief Inserts a key-value pair only if the key is not already present in the hash table.
 *
 * If the key already exists, the table is not modified and false is returned.
 *
 * \param self Pointer to the hash table.
 * \param pKey Null-terminated string key.
 * \param pVal Pointer to value to associate with key.
 * \return true if the key was inserted, false if the key already exists or arguments are invalid.
 */
bool adt_hash_insert(adt_hash_t *self, const char *pKey, void *pVal);

/**
 * \brief Retrieves a pointer to the value slot for the specified key.
 *
 * \param self Pointer to the hash table.
 * \param pKey Null-terminated string key to look up.
 * \return Pointer to value pointer (void**), or NULL if key is not found.
 */
void** adt_hash_get(const adt_hash_t *self, const char *pKey);

/**
 * \brief Retrieves the stored value pointer for the specified key directly.
 *
 * \param self Pointer to the hash table.
 * \param pKey Null-terminated string key to look up.
 * \return Stored value pointer (void*), or NULL if key is not found.
 */
void* adt_hash_value(const adt_hash_t *self, const char *pKey);

/**
 * \brief Removes the key-value pair from the hash table without invoking the element destructor.
 *
 * \param self Pointer to the hash table.
 * \param pKey Null-terminated string key to remove.
 * \return The removed value pointer (void*), or NULL if key was not found.
 */
void* adt_hash_remove(adt_hash_t *self, const char *pKey);

/**
 * \brief Removes the key-value pair from the hash table and invokes the element destructor on the value.
 *
 * \param self Pointer to the hash table.
 * \param pKey Null-terminated string key to remove.
 * \return true if the key was found and erased, false otherwise.
 */
bool adt_hash_erase(adt_hash_t *self, const char *pKey);

/**
 * \brief Initializes or resets the hash table iterator to the beginning.
 *
 * \param self Pointer to the hash table.
 */
void   adt_hash_iter_init(adt_hash_t *self);

/**
 * \brief Advances the iterator and retrieves the next key-value pair.
 *
 * \param self Pointer to the hash table.
 * \param ppKey Optional pointer to receive the current key string (can be NULL).
 * \return Pointer to the value pointer (void**), or NULL when iteration ends.
 */
void** adt_hash_iter_next(adt_hash_t *self, const char **ppKey);

/**
 * \brief Iterates over all key-value pairs in the hash table, calling the callback for each pair.
 *
 * This function does not modify the table and can be safely called on a const table.
 *
 * \param self Pointer to the hash table.
 * \param callback Callback function taking (const char *key, void *val, void *arg).
 * \param arg User-defined argument passed to each callback invocation.
 */
void adt_hash_foreach(const adt_hash_t *self, void (*callback)(const char *key, void *val, void *arg), void *arg);


//Utility functions

/**
 * \brief Returns the number of elements in the hash table.
 *
 * \param self Pointer to the hash table.
 * \return Number of elements, or -1 if self is NULL.
 */
int32_t adt_hash_length(const adt_hash_t *self);

/**
 * \brief Checks whether the hash table is empty (contains zero elements).
 *
 * \param self Pointer to the hash table.
 * \return true if self is NULL or table is empty, false otherwise.
 */
bool adt_hash_is_empty(const adt_hash_t *self);

/**
 * \brief Checks whether a key exists in the hash table.
 *
 * \param self Pointer to the hash table.
 * \param pKey Null-terminated string key to check.
 * \return true if key exists, false otherwise.
 */
bool adt_hash_exists(const adt_hash_t *self, const char *pKey);

/**
 * \brief Collects weak references to all string keys into the provided array.
 *
 * Keys are non-owning borrowed pointers referencing strings owned by the hash
 * table. The destination array must NOT have an active destructor enabled (or
 * must have it disabled via adt_ary_destructor_enable); passing an array with
 * an active destructor is an error.
 *
 * \param self Pointer to the hash table.
 * \param pArray Destination array (must have no destructor enabled).
 * \return Number of keys copied (>= 0), or -1 on error.
 */
int32_t adt_hash_keys(adt_hash_t *self, adt_ary_t* pArray);

/**
 * \brief Collects weak references to all value pointers into the provided array.
 *
 * Values are non-owning borrowed pointers referencing values stored in the hash
 * table. The destination array must NOT have an active destructor enabled (or
 * must have it disabled via adt_ary_destructor_enable); passing an array with
 * an active destructor is an error.
 *
 * \param self Pointer to the hash table.
 * \param pArray Destination array (must have no destructor enabled).
 * \return Number of values copied (>= 0), or -1 on error.
 */
int32_t adt_hash_values(adt_hash_t *self, adt_ary_t* pArray);

/**
 * \brief Returns the most recent error recorded on this hash table.
 *
 * \param self Pointer to the hash table.
 * \return Error code, or ADT_INVALID_ARGUMENT_ERROR if self is NULL.
 */
adt_error_t adt_hash_get_last_error(const adt_hash_t *self);

#endif //ADT_HASH_H__
