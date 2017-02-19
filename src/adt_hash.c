/*****************************************************************************
* \file:   		adt_hash.c
* \author:		Conny Gustafsson
* \date:		2013-06-08
* \brief:		hash data structure
*
* Copyright (c) 2013 Conny Gustafsson
*
******************************************************************************/
#include "adt_hash.h"
#include "adt_str.h"
#include <malloc.h>
#include <assert.h>
#include <string.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

#ifdef _MSC_VER
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

/**************** Private Function Declarations *******************/
static void adt_hnode_create(adt_hnode_t *node);
static void adt_hnode_destroy(adt_hnode_t *node,void (*pDestructor)(void*));
static void adt_hnode_destroy_shallow(adt_hnode_t *node);
static adt_hnode_t *adt_hnode_new(void);
static void adt_hnode_delete(adt_hnode_t *node,void (*pDestructor)(void*));
static void adt_hnode_insert(adt_hnode_t *node, adt_hkey_t *key, uint32_t u32Hash);
static adt_hkey_t * adt_hnode_find(adt_hnode_t *node, const char *key,uint32_t u32Hash);
static adt_hkey_t * adt_hnode_remove(adt_hnode_t *node, const char *key,uint32_t u32Hash,void (*pDestructor)(void*));
static void adt_hkey_create(adt_hkey_t *hkey, const char *key, void *value);
static void adt_hkey_destroy(adt_hkey_t *hkey, void (*pDestructor)(void*));
static adt_hkey_t *adt_hkey_new(const char *key, void *value);
static void adt_hkey_delete(adt_hkey_t *hkey, void (*pDestructor)(void*));
static uint32_t adt_hash_string(const char * s);
adt_hit_stored_t *adt_hit_stored_new(adt_hnode_t *pNode,uint8_t u8Cur);
static void adt_hit_stored_delete(adt_hit_stored_t *pIter);
static void adt_hit_stored_delete_void(void *pIter);

/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/

adt_hash_t*	adt_hash_new(void (*pDestructor)(void*)){
	adt_hash_t *self = (adt_hash_t*) malloc(sizeof(adt_hash_t));
	if(self){
		adt_hash_create(self,pDestructor);
	}
	return self;
}

void adt_hash_delete(adt_hash_t *self){
	if(self){
		adt_hash_destroy(self);
		free(self);
	}
}

void adt_hash_create(adt_hash_t *self,void (*pDestructor)(void*)){
	self->root = adt_hnode_new();
	self->u32Size = 0;
	self->pDestructor = pDestructor;
	adt_stack_create(&self->iter_stack,adt_hit_stored_delete_void);
	self->iter.pHkey = 0;
	self->iter.pNode = 0;
	self->iter.u8Cur = 0;
}

void adt_hash_destroy(adt_hash_t *self){
	if(self == 0) return;
	if(self->root != 0){
		adt_hnode_delete(self->root,self->pDestructor);
		self->root = 0;
	}
	adt_stack_destroy(&self->iter_stack);
	self->u32Size = 0;
}

void adt_hash_set(adt_hash_t *self, const char *pKey, uint32_t u32KeyLen, void *pVal){
	if(self && pKey){
		uint32_t u32HashVal = adt_hash_string(pKey);
		adt_hkey_t *hkey = adt_hnode_find(self->root,pKey,u32HashVal);
		if(hkey){
			//already in hash table
			if(self->pDestructor && hkey->val){
				self->pDestructor(hkey->val);
			}
			hkey->val = pVal;
		}
		else{
			//not found
			hkey = adt_hkey_new(pKey,pVal);
			if(hkey){
				adt_hnode_insert(self->root,hkey,u32HashVal);
				self->u32Size++;
			}
		}
	}
}

void**	adt_hash_get(adt_hash_t *self, const char *pKey, uint32_t u32KeyLen){
	if(self && pKey){
		uint32_t u32HashVal = adt_hash_string(pKey);
		adt_hkey_t *hkey = adt_hnode_find(self->root,pKey,u32HashVal);
		if(hkey){
			return &hkey->val;
		}
	}
	return (void**)0;
}

void**adt_hash_remove(adt_hash_t *self, const char *pKey, uint32_t u32KeyLen){
	if(self && pKey){
		uint32_t u32HashVal = adt_hash_string(pKey);
		adt_hkey_t *hkey = adt_hnode_remove(self->root,pKey,u32HashVal,0);
		if(hkey){
			void **pVal = &hkey->val;
			adt_hkey_delete(hkey,0);
			self->u32Size--;
			return pVal;
		}
	}
	return (void**) 0;
}

