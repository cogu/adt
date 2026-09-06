/*****************************************************************************
* \file      adt_hash.c
* \author    Conny Gustafsson
* \date      2013-06-08
* \brief     hash data structure
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
# ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE
# endif
# ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 200809L
# endif
#endif
#include "adt_hash.h"
#include "adt_str.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void adt_hnode_create(adt_hnode_t *node);
static void adt_hnode_destroy(adt_hnode_t *node, void (*pDestructor)(void*));
static void adt_hnode_destroy_shallow(adt_hnode_t *node);
static adt_hnode_t *adt_hnode_new(void);
static void adt_hnode_delete(adt_hnode_t *node, void (*pDestructor)(void*));
static void adt_hnode_insert(adt_hnode_t *node, adt_hkey_t *key, uint32_t u32Hash);
static bool adt_hnode_set(adt_hnode_t *node, const char *key, void *val, uint32_t u32Hash, void (*pDestructor)(void*), bool overwrite);
static adt_hkey_t * adt_hnode_find(const adt_hnode_t *node, const char *key, uint32_t u32Hash);
static adt_hkey_t * adt_hnode_remove(adt_hnode_t *node, const char *key, uint32_t u32Hash);
static void adt_hnode_foreach(const adt_hnode_t *node, void (*callback)(const char *key, void *val, void *arg), void *arg);
static void adt_hkey_create(adt_hkey_t *hkey, const char *key, void *value);
static void adt_hkey_destroy(adt_hkey_t *hkey, void (*pDestructor)(void*));
static adt_hkey_t *adt_hkey_new(const char *key, void *value);
static void adt_hkey_delete(adt_hkey_t *hkey, void (*pDestructor)(void*));
static uint32_t adt_hash_string(const char * s);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

adt_hash_t* adt_hash_new(void (*pDestructor)(void*)){
    adt_hash_t *self = (adt_hash_t*) malloc(sizeof(adt_hash_t));
    if(self){
        adt_hash_create(self, pDestructor);
    }
    return self;
}

void adt_hash_delete(adt_hash_t *self){
    if(self){
        adt_hash_destroy(self);
        free(self);
    }
}

void adt_hash_vdelete(void *arg){
    adt_hash_delete((adt_hash_t*) arg);
}

void adt_hash_create(adt_hash_t *self, void (*pDestructor)(void*)){
    self->root = adt_hnode_new();
    self->u32Size = 0;
    self->pDestructor = pDestructor;
    self->iter_depth = -1;
    self->last_error = ADT_NO_ERROR;
    self->iter_hkey = NULL;
}

void adt_hash_destroy(adt_hash_t *self){
    if(self == NULL) return;
    if(self->root != NULL){
        adt_hnode_delete(self->root, self->pDestructor);
        self->root = NULL;
    }
    self->iter_depth = -1;
    self->iter_hkey = NULL;
    self->u32Size = 0;
}

void adt_hash_clear(adt_hash_t *self){
    if(self == NULL) return;
    if(self->root != NULL){
        adt_hnode_delete(self->root, self->pDestructor);
        self->root = NULL;
    }
    self->root = adt_hnode_new();
    self->u32Size = 0;
    self->iter_depth = -1;
    self->last_error = ADT_NO_ERROR;
    self->iter_hkey = NULL;
}

void adt_hash_set(adt_hash_t *self, const char *pKey, void *pVal){
    if(self == NULL) return;
    if(pKey == NULL){
        self->last_error = ADT_INVALID_ARGUMENT_ERROR;
        return;
    }
    uint32_t u32HashVal = adt_hash_string(pKey);
    if(adt_hnode_set(self->root, pKey, pVal, u32HashVal, self->pDestructor, true)){
        self->u32Size++;
    }
    self->last_error = ADT_NO_ERROR;
}

bool adt_hash_insert(adt_hash_t *self, const char *pKey, void *pVal){
    if(self == NULL) return false;
    if(pKey == NULL){
        self->last_error = ADT_INVALID_ARGUMENT_ERROR;
        return false;
    }
    uint32_t u32HashVal = adt_hash_string(pKey);
    if(adt_hnode_set(self->root, pKey, pVal, u32HashVal, self->pDestructor, false)){
        self->u32Size++;
        self->last_error = ADT_NO_ERROR;
        return true;
    }
    self->last_error = ADT_ALREADY_EXISTS_ERROR;
    return false;
}

void** adt_hash_get(const adt_hash_t *self, const char *pKey){
    if(self && pKey){
        uint32_t u32HashVal = adt_hash_string(pKey);
        adt_hkey_t *hkey = adt_hnode_find(self->root, pKey, u32HashVal);
        if(hkey){
            return &hkey->val;
        }
    }
    return NULL;
}

void*  adt_hash_value(const adt_hash_t *self, const char *pKey){
   if(self && pKey){
      uint32_t u32HashVal = adt_hash_string(pKey);
      adt_hkey_t *hkey = adt_hnode_find(self->root, pKey, u32HashVal);
      if(hkey){
         return hkey->val;
      }
   }
   return NULL;
}

void*  adt_hash_remove(adt_hash_t *self, const char *pKey){
    if(self == NULL) return NULL;
    if(pKey == NULL){
        self->last_error = ADT_INVALID_ARGUMENT_ERROR;
        return NULL;
    }
    uint32_t u32HashVal = adt_hash_string(pKey);
    adt_hkey_t *hkey = adt_hnode_remove(self->root, pKey, u32HashVal);
    if(hkey){
        void *pVal = hkey->val;
        adt_hkey_delete(hkey, NULL);
        self->u32Size--;
        self->last_error = ADT_NO_ERROR;
        return pVal;
    }
    self->last_error = ADT_NOT_FOUND_ERROR;
    return NULL;
}

bool adt_hash_erase(adt_hash_t *self, const char *pKey){
    if(self == NULL) return false;
    if(pKey == NULL){
        self->last_error = ADT_INVALID_ARGUMENT_ERROR;
        return false;
    }
    uint32_t u32HashVal = adt_hash_string(pKey);
    adt_hkey_t *hkey = adt_hnode_remove(self->root, pKey, u32HashVal);
    if(hkey){
        adt_hkey_delete(hkey, self->pDestructor);
        self->u32Size--;
        self->last_error = ADT_NO_ERROR;
        return true;
    }
    self->last_error = ADT_NOT_FOUND_ERROR;
    return false;
}

int32_t adt_hash_length(const adt_hash_t *self){
    if(self){
        return self->u32Size;
    }
    return 0;
}

bool adt_hash_is_empty(const adt_hash_t *self){
    return (self == NULL) || (self->u32Size <= 0);
}

bool adt_hash_exists(const adt_hash_t *self, const char *pKey){
    if(self && pKey){
        uint32_t u32HashVal = adt_hash_string(pKey);
        adt_hkey_t *hkey = adt_hnode_find(self->root, pKey, u32HashVal);
        if(hkey){
            return true;
        }
    }
    return false;
}

void adt_hash_iter_init(adt_hash_t *self){
    if(self && self->root){
        self->iter_depth = 0;
        self->iter_frames[0].pNode = self->root;
        self->iter_frames[0].u8Cur = 0;
        self->iter_hkey = NULL;
    }
}

void** adt_hash_iter_next(adt_hash_t *self, const char **ppKey){
    if(!self || !ppKey) return NULL;

    while((self->iter_hkey == NULL) && (self->iter_depth >= 0)){
        adt_hit_frame_t *frame = &self->iter_frames[self->iter_depth];
        adt_hnode_t *pNode = frame->pNode;

        if(pNode->u8Width == 16){
            if(frame->u8Cur < 16){
                adt_hnode_t *pChild = &pNode->child.node[frame->u8Cur++];
                if(pChild->u8Cur > 0){
                    assert(self->iter_depth < ADT_HASH_MAX_DEPTH);
                    self->iter_depth++;
                    self->iter_frames[self->iter_depth].pNode = pChild;
                    self->iter_frames[self->iter_depth].u8Cur = 0;
                }
            }
            else{
                self->iter_depth--;
            }
        }
        else{
            if(frame->u8Cur < pNode->u8Cur){
                self->iter_hkey = pNode->child.match[frame->u8Cur++].key;
            }
            else{
                self->iter_depth--;
            }
        }
    }

    if(self->iter_hkey != NULL){
        void **ppVal;
        *ppKey = self->iter_hkey->key;
        ppVal = &self->iter_hkey->val;
        self->iter_hkey = self->iter_hkey->next;
        return ppVal;
    }

    *ppKey = NULL;
    return NULL;
}

void adt_hash_foreach(const adt_hash_t *self, void (*callback)(const char *key, void *val, void *arg), void *arg){
    if(self && self->root && callback){
        adt_hnode_foreach(self->root, callback, arg);
    }
}

int32_t adt_hash_keys(adt_hash_t *self, adt_ary_t *pArray){
    const char *pKey;
    int32_t s32i = 0;

    if(self == NULL) return -1;
    if((pArray == NULL) || adt_ary_destructor_is_enabled(pArray)){
        self->last_error = ADT_INVALID_ARGUMENT_ERROR;
        return -1;
    }

    adt_hash_iter_init(self);
    adt_ary_clear(pArray);
    adt_error_t result = adt_ary_extend(pArray, adt_hash_length(self));
    if(result != ADT_NO_ERROR){
        self->last_error = result;
        return -1;
    }
    do{
        (void) adt_hash_iter_next(self, &pKey);
        if(pKey != NULL){
            adt_ary_set(pArray, s32i++, (void*)pKey);
        }
    }while(pKey);

    self->last_error = ADT_NO_ERROR;
    return s32i;
}

int32_t adt_hash_values(adt_hash_t *self, adt_ary_t* pArray)
{
    const char *pKey;
    int32_t s32i = 0;

    if(self == NULL) return -1;
    if((pArray == NULL) || adt_ary_destructor_is_enabled(pArray)){
        self->last_error = ADT_INVALID_ARGUMENT_ERROR;
        return -1;
    }
    adt_hash_iter_init(self);
    adt_ary_clear(pArray);
    adt_error_t result = adt_ary_extend(pArray, adt_hash_length(self));
    if(result != ADT_NO_ERROR){
        self->last_error = result;
        return -1;
    }
    do{
        void **ppValue = adt_hash_iter_next(self, &pKey);
        if(ppValue != NULL){
            adt_ary_set(pArray, s32i++, *ppValue);
        }
    } while(pKey);

    self->last_error = ADT_NO_ERROR;
    return s32i;
}

adt_error_t adt_hash_get_last_error(const adt_hash_t *self)
{
    if (self != NULL)
    {
        return self->last_error;
    }
    return ADT_INVALID_ARGUMENT_ERROR;
}


//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

void adt_hnode_create(adt_hnode_t *node){
    assert(node);
    node->u8Width = 0;
    node->u8Cur = 0;
    node->u8Depth = 0;
    node->child.match = NULL;
}

adt_hnode_t *adt_hnode_new(void){
    adt_hnode_t *node = malloc(sizeof(adt_hnode_t));
    assert(node);
    adt_hnode_create(node);
    return node;
}

void adt_hnode_destroy(adt_hnode_t *node, void (*pDestructor)(void*)){
    if(node->u8Width < 16){
        int i;
        for(i = 0; i < node->u8Cur; i++){
            adt_hkey_t *hkey = node->child.match[i].key;
            while(hkey){
                adt_hkey_t *next = hkey->next;
                adt_hkey_delete(hkey, pDestructor);
                hkey = next;
            }
        }
        if(node->child.match != NULL){
            free(node->child.match);
            node->child.match = NULL;
        }
    }
    else{
      uint8_t i;
      assert(node->u8Width == 16);
        for(i = 0; i < 16; i++){
            adt_hnode_destroy(&node->child.node[i], pDestructor);
        }
        free(node->child.node);
        node->child.node = NULL;
    }
}

void adt_hnode_destroy_shallow(adt_hnode_t *node){
    if(node->u8Width < 16){
        if(node->child.match != NULL){
            free(node->child.match);
            node->child.match = NULL;
        }
    }
    else{
      uint8_t i;
        assert(node->u8Width == 16);
        for(i = 0; i < 16; i++){
            adt_hnode_destroy_shallow(&node->child.node[i]);
        }
        free(node->child.node);
        node->child.node = NULL;
    }
}

void adt_hnode_delete(adt_hnode_t *node, void (*pDestructor)(void*)){
    if(!node) return;
    adt_hnode_destroy(node, pDestructor);
    free(node);
}

void adt_hnode_insert(adt_hnode_t *node, adt_hkey_t *key, uint32_t u32Hash){
    uint8_t i;
    assert(node);
    assert(key);
    if(node->u8Width == 16){
        uint32_t u32Bits = ((uint32_t) node->u8Depth) * 4;
        uint8_t u8Bucket = (uint8_t) ((u32Hash >> u32Bits) & 0xF);
        adt_hnode_insert(&node->child.node[u8Bucket], key, u32Hash);
    }
    else{
        for(i = 0; i < node->u8Cur; i++){
            if(node->child.match[i].u32Hash == u32Hash){
                adt_hkey_t *hkey = node->child.match[i].key;
                assert(hkey != NULL);
                while(hkey->next){
                    hkey = hkey->next;
                }
                hkey->next = key;
                return;
            }
        }
        if(node->u8Cur < node->u8Width){
            node->child.match[node->u8Cur].key = key;
            node->child.match[node->u8Cur++].u32Hash = u32Hash;
        }
        else{
            if(node->u8Width == 0){
                node->u8Width = 1;
                node->child.match = (adt_hmatch_t*) malloc(sizeof(adt_hmatch_t) * node->u8Width);
                assert(node->child.match);
                node->child.match[0].key = key;
                node->child.match[0].u32Hash = u32Hash;
                node->u8Cur = 1;
                return;
            }
            adt_hmatch_t* old = node->child.match;
            node->u8Width *= 2;
            if(node->u8Width < 16){
                node->child.match = (adt_hmatch_t*) malloc(sizeof(adt_hmatch_t) * node->u8Width);
                assert(node->child.match);
                for(i = 0; i < node->u8Cur; i++){
                    node->child.match[i] = old[i];
                }
                free(old);
                node->child.match[node->u8Cur].key = key;
                node->child.match[node->u8Cur++].u32Hash = u32Hash;
            }
            else{
            uint32_t u32Bits;
            uint8_t u8Bucket;
                assert(node->u8Width == 16);
                node->child.node = (adt_hnode_t*) malloc(sizeof(adt_hnode_t) * 16);
                assert(node->child.node);
                for(i = 0; i < 16; i++){
                    adt_hnode_create(&node->child.node[i]);
                    node->child.node[i].u8Depth = node->u8Depth + 1;
                }
                assert(node->u8Depth <= 8);
            u32Bits = ((uint32_t) node->u8Depth) * 4;
                for(i = 0; i < node->u8Cur; i++){
                    u8Bucket = (uint8_t) ((old[i].u32Hash >> u32Bits) & 0xF);
                    adt_hnode_insert(&node->child.node[u8Bucket], old[i].key, old[i].u32Hash);
                }
                free(old);
            u8Bucket = (uint8_t) ((u32Hash >> u32Bits) & 0xF);
                adt_hnode_insert(&node->child.node[u8Bucket], key, u32Hash);
            }
        }
    }
}

static bool adt_hnode_set(adt_hnode_t *node, const char *key, void *val, uint32_t u32Hash, void (*pDestructor)(void*), bool overwrite){
    uint8_t i;
    assert(node);
    assert(key);
    if(node->u8Width == 16){
        uint32_t u32Bits = ((uint32_t) node->u8Depth) * 4;
        uint8_t u8Bucket = (uint8_t) ((u32Hash >> u32Bits) & 0xF);
        return adt_hnode_set(&node->child.node[u8Bucket], key, val, u32Hash, pDestructor, overwrite);
    }
    else{
        for(i = 0; i < node->u8Cur; i++){
            if(node->child.match[i].u32Hash == u32Hash){
                adt_hkey_t *hkey = node->child.match[i].key;
                adt_hkey_t *hprev = NULL;
                while(hkey){
                    if(strcmp(hkey->key, key) == 0){
                        if(overwrite){
                            if(pDestructor && hkey->val){
                                pDestructor(hkey->val);
                            }
                            hkey->val = val;
                        }
                        return false; // not inserted as new entry
                    }
                    hprev = hkey;
                    hkey = hkey->next;
                }
                // Hash matched, but key string is different (collision)
                hkey = adt_hkey_new(key, val);
                assert(hkey != NULL);
                assert(hprev != NULL);
                hprev->next = hkey;
                return true; // inserted new
            }
        }

        adt_hkey_t *new_key = adt_hkey_new(key, val);
        if(new_key == NULL) return false;

        if(node->u8Cur < node->u8Width){
            node->child.match[node->u8Cur].key = new_key;
            node->child.match[node->u8Cur++].u32Hash = u32Hash;
            return true;
        }
        else{
            if(node->u8Width == 0){
                node->u8Width = 1;
                node->child.match = (adt_hmatch_t*) malloc(sizeof(adt_hmatch_t) * node->u8Width);
                assert(node->child.match);
                node->child.match[0].key = new_key;
                node->child.match[0].u32Hash = u32Hash;
                node->u8Cur = 1;
                return true;
            }
            adt_hmatch_t* old = node->child.match;
            node->u8Width *= 2;
            if(node->u8Width < 16){
                node->child.match = (adt_hmatch_t*) malloc(sizeof(adt_hmatch_t) * node->u8Width);
                assert(node->child.match);
                for(i = 0; i < node->u8Cur; i++){
                    node->child.match[i] = old[i];
                }
                free(old);
                node->child.match[node->u8Cur].key = new_key;
                node->child.match[node->u8Cur++].u32Hash = u32Hash;
                return true;
            }
            else{
                uint32_t u32Bits;
                uint8_t u8Bucket;
                assert(node->u8Width == 16);
                node->child.node = (adt_hnode_t*) malloc(sizeof(adt_hnode_t) * 16);
                assert(node->child.node);
                for(i = 0; i < 16; i++){
                    adt_hnode_create(&node->child.node[i]);
                    node->child.node[i].u8Depth = node->u8Depth + 1;
                }
                assert(node->u8Depth <= 8);
                u32Bits = ((uint32_t) node->u8Depth) * 4;
                for(i = 0; i < node->u8Cur; i++){
                    u8Bucket = (uint8_t) ((old[i].u32Hash >> u32Bits) & 0xF);
                    adt_hnode_insert(&node->child.node[u8Bucket], old[i].key, old[i].u32Hash);
                }
                free(old);
                u8Bucket = (uint8_t) ((u32Hash >> u32Bits) & 0xF);
                adt_hnode_insert(&node->child.node[u8Bucket], new_key, u32Hash);
                return true;
            }
        }
    }
}

adt_hkey_t * adt_hnode_find(const adt_hnode_t *node, const char *key, uint32_t u32Hash){
    if(node == NULL) return NULL;
    if(node->u8Width == 16){
        uint8_t u8Bucket = (uint8_t) ((u32Hash >> (((uint32_t) node->u8Depth) * 4)) & 0xF);
        return adt_hnode_find(&node->child.node[u8Bucket], key, u32Hash);
    }
    else{
        int i;
        for(i = 0; i < node->u8Cur; i++){
            if(node->child.match[i].u32Hash == u32Hash){
                adt_hkey_t *hkey = node->child.match[i].key;
                while((hkey) && (strcmp(hkey->key, key) != 0)){
                    hkey = hkey->next;
                }
                return hkey;
            }
        }
    }
    return NULL;
}

adt_hkey_t * adt_hnode_remove(adt_hnode_t *node, const char *key, uint32_t u32Hash){
    adt_hnode_t *parent = NULL;
    adt_hkey_t *hkey = NULL;
    while(node){
      uint8_t u8Bucket;
        if(node->u8Width == 16){
            parent = node;
         u8Bucket = (uint8_t) ((u32Hash >> (((uint32_t) node->u8Depth) * 4)) & 0xF);
            node = &node->child.node[u8Bucket];
        }
        else{
            int i, j;
            adt_hkey_t *hprev = NULL;
            for(i = 0; i < node->u8Cur; i++){
                if(node->child.match[i].u32Hash == u32Hash){
                    //hash found
                    hkey = node->child.match[i].key;
                    while((hkey) && (strcmp(hkey->key, key) != 0)){
                        hprev = hkey;
                        hkey = hkey->next;
                    }
                    break;
                }
            }
            if(hkey){
                //key found
                if(hprev){
                    //remove from linked list
                    hprev->next = hkey->next;
                    hkey->next = NULL;
                }
                else if(hkey->next){
                    //remove from linked list
                    node->child.match[i].key = hkey->next;
                }
                else{
                    //remove from node->child.match array
                    for(j = i + 1; j < node->u8Cur; j++){
                        node->child.match[j - 1] = node->child.match[j];
                    }
                    node->u8Cur--;
                    node->child.match[node->u8Cur].u32Hash = 0;
                    node->child.match[node->u8Cur].key = NULL;

                    //compact self node
                    if(node->u8Cur == 0){
                        if(node->child.match != NULL){
                            free(node->child.match);
                            node->child.match = NULL;
                        }
                        node->u8Width = 0;
                    }
                    else if((node->u8Width > 1) && (node->u8Cur <= node->u8Width / 2)){
                        adt_hmatch_t* old = node->child.match;
                        node->u8Width /= 2;
                        node->child.match = (adt_hmatch_t*) malloc(sizeof(adt_hmatch_t) * node->u8Width);
                        assert(node->child.match);
                        for(j = 0; j < node->u8Cur; j++){
                            node->child.match[j] = old[j];
                        }
                        free(old);
                    }

                    //compact parent node
                    if(parent){
                        uint8_t u8Count = 0;
                        assert(parent->u8Width == 16);
                        for(j = 0; j < 16; j++){
                            if(parent->child.node[j].u8Width > 8){
                                //if a child has width > 8 then there is nothing more to do
                                return hkey;
                            }
                            assert(parent->child.node[j].u8Cur <= 8);
                            u8Count += parent->child.node[j].u8Cur;
                        }
                        if(u8Count <= 8){
                            int k;
                            //reduce node from width 16 to width 8
                            adt_hnode_t *old = parent->child.node;
                            parent->u8Width /= 2;
                            parent->u8Cur = 0;
                            parent->child.match = (adt_hmatch_t*) malloc(sizeof(adt_hmatch_t) * parent->u8Width);
                            assert(parent->child.match);
                            for(j = 0; j < 16; j++){
                                node = &old[j];
                                for(k = 0; k < node->u8Cur; k++){
                                    parent->child.match[parent->u8Cur++] = node->child.match[k];
                                    assert(parent->u8Cur <= parent->u8Width);
                                }
                                adt_hnode_destroy_shallow(node);
                            }
                            free(old);
                        }
                    }
                }
            }
            return hkey;
        }
    }
    return NULL;
}

static void adt_hnode_foreach(const adt_hnode_t *node, void (*callback)(const char *key, void *val, void *arg), void *arg){
    if(node == NULL) return;
    if(node->u8Width == 16){
        for(uint8_t i = 0; i < 16; i++){
            if(node->child.node[i].u8Cur > 0 || node->child.node[i].u8Width == 16){
                adt_hnode_foreach(&node->child.node[i], callback, arg);
            }
        }
    }
    else{
        for(uint8_t i = 0; i < node->u8Cur; i++){
            adt_hkey_t *hkey = node->child.match[i].key;
            while(hkey){
                callback(hkey->key, hkey->val, arg);
                hkey = hkey->next;
            }
        }
    }
}

void adt_hkey_create(adt_hkey_t *hkey, const char *key, void *value){
    if(hkey){
        hkey->key = STRDUP(key);
        hkey->val = value;
        hkey->next = NULL;
    }
}

void adt_hkey_destroy(adt_hkey_t *hkey, void (*pDestructor)(void*)){
    if(hkey){
        if(hkey->key) free(hkey->key);
        if((hkey->val) && (pDestructor)) pDestructor(hkey->val);
    }
}

adt_hkey_t *adt_hkey_new(const char *key, void *value){
    adt_hkey_t * hkey = (adt_hkey_t *) malloc(sizeof(adt_hkey_t));
    if(hkey){
        adt_hkey_create(hkey, key, value);
    }
    return hkey;
}

void adt_hkey_delete(adt_hkey_t *hkey, void (*pDestructor)(void*)){
    if(hkey){
        adt_hkey_destroy(hkey, pDestructor);
        free(hkey);
    }
}

uint32_t adt_hash_string(const char * s)
{
    uint32_t hash = 0;

    for(; *s; ++s)
    {
        hash += (uint8_t)*s;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}
