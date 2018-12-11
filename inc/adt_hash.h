/*****************************************************************************
* \file:   		adt_hash.h
* \author:		Conny Gustafsson
* \date:		2013-06-08
* \brief:		hash data structure
*
* Copyright (c) 2013 Conny Gustafsson
*
******************************************************************************/
#ifndef ADT_HASH_H__
#define ADT_HASH_H__
#include <stdint.h>
#if defined(_MSC_PLATFORM_TOOLSET) && (_MSC_PLATFORM_TOOLSET<=110)
#ifndef _MSC_BOOL_DEFINED
#define _MSC_BOOL_DEFINED
#define false 0
#define true 1
typedef uint8_t bool;
#endif
#else
#include <stdbool.h>
#endif

#include "adt_stack.h"
#include "adt_ary.h"

#define ADT_HASH_VERSION 0.1.2

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
 * 	depth 0: 0xF (root node)
 * 	depth 1: 0xF0
 * 	depth 2: 0xF00
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

typedef struct adt_hit_tag{
	adt_hnode_t *pNode; //pointer to current node
	adt_hkey_t *pHkey;  //pointer to current hkey struct
	uint8_t u8Cur;   //0-8 or 0-15 depending on node type
}adt_hit_t;

typedef struct adt_hit_stored_tag{
	adt_hnode_t *pNode; //pointer to current node
	uint8_t u8Cur;   //0-8 or 0-15 depending on node type
}adt_hit_stored_t;

typedef struct adt_hash_tag{
	uint32_t u32Size;		//number of elements in hash
	adt_stack_t iter_stack;	//stack of hiter_stored objects
	adt_hit_t iter;			//iterator state
	adt_hnode_t *root;		//root node
	void (*pDestructor)(void*); //element destructor
} adt_hash_t;



/***************** Public Function Declarations *******************/
//Constructor/Destructor
adt_hash_t* adt_hash_new(void (*pDestructor)(void*));
void adt_hash_delete(adt_hash_t *self);
void adt_hash_create(adt_hash_t *self,void (*pDestructor)(void*));
void adt_hash_destroy(adt_hash_t *self);


//Accessors
void	 adt_hash_set(adt_hash_t *self, const char *pKey,  void *pVal);
void** adt_hash_get(const adt_hash_t *self, const char *pKey);
void** adt_hash_remove(adt_hash_t *self, const char *pKey);
void   adt_hash_iter_init(adt_hash_t *self);

/**
 * \param[in] this adt_hash
 * \param[out] pKey pointer to key string
 * \param[out] pKeyLen pointer to integer containing length of key string
 * \return pointer to value or NULL
 */
void** adt_hash_iter_next(adt_hash_t *self,const char **ppKey,uint32_t *pKeyLen);



//Utility functions
uint32_t 	adt_hash_length(const adt_hash_t *self);
bool		adt_hash_exists(const adt_hash_t *self, const char *pKey);
uint32_t	adt_hash_keys(adt_hash_t *self,adt_ary_t* pArray);

#endif //ADT_HASH_H__