uint32_t adt_hash_size(adt_hash_t *self){
	if(self){
		return self->u32Size;
	}
	return 0;
}
bool adt_hash_exists(adt_hash_t *self, const char *pKey, uint32_t u32KeyLen){
	if(self){
		uint32_t u32HashVal = adt_hash_string(pKey);
		adt_hkey_t *hkey = adt_hnode_find(self->root,pKey,u32HashVal);
		if(hkey){
			return true;
		}
	}
	return false;
}

void	adt_hash_iter_init(adt_hash_t *self){
	if(self){
		self->iter.pNode = self->root;
		self->iter.u8Cur = 0;
		self->iter.pHkey = 0;
		adt_stack_clear(&self->iter_stack);
	}
}

void** adt_hash_iter_next(adt_hash_t *self,const char **ppKey,uint32_t *pKeyLen){
	if(!self || !ppKey || !pKeyLen) return (void*) 0;

	if(self->iter.pHkey == 0){
		//find next hkey
		adt_hnode_t *pNode;
BEGIN:
		pNode =  self->iter.pNode;
		if(pNode->u8Width == 16){
			while(self->iter.u8Cur<16){
				adt_hnode_t *pChild = &pNode->child.node[self->iter.u8Cur++];
				if(pChild->u8Cur>0){
					adt_stack_push(&self->iter_stack,adt_hit_stored_new(pNode,self->iter.u8Cur));
					self->iter.pNode = pChild;
					self->iter.u8Cur = 0;
					goto BEGIN;
				}
			}
			if(self->iter.u8Cur==16){
				//check stack
				adt_hit_stored_t *pStored = (adt_hit_stored_t*) adt_stack_pop(&self->iter_stack);
				if(pStored){
					//restore self->iter.Pnode and continue
					self->iter.pNode = pStored->pNode;
					self->iter.u8Cur = pStored->u8Cur;
					adt_hit_stored_delete(pStored);
					goto BEGIN;
				}
			}
		}
		else{
			if(self->iter.u8Cur >= pNode->u8Cur){
				adt_hit_stored_t *pStored = (adt_hit_stored_t*) adt_stack_pop(&self->iter_stack);
				if(pStored){
					//restore self->iter.Pnode and continue
					self->iter.pNode = pStored->pNode;
					self->iter.u8Cur = pStored->u8Cur;
					adt_hit_stored_delete(pStored);
					goto BEGIN;
				}
			}
			else{
				self->iter.pHkey = pNode->child.match[self->iter.u8Cur++].key;
			}
		}
	}

	if(self->iter.pHkey){
      void **ppVal;
      *ppKey = self->iter.pHkey->key;
		*pKeyLen = strlen(self->iter.pHkey->key);
		ppVal = &self->iter.pHkey->val;
		self->iter.pHkey = self->iter.pHkey->next;
		return ppVal;
	}
	else{
		*ppKey=0;
		*pKeyLen=0;
		return (void**) 0; //done
	}
}

uint32_t adt_hash_keys(adt_hash_t *self,adt_ary_t *pArray){
	const char *pKey;
	uint32_t u32KeyLen;
	int32_t s32i=0;

	if(!self) return 0;

	if(pArray){
		adt_hash_iter_init(self);
		adt_ary_clear(pArray);
		adt_ary_extend(pArray,adt_hash_size(self));
		do{
			adt_hash_iter_next(self,&pKey,&u32KeyLen);
			if(pKey){
				adt_ary_set(pArray,s32i++, STRDUP(pKey));
			}
		}while(pKey);
	}
	return (uint32_t) s32i;
}


/***************** Private Function Definitions *******************/



void adt_hnode_create(adt_hnode_t *node){
	assert(node);
	node->u8Width = 1;
	node->u8Cur = 0;
	node->u8Depth = 0;
	node->child.match = (adt_hmatch_t*) malloc(sizeof(adt_hmatch_t)*node->u8Width);
	assert(node->child.match);
}

adt_hnode_t *adt_hnode_new(void){
	adt_hnode_t *node = malloc(sizeof(adt_hnode_t));
	assert(node);
	adt_hnode_create(node);
	return node;
}

