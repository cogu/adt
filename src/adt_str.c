/*****************************************************************************
* \file      adt_str.c
* \author    Conny Gustafsson
* \date      2013-05-09
* \brief     String Container
*
* Copyright 2013-2019 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "adt_str.h"
#include <string.h>
#include <limits.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

/*
 * Each string starts out being 16 bytes in length
 * When self becomes too small, it quadruples in size (multiply by 4) until it reaches 64K.
 * It then doubles until it reaches 64MB. After that it does linear growth in chunks of 64MB.
 */
#define MIN_BLOCK_SIZE     16
#define MEDIUM_BLOCK_SIZE  65536
#define LARGE_BLOCK_SIZE   67108864

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static int32_t adt_str_calcLen(int32_t s32CurLen, int32_t s32NewLen);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

/* constructors */

void adt_str_create(adt_str_t *self)
{
   if(self)
   {
      self->s32Cur = 0;
      self->s32Len = 0;
      self->pStr = (uint8_t*) 0;
      self->lastError = ADT_NO_ERROR;
   }
}

adt_str_t *adt_str_new(void)
{
   adt_str_t *self;
   if((self = (adt_str_t*) malloc(sizeof(adt_str_t))) == 0){
      return (adt_str_t*)0;
   }
   adt_str_create(self);
   return self;
}

/**
 * Creates a copy of another ADT string
 */
adt_str_t *adt_str_clone(const adt_str_t* other)
{
   adt_str_t *self = (adt_str_t*) 0;
   if (other != 0)
   {
      self = adt_str_new();
      if (self != 0)
      {
         adt_error_t result = adt_str_set(self, other);
         if (result != ADT_NO_ERROR)
         {
            adt_str_delete(self);
            self = (adt_str_t*) 0;
         }
      }
   }
   return self;
}

/**
 * Creates a new string based on a bounded string (bstr)
 * alias: adt_str_make
 */
adt_str_t *adt_str_new_bstr(const char *pBegin, const char *pEnd)
{
   adt_str_t *self = (adt_str_t*) 0;
   if( (pBegin != 0) && (pEnd != 0) && (pEnd >= pBegin) )
   {
      self = adt_str_new();
      if (self != 0)
      {
         adt_error_t result = adt_str_set_bstr(self, pBegin, pEnd);
         if (result != ADT_NO_ERROR)
         {
            adt_str_delete(self);
            self = (adt_str_t*) 0;
         }
      }
   }
   return self;
}

adt_str_t *adt_str_new_cstr(const char * other)
{
   adt_str_t *self = (adt_str_t*) 0;
   if( other != 0 )
   {
      self = adt_str_new();
      if (self != 0)
      {
         adt_error_t result = adt_str_set_cstr(self, other);
         if (result != ADT_NO_ERROR)
         {
            adt_str_delete(self);
            self = (adt_str_t*) 0;
         }
      }
   }
   return self;
}

adt_str_t *adt_str_new_byterray(adt_bytearray_t *bytearray)
{
   //NOT YET IMPLEMENTED
   (void) bytearray;
   return (adt_str_t*) 0;
}

/**
 * Creates a new string by concatenating two existing strings together.
 * This function does not mutate existing strings.
 */
adt_str_t *adt_str_concat(const adt_str_t *lhs, const adt_str_t *rhs)
{
   //NOT YET IMPLEMENTED
   (void) lhs;
   (void) rhs;
   return (adt_str_t*) 0;
}

/* destructors */

void adt_str_destroy(adt_str_t *self)
{
   if(self != 0)
   {
      if(self->pStr != 0)
      {
         free(self->pStr);
         self->pStr = 0;
      }
      self->s32Cur = 0;
      self->s32Len = 0;
      self->lastError = ADT_NO_ERROR;
   }
}

void adt_str_delete(adt_str_t *self)
{
   if(self != 0)
   {
      adt_str_destroy(self);
      free(self);
   }
}

void adt_str_vdelete(void *arg)
{
   adt_str_delete((adt_str_t*) arg);
}

