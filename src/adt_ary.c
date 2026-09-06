/*****************************************************************************
* \file      adt_ary.c
* \author    Conny Gustafsson
* \date      2013-06-03
* \brief     array data structure
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_ary.h"
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define DATA_BLOCK_MAX 65536  //maximum amount of bytes that can be copied in memmmove is implementation specific,
                              //use define to control how many bytes shall be copied

#define ELEM_SIZE (sizeof(void*))
#define ELEM_VALUE_IS_LESS(T) ( *((T*) a) < *((T*) b) )
#define ADT_ARY_MIN_CAPACITY 4

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void adt_block_memmove(uint8_t*pDest, uint8_t*pSrc, uint32_t u32Remain);
static adt_error_t adt_ary_introsort(void **elems, int32_t low, int32_t high, int32_t depth_limit, adt_vlt_func_t *key, bool reverse);
static adt_error_t adt_ary_insertion_sort_range(void **elems, int32_t start, int32_t end, adt_vlt_func_t *key, bool reverse);
static adt_error_t adt_ary_heapsort_range(void **elems, int32_t start, int32_t end, adt_vlt_func_t *key, bool reverse);
static adt_error_t adt_ary_sift_down(void **elems, int32_t start, int32_t count, int32_t root, adt_vlt_func_t *key, bool reverse);
static adt_error_t adt_ary_partition(void **elems, int32_t low, int32_t high, int32_t *pivot_idx, adt_vlt_func_t *key, bool reverse);
static adt_error_t adt_ary_median_of_three(void **elems, int32_t a, int32_t b, int32_t c, adt_vlt_func_t *key, bool reverse);
static void adt_ary_swap(void **a, void **b);
static bool adt_ary_elem_less(const void *a, const void *b, adt_vlt_func_t *key, bool reverse, adt_error_t *err);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
adt_ary_t* adt_ary_new(void (*pDestructor)(void*)){
   adt_ary_t *self;
   if((self = malloc(sizeof(adt_ary_t)))==NULL){
      return NULL;
   }
   adt_ary_create(self,pDestructor);
   return self;
}
adt_ary_t* adt_ary_make(void** ppElem, int32_t s32Len,void (*pDestructor)(void*)){
   adt_ary_t *self;
   int32_t s32i;
   void **ppDest,**ppSrc;

   if(s32Len>= INT32_MAX){
      //array length too long
      return NULL;
   }

   if(ppElem == NULL){
      return NULL;
   }

   self = adt_ary_new(pDestructor);
   if(self==NULL){
      return NULL;
   }
   adt_ary_extend(self,s32Len);
   ppDest=self->pFirst;
   assert(ppDest != NULL);
   ppSrc=ppElem;
   for(s32i=0;s32i<s32Len;s32i++){
      *(ppDest++) = *(ppSrc++);
   }
   return self;
}
void adt_ary_delete(adt_ary_t *self){
   if(self != NULL){
      adt_ary_destroy(self);
      free(self);
   }
}

void  adt_ary_vdelete(void *arg)
{
   adt_ary_delete((adt_ary_t*) arg);
}

void adt_ary_destructor_enable(adt_ary_t *self, bool enable){
   if(self != NULL){
      self->destructorEnable = enable;
   }
}

bool adt_ary_has_destructor(const adt_ary_t *self){
   if(self != NULL){
      return (self->pDestructor != NULL);
   }
   return false;
}

bool adt_ary_destructor_is_enabled(const adt_ary_t *self){
   if(self != NULL){
      return ((self->pDestructor != NULL) && (self->destructorEnable != false));
   }
   return false;
}

//Accessors
void** adt_ary_set(adt_ary_t *self, int32_t s32Index, void *pElem){
   if(self == NULL){
      return NULL;
   }
   if(s32Index<0){
      s32Index = (-s32Index);
      if(s32Index > (self->s32CurLen) ){
         //negative index outside array bounds
         return NULL;
      }
      //negative index inside array bounds
      s32Index=self->s32CurLen-s32Index;
   }
   adt_ary_fill(self,((int32_t) s32Index)+1);
   self->pFirst[s32Index]=pElem;
   return &self->pFirst[s32Index];
}

/**
 * This returns pointer to pointer, make sure to dereference the result.
 * It returns NULL in case a negative index is out of range
 */