void adt_hnode_destroy(adt_hnode_t *node,void (*pDestructor)(void*)){
	if(node->u8Width<16){
		int i;
		for(i=0;i<node->u8Cur;i++){
			adt_hkey_t *hkey = node->child.match[i].key;
			while( hkey ){
				adt_hkey_t *next = hkey->next;
				adt_hkey_delete(hkey,pDestructor);
				hkey = next;
			}
		}
		free(node->child.match);
	}
	else{
      uint8_t i; 
      assert(node->u8Width==16);		
		for(i=0;i<16;i++){
			adt_hnode_destroy(&node->child.node[i],pDestructor);
		}
		free(node->child.node);
	}
}

void adt_hnode_destroy_shallow(adt_hnode_t *node){
	if(node->u8Width<16){
		free(node->child.match);
	}
	else{
      uint8_t i;
		assert(node->u8Width==16);		
		for(i=0;i<16;i++){
			adt_hnode_destroy_shallow(&node->child.node[i]);
		}
		free(node->child.node);
	}
}

void adt_hnode_delete(adt_hnode_t *node,void (*pDestructor)(void*)){
	if(!node) return;
	adt_hnode_destroy(node,pDestructor);
	free(node);
}


void adt_hnode_insert(adt_hnode_t *node, adt_hkey_t *key, uint32_t u32Hash){
	uint8_t i;
	assert(node);
	assert(key);
	if(node->u8Width == 16){
		uint32_t u32Bits = (node->u8Depth)*4;
		uint8_t u8Bucket = (uint8_t) ((u32Hash >> u32Bits) & 0xF);
		adt_hnode_insert(&node->child.node[u8Bucket],key,u32Hash);
	}
	else{
		for(i=0;i<node->u8Cur;i++){
			if(node->child.match[i].u32Hash == u32Hash){
				adt_hkey_t *hkey = node->child.match[i].key;
				while( (hkey) && (hkey->next) ){
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
			adt_hmatch_t* old = node->child.match;
			node->u8Width *= 2;
			if(node->u8Width<16){
				node->child.match = (adt_hmatch_t*) malloc(sizeof(adt_hmatch_t)*node->u8Width);
				assert(node->child.match);
				for(i=0;i<node->u8Cur;i++){
					node->child.match[i]=old[i];
				}
				free(old);
				node->child.match[node->u8Cur].key = key;
				node->child.match[node->u8Cur++].u32Hash = u32Hash;
			}
			else{
				adt_hmatch_t* old = node->child.match;
            uint32_t u32Bits;
            uint8_t u8Bucket;
				assert(node->u8Width==16);
				node->child.node = (adt_hnode_t*) malloc(sizeof(adt_hnode_t)*16);
				assert(node->child.node);
				for(i=0;i<16;i++){
					adt_hnode_create(&node->child.node[i]);
					node->child.node[i].u8Depth = node->u8Depth+1;
				}
				assert(node->u8Depth<=8);
            u32Bits = (node->u8Depth)*4;
				for(i=0;i<node->u8Cur;i++){
					uint8_t u8Bucket = (uint8_t) ( (old[i].u32Hash >> u32Bits) & 0xF);
					adt_hnode_insert(&node->child.node[u8Bucket],old[i].key,old[i].u32Hash);
				}
				free(old);
            u8Bucket = (uint8_t) ((u32Hash >> u32Bits) & 0xF);
				adt_hnode_insert(&node->child.node[u8Bucket],key,u32Hash);
			}
		}
	}
}

adt_hkey_t * adt_hnode_find(adt_hnode_t *node, const char *key,uint32_t u32Hash){
	if(node->u8Width == 16){
			uint8_t u8Bucket = (uint8_t) ( (u32Hash >> (node->u8Depth*4)) & 0xF);
			return adt_hnode_find(&node->child.node[u8Bucket],key,u32Hash);
	}
	else{
		int i;
		for(i=0;i<node->u8Cur;i++){
			if(node->child.match[i].u32Hash == u32Hash){
				adt_hkey_t *hkey = node->child.match[i].key;
				while( (hkey) && (strcmp(hkey->key,key)!=0) ){
					hkey = hkey->next;
				}
				return hkey;
			}
		}
	}
	return 0;
}

adt_hkey_t * adt_hnode_remove(adt_hnode_t *node, const char *key,uint32_t u32Hash, void (*pDestructor)(void*)){
	adt_hnode_t *parent = 0;
	adt_hkey_t *hkey = 0;
	while(node){
      uint8_t u8Bucket;
		if(node->u8Width == 16){
			parent = node;
         u8Bucket = (uint8_t) ( (u32Hash >> (node->u8Depth*4)) & 0xF);
			node = &node->child.node[u8Bucket];
		}
		else{
			int i,j;
			adt_hkey_t *hprev = 0;
			for(i=0;i<node->u8Cur;i++){
				if(node->child.match[i].u32Hash == u32Hash){
					//hash found
					hkey = node->child.match[i].key;
					while( (hkey) && (strcmp(hkey->key,key)!=0) ){
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
					hkey->next = 0;
				}
				else if(hkey->next){
					//remove from linked list
					node->child.match[i].key = hkey->next;
				}
				else{
					//remove from node->child.match array
					for(j=i+1;j<node->u8Cur;j++){
						node->child.match[j-1] = node->child.match[j];
					}
					node->u8Cur--;
					node->child.match[node->u8Cur].u32Hash = 0;
					node->child.match[node->u8Cur].key = 0;


					//compact self node
					if( (node->u8Width > 1) && (node->u8Cur<=node->u8Width/2) ){
						adt_hmatch_t* old = node->child.match;
						node->u8Width /= 2;
						node->child.match = (adt_hmatch_t*) malloc(sizeof(adt_hmatch_t)*node->u8Width);
						assert(node->child.match);
						for(j=0;j<node->u8Cur;j++){
							node->child.match[j]=old[j];
						}
						free(old);
					}

					//compact parent node
					if(parent){
						uint8_t u8Count = 0;
						assert(parent->u8Width == 16);
						for(j=0;j<16;j++){
							if(parent->child.node[j].u8Width>8){
								//if a child has width > 8 then there is nothing more to do
								return hkey;
							}
							assert(parent->child.node[j].u8Cur<=8);
							u8Count+=parent->child.node[j].u8Cur;
						}
						if(u8Count<=8){
							int k;
							//reduce node from width 16 to width 8
							adt_hnode_t *old = parent->child.node;
							parent->u8Width /= 2;
							parent->u8Cur = 0;
							parent->child.match = (adt_hmatch_t*) malloc(sizeof(adt_hmatch_t)*parent->u8Width);
							assert(parent->child.match);
							for(j=0;j<16;j++){
								node = &old[j];
								for(k=0;k<node->u8Cur;k++){
									parent->child.match[parent->u8Cur++] = node->child.match[k];
									assert(parent->u8Cur<=parent->u8Width);
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
	return 0;
}

void adt_hkey_create(adt_hkey_t *hkey, const char *key, void *value){
	if(hkey){
		hkey->key = STRDUP(key);
		hkey->val = value;
		hkey->next = 0;
	}
}

void adt_hkey_destroy(adt_hkey_t *hkey, void (*pDestructor)(void*)){
	if(hkey){
		if(hkey->key) free(hkey->key);
		if( (hkey->val) && (pDestructor)) pDestructor(hkey->val);
	}
}

adt_hkey_t *adt_hkey_new(const char *key, void *value){
	adt_hkey_t * hkey = (adt_hkey_t *) malloc(sizeof(adt_hkey_t));
	if(hkey){
		adt_hkey_create(hkey,key,value);
	}
	return hkey;
}

void adt_hkey_delete(adt_hkey_t *hkey, void (*pDestructor)(void*)){
	if(hkey){
		adt_hkey_destroy(hkey,pDestructor);
		free(hkey);
	}
}

adt_hit_stored_t *adt_hit_stored_new(adt_hnode_t *pNode,uint8_t u8Cur){
	adt_hit_stored_t *pIter;
	if((pIter = (adt_hit_stored_t*) malloc(sizeof(adt_hit_stored_t))) == 0){
			return (adt_hit_stored_t*)0;
	}
	pIter->pNode = pNode;
	pIter->u8Cur = u8Cur;
	return pIter;
}

void adt_hit_stored_delete(adt_hit_stored_t *pIter){
	if(pIter){
		free(pIter);
	}
}

void adt_hit_stored_delete_void(void *pIter){
	adt_hit_stored_delete((adt_hit_stored_t*) pIter);
}



uint32_t adt_hash_string(const char * s)
{
    uint32_t hash = 0;

    for(; *s; ++s)
    {
    	hash += *s;
    	hash += (hash << 10);
    	hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}