/**** string manipulation ****/


/**
 * Overwrites this string with data from another string
 */
adt_error_t adt_str_set(adt_str_t *self, const adt_str_t* other)
{
   adt_error_t retval = ADT_NO_ERROR;

   if( (self == 0) || (other == 0) )
   {
      retval = ADT_INVALID_ARGUMENT_ERROR;
   }
   else
   {
      int32_t s32Len = other->s32Cur;
      if (s32Len > 0)
      {
         adt_error_t result;
         adt_str_reset(self);
         result = adt_str_reserve(self, s32Len);
         if (result == ADT_NO_ERROR)
         {
            assert(self->pStr != 0);
            assert(self->s32Len > s32Len);
            memcpy(self->pStr, other->pStr, s32Len);
            self->s32Cur = s32Len;
         }
      }
   }
   return retval;
}

/**
 * Overwrites this string with data from a bounded string (bstr)
 */
adt_error_t adt_str_set_bstr(adt_str_t *self, const char *pBegin, const char *pEnd)
{
   adt_error_t retval = ADT_NO_ERROR;

   if( (self == 0) || (pBegin == 0) || (pEnd == 0) || (pEnd < pBegin) )
   {
      retval = ADT_INVALID_ARGUMENT_ERROR;
   }
   else
   {
      int32_t s32Len = (int32_t) (pEnd-pBegin);
      if (s32Len > 0)
      {
         adt_error_t result;
         adt_str_reset(self);
         result = adt_str_reserve(self, s32Len);
         if (result == ADT_NO_ERROR)
         {
            assert(self->pStr != 0);
            assert(self->s32Len > s32Len);
            memcpy(self->pStr, pBegin, s32Len);
            self->s32Cur = s32Len;
         }
      }
   }
   return retval;
}

/**
 * Overwrites this string with data from a C string (cstr)
 */
adt_error_t adt_str_set_cstr(adt_str_t *self, const char *cstr)
{
   adt_error_t retval = ADT_NO_ERROR;
   if( (self == 0) || (cstr == 0) )
   {
      retval = ADT_INVALID_ARGUMENT_ERROR;
   }
   else
   {
      int32_t s32Len = (int32_t) strlen(cstr);
      if (s32Len > 0)
      {
         adt_error_t result;
         adt_str_reset(self);
         result = adt_str_reserve(self, s32Len);
         if (result == ADT_NO_ERROR)
         {
            assert(self->pStr != 0);
            assert(self->s32Len > s32Len);
            memcpy(self->pStr, cstr, s32Len);
            self->s32Cur = s32Len;
         }
      }
   }
   return retval;
}

/**
 * Appends another string after this string
 */
adt_error_t adt_str_append(adt_str_t *self, const adt_str_t* other)
{
   adt_error_t retval = ADT_NO_ERROR;
   if ( (self == 0) || (other == 0) )
   {
      retval = ADT_INVALID_ARGUMENT_ERROR;
   }
   else
   {
      if (other->s32Cur > 0)
      {
         adt_error_t result;
         int32_t newLen = self->s32Cur + other->s32Cur;
         result = adt_str_reserve(self, newLen);
         if (result == ADT_NO_ERROR)
         {
            assert(self->s32Len > newLen);
            assert(self->pStr != 0);
            assert(other->pStr != 0);
            memcpy(self->pStr+self->s32Cur, other->pStr, other->s32Cur);
            self->s32Cur = newLen;
         }
         else
         {
            retval = result;
         }
      }
   }
   return retval;
}

/**
 * Appends data from a bounded string after this string
 */