void** adt_ary_get(adt_ary_t *self, int32_t s32Index){
   if(self == NULL){
      return NULL;
   }
   if(s32Index<0){
      s32Index = (-s32Index);
      if(s32Index > (self->s32CurLen) ){
         //negative index outside array bounds
         return NULL;
      }
      //negative index inside array bounds
      s32Index=self->s32CurLen-s32Index;
   }
   adt_ary_fill(self,(int32_t) (s32Index+1));
   return &self->pFirst[s32Index];
}

/**
 * If you are certain that the index is going to be within bounds, you can use
 * this convenience function for getting the value without additional pointer dereference.
 */
void* adt_ary_value(const adt_ary_t *self, int32_t s32Index)
{
   if(self == NULL){
      return NULL;
   }
   if(s32Index<0){
      //negative index inside array bounds ensured by caller
      s32Index=self->s32CurLen+s32Index;
   }
   return self->pFirst[s32Index];
}

/**
 * Removes the first occurrence of pElem from the array
 */
adt_error_t adt_ary_remove(adt_ary_t *self, void *pElem)
{
   if (self != NULL)
   {
      int32_t s32Index;
      adt_error_t result = ADT_NO_ERROR;
      for(s32Index = 0; s32Index < self->s32CurLen; s32Index++ )
      {
         if (self->pFirst[s32Index] == pElem){
            result = adt_ary_splice(self, s32Index, 1);
            break;
         }
      }
      return result;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

static int32_t adt_ary_next_capacity(int32_t current_capacity, int32_t min_needed)
{
   int32_t new_capacity = (current_capacity < ADT_ARY_MIN_CAPACITY) ? ADT_ARY_MIN_CAPACITY : current_capacity;
   while (new_capacity < min_needed)
   {
      if (new_capacity > (INT32_MAX / 2))
      {
         new_capacity = INT32_MAX;
         break;
      }
      new_capacity *= 2;
   }
   return new_capacity;
}

adt_error_t adt_ary_reserve(adt_ary_t *self, int32_t s32Len)
{
   if ((self == NULL) || (s32Len < 0))
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (s32Len <= self->s32AllocLen)
   {
      return ADT_NO_ERROR;
   }
   if (s32Len >= INT32_MAX)
   {
      return ADT_LENGTH_ERROR;
   }

   void **ppAlloc = (void**) malloc(sizeof(void*) * (size_t)s32Len);
   if (ppAlloc == NULL)
   {
      return ADT_MEM_ERROR;
   }

   if (self->s32CurLen > 0)
   {
      memcpy(ppAlloc, self->pFirst, sizeof(void*) * (size_t)self->s32CurLen);
   }

   if (self->ppAlloc != NULL)
   {
      free(self->ppAlloc);
   }

   self->ppAlloc = ppAlloc;
   self->pFirst = ppAlloc;
   self->s32AllocLen = s32Len;
   return ADT_NO_ERROR;
}

/**
 * Appends pElem to the end of the array
 */
adt_error_t adt_ary_push(adt_ary_t *self, void *pElem){
   if (self != NULL) {
      if (self->s32CurLen == INT32_MAX) {
         return ADT_LENGTH_ERROR;
      }

      int32_t offset = (int32_t)(self->pFirst - self->ppAlloc);
      if (offset + self->s32CurLen < self->s32AllocLen) {
         self->pFirst[self->s32CurLen++] = pElem;
         return ADT_NO_ERROR;
      }

      if (self->s32CurLen < self->s32AllocLen) {
         memmove(self->ppAlloc, self->pFirst, sizeof(void*) * (size_t)self->s32CurLen);
         self->pFirst = self->ppAlloc;
         self->pFirst[self->s32CurLen++] = pElem;
         return ADT_NO_ERROR;
      }

      int32_t new_capacity = adt_ary_next_capacity(self->s32AllocLen, self->s32CurLen + 1);
      adt_error_t result = adt_ary_reserve(self, new_capacity);
      if (result != ADT_NO_ERROR) {
         return result;
      }

      self->pFirst[self->s32CurLen++] = pElem;
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

/**
 * Same as adt_ary_push but skips adding if it already exists.
 * Runs in linear time.
 */
adt_error_t adt_ary_push_unique(adt_ary_t *self, void *pElem){
   if (self != NULL)
   {
      int32_t s32Index;
      for(s32Index = 0; s32Index < self->s32CurLen; s32Index++ )
      {
         if (self->pFirst[s32Index] == pElem){
            return ADT_NO_ERROR;
         }
      }
      return adt_ary_push(self, pElem);
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

void* adt_ary_pop(adt_ary_t *self){
   void *pElem;
   assert(self != NULL);
   if(self->s32CurLen==0){
      return NULL;
   }
   pElem = self->pFirst[--self->s32CurLen];
   if(self->s32CurLen == 0){
      //reallign pFirst with pAlloc when buffer becomes empty
      self->pFirst = self->ppAlloc;
   }
   return pElem;
}

/**
 * Removes the first element from the array
 */
void* adt_ary_shift(adt_ary_t *self){
   void *pElem;
   assert(self != NULL);
   if(self->s32CurLen==0){
      return NULL;
   }
   pElem=*(self->pFirst++); //move pFirst forward by 1
   self->s32CurLen--; //reduce array length by 1
   if(self->s32CurLen == 0){
      //reallign pFirst with pAlloc when buffer becomes empty
      self->pFirst = self->ppAlloc;
   }
   return pElem;
}

/**
 * Appends pElem to the beginning of the array, shifting all other elements one step forward
 */
adt_error_t adt_ary_unshift(adt_ary_t *self, void *pElem){
   if ( self != NULL ) {
      if (self->s32CurLen == INT32_MAX) {
         return ADT_LENGTH_ERROR; //cannot fit any more elements into the array
      }
      if(self->pFirst > self->ppAlloc){
         //room for one more element at the beginning
         *(--self->pFirst)=pElem;
         self->s32CurLen++;
         return ADT_NO_ERROR;
      }
      else {
         if (self->s32CurLen < self->s32AllocLen) {
            memmove(self->ppAlloc + 1, self->ppAlloc, sizeof(void*) * (size_t)self->s32CurLen);
            self->ppAlloc[0] = pElem;
            self->pFirst = self->ppAlloc;
            self->s32CurLen++;
            return ADT_NO_ERROR;
         }

         int32_t new_capacity = adt_ary_next_capacity(self->s32AllocLen, self->s32CurLen + 1);
         void **ppAlloc = (void**) malloc(sizeof(void*) * (size_t)new_capacity);
         if (ppAlloc == NULL) {
            return ADT_MEM_ERROR;
         }

         ppAlloc[0] = pElem;
         if (self->s32CurLen > 0) {
            memcpy(ppAlloc + 1, self->pFirst, sizeof(void*) * (size_t)self->s32CurLen);
         }

         if (self->ppAlloc != NULL) {
            free(self->ppAlloc);
         }

         self->ppAlloc = ppAlloc;
         self->pFirst = ppAlloc;
         self->s32AllocLen = new_capacity;
         self->s32CurLen++;
         return ADT_NO_ERROR;
      }
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}


//Utility functions
adt_error_t adt_ary_extend(adt_ary_t *self, int32_t s32Len){
   if (self != NULL) {
      void **ppAlloc;
      //check if current length is greater than requested length
      if( self->s32CurLen>=s32Len ) return ADT_NO_ERROR;

      //check if allocated length is greater than requested length
      if( (self->s32AllocLen>=s32Len) ){
         //shift array data to start of allocated array
         memmove(self->ppAlloc,self->pFirst,((unsigned int)self->s32CurLen) * sizeof(void*));
         self->pFirst = self->ppAlloc;
         self->s32CurLen = s32Len;
      }
      else {
         //need to allocate new array data element and copy data to newly allocated memory
         if(s32Len>= INT32_MAX){
            return ADT_LENGTH_ERROR;
         }
         ppAlloc = (void**) malloc(ELEM_SIZE*((unsigned int)s32Len));
         if (ppAlloc == NULL)
         {
            return ADT_MEM_ERROR;
         }
         if(self->ppAlloc){
            size_t numNewElems = (size_t) (s32Len - self->s32CurLen);
            memset(ppAlloc+self->s32CurLen, 0,  numNewElems * ELEM_SIZE);
            memcpy(ppAlloc,self->pFirst, ((unsigned int)self->s32CurLen) * ELEM_SIZE);
            free(self->ppAlloc);
         }
         self->ppAlloc = self->pFirst = ppAlloc;
         self->s32AllocLen = self->s32CurLen = s32Len;
      }
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

adt_error_t adt_ary_fill(adt_ary_t *self, int32_t s32Len){
   if (self != NULL) {
      int32_t s32Index;
      int32_t s32CurLen = self->s32CurLen;
      adt_error_t result;
      if(s32CurLen >= s32Len) {
         return ADT_NO_ERROR; //increase not necessary
      }
      result = adt_ary_extend(self,(int32_t)s32Len);
      if (result != ADT_NO_ERROR) {
         return result;
      }
      //set pFillElem to all newly created array elements
      for(s32Index=s32CurLen; s32Index<s32Len; s32Index++){
         self->pFirst[s32Index]=self->pFillElem;
      }
      assert(self->s32CurLen>=s32Len);
      assert(s32Index==self->s32CurLen);
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

adt_error_t adt_ary_resize(adt_ary_t *self, int32_t s32Len)
{
   if ( (self != NULL) && (s32Len >= 0) ) {
      adt_error_t result = adt_ary_fill(self,s32Len);
      if (result != ADT_NO_ERROR) {
         return result;
      }
      if (s32Len < self->s32CurLen) {
         int32_t delta = self->s32CurLen-s32Len;
         result = adt_ary_splice(self,s32Len,delta);
      }
      return result;
   }
   return ADT_INVALID_ARGUMENT_ERROR;
}

void adt_ary_clear(adt_ary_t *self){
   if(self){
      adt_ary_destroy(self);
   }
}

int32_t adt_ary_length(const adt_ary_t *self){
   if(self){
      return self->s32CurLen;
   }
   return -1;
}

bool adt_ary_is_empty(const adt_ary_t* self) {
   if (self) {
      return self->s32CurLen > 0? false : true;
   }
   return false;
}

//Returns nonzero if the element exists
int32_t adt_ary_exists(const adt_ary_t *self, int32_t s32Index){
   if(self == NULL){
      return 0;
   }
   if(s32Index<0){
      s32Index = (-s32Index);
      if(s32Index > (self->s32CurLen) ){
         //negative index outside array bounds
         return 0;
      }
      //negative index inside array bounds
      s32Index=self->s32CurLen-s32Index;
   }
   if(s32Index<self->s32CurLen){
      return 1;
   }
   return -1;
}

void adt_ary_create(adt_ary_t *self,void (*pDestructor)(void*)){
   self->ppAlloc = NULL;
   self->pFirst = NULL;
   self->s32AllocLen = 0;
   self->s32CurLen = 0;
   self->pDestructor = pDestructor;
   self->pFillElem = NULL;
   self->destructorEnable = true;
}

void adt_ary_destroy(adt_ary_t *self){
   int32_t s32i;

   void **ppElem=self->pFirst;
   if( (self->pDestructor != NULL) && (self->destructorEnable != false) ){
      for(s32i=0;s32i<(int32_t) self->s32CurLen;s32i++){
         self->pDestructor(*(ppElem++));
      }
   }
   if(self->ppAlloc != NULL){
      free(self->ppAlloc);
   }
   self->ppAlloc = NULL;
   self->s32AllocLen = 0;
   self->pFirst = NULL;
   self->s32CurLen = 0;
}

void adt_ary_set_fill_elem(adt_ary_t *self,void* pFillElem){
   if(self){
      self->pFillElem = pFillElem;
   }
}
void* adt_ary_get_fill_elem(const adt_ary_t *self){
   if(self){
      return self->pFillElem;
   }
   return NULL;
}

/**
 * removes s32Len items from array starting from s32Index (s32Index can be both positive or negative)
 */
adt_error_t adt_ary_splice(adt_ary_t *self,int32_t s32Index, int32_t s32Len){
   if ((self != NULL) && (s32Len > 0)) {
      int32_t s32Source;
      int32_t s32Destination;
      int32_t i;
      int32_t s32ElemsRemain;


      if(s32Index<0){
         s32Index = (-s32Index);
         if(s32Index > (self->s32CurLen) ){
            return ADT_INDEX_OUT_OF_BOUNDS_ERROR;
         }
         s32Index=self->s32CurLen-s32Index;
      }
      else if(s32Index >= self->s32CurLen){
         return ADT_INDEX_OUT_OF_BOUNDS_ERROR;
      }

      s32Destination = s32Index;
      s32Source = s32Index + s32Len;

      if (s32Source > self->s32CurLen) {
         return ADT_LENGTH_ERROR; //s32Len is too large
      }

      //call destructor on elements about to be removed
      if( (self->destructorEnable != false) && (self->pDestructor != NULL) ){
         for (i=0;i<s32Len; i++) {
            self->pDestructor(self->pFirst[s32Destination+i]);
         }
      }
      s32ElemsRemain = self->s32CurLen-s32Source;
      if (s32ElemsRemain > 0) {
         uint32_t u32BytesRemain;
         uint8_t* pDest;
         uint8_t* pSrc;
         pDest = (uint8_t*) &self->pFirst[s32Destination];
         pSrc = (uint8_t*) &self->pFirst[s32Source];
         u32BytesRemain = ((uint32_t)s32ElemsRemain) * ((uint32_t) ELEM_SIZE);
         adt_block_memmove(pDest, pSrc, u32BytesRemain);
      }
      self->s32CurLen-=s32Len;
      if (self->s32CurLen == 0) {
         self->pFirst = self->ppAlloc;
      }
      return ADT_NO_ERROR;
   }
   return ADT_INVALID_ARGUMENT_ERROR;

}

/**
 * sorts the array using the given key function.
 * If reverse is true it will be sorted in descending order, otherwise it will
 * be sorted in ascending order
 */
adt_error_t adt_ary_sort(adt_ary_t *self, adt_vlt_func_t *key, bool reverse)
{
   if ( (self == NULL) || (key == NULL) )
   {
      return ADT_INVALID_ARGUMENT_ERROR;
   }
   if (self->s32CurLen <= 1)
   {
      return ADT_NO_ERROR;
   }

   int32_t depth_limit = 0;
   for (int32_t n = self->s32CurLen; n > 1; n >>= 1)
   {
      depth_limit += 2;
   }

   return adt_ary_introsort(self->pFirst, 0, self->s32CurLen - 1, depth_limit, key, reverse);
}

int adt_i32_vlt(const void *a, const void *b)
{
   if ( (a != NULL) && (b != NULL) )
   {
      return ELEM_VALUE_IS_LESS(int32_t);
   }
   return -1;
}

int adt_u32_vlt(const void *a, const void *b)
{
   if ( (a != NULL) && (b != NULL) )
   {
      return ELEM_VALUE_IS_LESS(uint32_t);
   }
   return -1;
}

/**
 * Returns the array index of the element pElem. In the event that pElem is not in the array the function returns -1
 */
int32_t adt_ary_index_of(adt_ary_t *self, void *pElem)
{
   if (self != NULL)
   {
      int32_t s32Index;
      for(s32Index = 0; s32Index < self->s32CurLen; s32Index++ )
      {
         if (self->pFirst[s32Index] == pElem) {
            return s32Index;
         }
      }
   }
   return -1;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
/**
 * CG: I had some serious issues with some Microsoft compilers not handling large memmoves.
 * To mitigate this potential problem I use this function to transform one large memmoves into a series of smaller memmoves.
 */
static void adt_block_memmove(uint8_t *pDest, uint8_t*pSrc, uint32_t u32Remain){
   while(u32Remain>0){
      uint32_t u32Size = (u32Remain>DATA_BLOCK_MAX)? DATA_BLOCK_MAX : u32Remain;
      memmove(pDest, pSrc, u32Size);
      u32Remain-=u32Size;
      pDest+=u32Size;
      pSrc+=u32Size;
   }
}

#define ADT_SORT_THRESHOLD 16

static void adt_ary_swap(void **a, void **b)
{
   void *tmp = *a;
   *a = *b;
   *b = tmp;
}

static bool adt_ary_elem_less(const void *a, const void *b, adt_vlt_func_t *key, bool reverse, adt_error_t *err)
{
   int res = reverse ? key(b, a) : key(a, b);
   if (res < 0)
   {
      *err = ADT_OBJECT_COMPARE_ERROR;
      return false;
   }
   return (res > 0);
}

static adt_error_t adt_ary_insertion_sort_range(void **elems, int32_t start, int32_t end, adt_vlt_func_t *key, bool reverse)
{
   adt_error_t err = ADT_NO_ERROR;
   for (int32_t i = start + 1; i <= end; i++)
   {
      void *val = elems[i];
      int32_t j = i - 1;
      while (j >= start)
      {
         bool less = adt_ary_elem_less(val, elems[j], key, reverse, &err);
         if (err != ADT_NO_ERROR)
         {
            return err;
         }
         if (!less)
         {
            break;
         }
         elems[j + 1] = elems[j];
         j--;
      }
      elems[j + 1] = val;
   }
   return ADT_NO_ERROR;
}

static adt_error_t adt_ary_sift_down(void **elems, int32_t start, int32_t count, int32_t root, adt_vlt_func_t *key, bool reverse)
{
   adt_error_t err = ADT_NO_ERROR;
   while ((2 * root + 1) < count)
   {
      int32_t child = 2 * root + 1;
      if (child + 1 < count)
      {
         bool child_less = adt_ary_elem_less(elems[start + child], elems[start + child + 1], key, reverse, &err);
         if (err != ADT_NO_ERROR)
         {
            return err;
         }
         if (child_less)
         {
            child++;
         }
      }
      bool root_less = adt_ary_elem_less(elems[start + root], elems[start + child], key, reverse, &err);
      if (err != ADT_NO_ERROR)
      {
         return err;
      }
      if (root_less)
      {
         adt_ary_swap(&elems[start + root], &elems[start + child]);
         root = child;
      }
      else
      {
         break;
      }
   }
   return ADT_NO_ERROR;
}

static adt_error_t adt_ary_heapsort_range(void **elems, int32_t start, int32_t end, adt_vlt_func_t *key, bool reverse)
{
   int32_t count = end - start + 1;
   adt_error_t err = ADT_NO_ERROR;

   for (int32_t i = (count / 2) - 1; i >= 0; i--)
   {
      err = adt_ary_sift_down(elems, start, count, i, key, reverse);
      if (err != ADT_NO_ERROR)
      {
         return err;
      }
   }

   for (int32_t i = count - 1; i > 0; i--)
   {
      adt_ary_swap(&elems[start], &elems[start + i]);
      err = adt_ary_sift_down(elems, start, i, 0, key, reverse);
      if (err != ADT_NO_ERROR)
      {
         return err;
      }
   }
   return ADT_NO_ERROR;
}

static adt_error_t adt_ary_median_of_three(void **elems, int32_t a, int32_t b, int32_t c, adt_vlt_func_t *key, bool reverse)
{
   adt_error_t err = ADT_NO_ERROR;
   bool less;

   less = adt_ary_elem_less(elems[b], elems[a], key, reverse, &err);
   if (err != ADT_NO_ERROR) return err;
   if (less) adt_ary_swap(&elems[a], &elems[b]);

   less = adt_ary_elem_less(elems[c], elems[a], key, reverse, &err);
   if (err != ADT_NO_ERROR) return err;
   if (less) adt_ary_swap(&elems[a], &elems[c]);

   less = adt_ary_elem_less(elems[c], elems[b], key, reverse, &err);
   if (err != ADT_NO_ERROR) return err;
   if (less) adt_ary_swap(&elems[b], &elems[c]);

   return ADT_NO_ERROR;
}

static adt_error_t adt_ary_partition(void **elems, int32_t low, int32_t high, int32_t *pivot_idx, adt_vlt_func_t *key, bool reverse)
{
   adt_error_t err = ADT_NO_ERROR;
   int32_t mid = low + (high - low) / 2;

   err = adt_ary_median_of_three(elems, low, mid, high, key, reverse);
   if (err != ADT_NO_ERROR)
   {
      return err;
   }

   void *pivot = elems[mid];
   int32_t i = low - 1;
   int32_t j = high + 1;

   while (true)
   {
      do
      {
         i++;
      } while (adt_ary_elem_less(elems[i], pivot, key, reverse, &err));
      if (err != ADT_NO_ERROR)
      {
         return err;
      }

      do
      {
         j--;
      } while (adt_ary_elem_less(pivot, elems[j], key, reverse, &err));
      if (err != ADT_NO_ERROR)
      {
         return err;
      }

      if (i >= j)
      {
         *pivot_idx = j;
         return ADT_NO_ERROR;
      }

      adt_ary_swap(&elems[i], &elems[j]);
   }
}

static adt_error_t adt_ary_introsort(void **elems, int32_t low, int32_t high, int32_t depth_limit, adt_vlt_func_t *key, bool reverse)
{
   while ((high - low + 1) > ADT_SORT_THRESHOLD)
   {
      if (depth_limit <= 0)
      {
         return adt_ary_heapsort_range(elems, low, high, key, reverse);
      }
      depth_limit--;

      int32_t p = 0;
      adt_error_t err = adt_ary_partition(elems, low, high, &p, key, reverse);
      if (err != ADT_NO_ERROR)
      {
         return err;
      }

      if ((p - low) < (high - (p + 1)))
      {
         err = adt_ary_introsort(elems, low, p, depth_limit, key, reverse);
         if (err != ADT_NO_ERROR)
         {
            return err;
         }
         low = p + 1;
      }
      else
      {
         err = adt_ary_introsort(elems, p + 1, high, depth_limit, key, reverse);
         if (err != ADT_NO_ERROR)
         {
            return err;
         }
         high = p;
      }
   }
   return adt_ary_insertion_sort_range(elems, low, high, key, reverse);
}