adt_error_t adt_str_append_bstr(adt_str_t *self, const char *pBegin, const char *pEnd)
{
   adt_error_t retval = ADT_NO_ERROR;
   if( (self == 0) || (pBegin == 0) || (pEnd == 0) || (pEnd < pBegin) )
   {
      retval = ADT_INVALID_ARGUMENT_ERROR;
   }
   else
   {
      int32_t strLen = (int32_t) (pEnd-pBegin);
      if (strLen > 0)
      {
         adt_error_t result;
         int32_t newLen = self->s32Cur + strLen;
         result = adt_str_reserve(self, newLen);
         if (result == ADT_NO_ERROR)
         {
            assert(self->pStr != 0);
            assert(self->s32Len > newLen);
            memcpy(self->pStr + self->s32Cur, pBegin, strLen);
            self->s32Cur = newLen;
         }
         else
         {
            retval = result;
         }
      }
   }
   return retval;
}

/**
 * Appends data from a C string after this string
 */
adt_error_t adt_str_append_cstr(adt_str_t *self, const char *cstr)
{
   adt_error_t retval = ADT_NO_ERROR;
   if ( (self == 0) || (cstr == 0) )
   {
      retval = ADT_INVALID_ARGUMENT_ERROR;
   }
   else
   {
      int32_t strLen = (int32_t) strlen(cstr);
      if (strLen > 0)
      {
         adt_error_t result;
         int32_t newLen = self->s32Cur+strLen;
         result = adt_str_reserve(self, newLen);
         if (result == ADT_NO_ERROR)
         {
            assert(self->s32Len > newLen);
            memcpy(self->pStr+self->s32Cur, cstr, strLen);
            self->s32Cur = newLen;
         }
         else
         {
            retval = result;
         }
      }
   }
   return retval;
}

void adt_str_prepend(adt_str_t *self, const adt_str_t* other);
void adt_str_prepend_bstr(adt_str_t *self, const char *pBegin, const char *pEnd);
void adt_str_prepend_cstr(adt_str_t *self, const char *cstr);

adt_error_t adt_str_push(adt_str_t *self, const int c)
{
   adt_error_t retval = ADT_NO_ERROR;
   if( (self == 0) || (c < 0) )
   {
      retval = ADT_INVALID_ARGUMENT_ERROR;
   }
   else
   {
      if (self->s32Cur > INT_MAX-2)
      {
         retval = ADT_ARRAY_TOO_LARGE_ERROR;
      }
      else
      {
         adt_error_t result = adt_str_reserve(self, self->s32Cur+1);
         if (result == ADT_NO_ERROR)
         {
            assert(self->s32Cur < self->s32Len);
            self->pStr[self->s32Cur++] = (uint8_t) c;
            assert(self->s32Cur < self->s32Len);
         }
      }
   }
   return retval;
}

int adt_str_pop(adt_str_t *self)
{
   int retval = -1;
   if(self != 0)
   {
      if(self->s32Cur>0)
      {
         retval = (int) self->pStr[--self->s32Cur];
      }
   }
   return retval;
}

/* getters */

int adt_str_charAt(adt_str_t *self, int index)
{
   int retval = -1;
   adt_error_t errorCode = ADT_NO_ERROR;
   if(self != 0)
   {
      int32_t s32Index;
      if(index < 0)
      {
         s32Index=(int32_t) (-index);
         if(s32Index <= self->s32Cur)
         {
            s32Index = self->s32Cur-s32Index;
         }
         else
         {
            errorCode = ADT_INDEX_OUT_OF_BOUNDS_ERROR;
         }
      }
      else
      {
         s32Index = (int32_t) index;
         if(s32Index >= self->s32Cur)
         {
            errorCode = ADT_INDEX_OUT_OF_BOUNDS_ERROR;
         }
      }
      if (errorCode == ADT_NO_ERROR)
      {
         retval = (int) self->pStr[s32Index];
      }
      else
      {
         self->lastError = errorCode;
      }
   }
   return retval;

}

const char* adt_str_cstr(adt_str_t *self)
{
   const char *retval = (const char*) 0;
   if(self != 0)
   {
      adt_error_t result = ADT_NO_ERROR;
      if(self->pStr == 0)
      {
         result = adt_str_reserve(self, 0);
      }
      if (result == ADT_NO_ERROR)
      {
         assert(self->s32Cur < self->s32Len);
         self->pStr[self->s32Cur] = 0u;
         retval = (const char*) self->pStr;
      }
      else
      {
         self->lastError = result;
      }
   }
   return retval;
}

adt_bytearray_t *adt_str_bytearray(adt_str_t *self);
adt_error_t adt_str_bstr(adt_str_t *self, const char **ppBegin, const char **ppEnd);

/* utility */

int32_t adt_str_length(const adt_str_t *self)
{
   ///TODO: This function needs to return number of characters in the string.

   //In the meantime, report number of bytes in the string since works for ASCII-strings.
   return adt_str_size(self);
}

int32_t adt_str_size(const adt_str_t *self)
{
   if (self != 0)
   {
      return self->s32Cur;
   }
   return -1;
}

/**
 * Reserves memory for a larger string without actually changing string contents.
 */
adt_error_t adt_str_reserve(adt_str_t *self, int32_t s32NewLen)
{
   adt_error_t retval = ADT_NO_ERROR;

   if ( (self == 0) || (s32NewLen < 0) )
   {
      retval = ADT_INVALID_ARGUMENT_ERROR;
   }
   else
   {
      s32NewLen++; //reserve 1 byte for null terminator
      if (self->s32Len < s32NewLen)
      {
         uint8_t *pStr;
         int32_t s32Len;

         s32Len = adt_str_calcLen(self->s32Len, s32NewLen);
         pStr = (uint8_t*) malloc(s32Len);
         if (pStr == 0)
         {
            retval = ADT_MEM_ERROR;
         }
         else
         {
            if(self->pStr != 0)
            {
               memcpy(pStr,self->pStr, self->s32Cur);
               free(self->pStr);
            }
            self->pStr = pStr;
            self->s32Len = s32Len;
         }
      }
   }
   return retval;
}

/**
 * Clears string but keeps allocated memory
 */
void adt_str_clear(adt_str_t *self)
{

   if (self != 0)
   {
      self->s32Cur = 0;
      self->lastError = ADT_NO_ERROR;
   }
}

bool adt_str_equal(const adt_str_t *self, const adt_str_t* other);
bool adt_str_equal_bstr(const adt_str_t *self, const char *pBegin, const char *pEnd);
bool adt_str_equal_cstr(const adt_str_t *self, const char *cstr);
void free_void(void *ptr);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

/**
 * Calculates the amount of memory to allocate for a string.
 *
 * Each string starts out being 16 bytes in length
 * When that becomes too small, it quadruples in size (multiply by 4) until it reaches 64K.
 * It then doubles until it reaches 64MB. After that it does linear growth in chunks of 64MB.
 */

static int32_t adt_str_calcLen(int32_t s32CurLen, int32_t s32NewLen)
{

   if(s32CurLen>=s32NewLen)
   {
      return s32CurLen;
   }

   if(s32NewLen > (INT_MAX - LARGE_BLOCK_SIZE) )
   {
      return INT_MAX;
   }
   else if( s32NewLen>=LARGE_BLOCK_SIZE )
   {
      //upgrade s32CurLen to LARGE_BLOCK_SIZE
      if( s32CurLen < LARGE_BLOCK_SIZE )
      {
         s32CurLen = LARGE_BLOCK_SIZE;
      }
      while(s32CurLen<s32NewLen) s32CurLen+=LARGE_BLOCK_SIZE;
   }
   else if(s32NewLen >= MEDIUM_BLOCK_SIZE)
   {
      //upgrade s32CurLen to MEDIUM_BLOCK_SIZE
      if( s32CurLen < MEDIUM_BLOCK_SIZE )
      {
         s32CurLen = MEDIUM_BLOCK_SIZE;
      }
      while(s32CurLen<s32NewLen) s32CurLen*=2;
   }
   else
   {
      //upgrade s32CurLen to MIN_BLOCK_SIZE
      if( s32CurLen < MIN_BLOCK_SIZE )
      {
         s32CurLen = MIN_BLOCK_SIZE;
      }
      while(s32CurLen<s32NewLen) s32CurLen*=4;
   }
   return s32CurLen;
}


