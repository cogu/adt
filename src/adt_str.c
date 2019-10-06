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
#include "adt_bytes.h"
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

#ifdef UNIT_TEST
# define DYN_STATIC
#else
# define DYN_STATIC static
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static int32_t adt_str_calcSize(int32_t s32CurSize, int32_t s32NewSize);
DYN_STATIC adt_str_encoding_t adt_utf8_checkEncoding(const uint8_t *strBuf, int32_t bufLen);
DYN_STATIC adt_str_encoding_t adt_utf8_checkEncodingAndSize(const uint8_t *strBuf, int32_t *strLen);
DYN_STATIC int32_t adt_utf8_readCodePoint(const uint8_t *strBuf, int32_t bufLen, int *codePoint);
static int adt_str_lt_ascii(const adt_str_t *self, const adt_str_t *other);
static int adt_str_lt_utf8(const adt_str_t *self, const adt_str_t *other);


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
      self->s32Size = 0;
      self->pStr = (uint8_t*) 0;
      self->lastError = ADT_NO_ERROR;
      self->encoding = ADT_STR_ENCODING_ASCII;
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
adt_str_t *adt_str_new_bstr(const uint8_t *pBegin, const uint8_t *pEnd)
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

adt_str_t *adt_str_new_bytearray(adt_bytearray_t *bytearray)
{
   if (bytearray != 0)
   {
      adt_str_t *self = (adt_str_t*) 0;
      const uint8_t *pBegin, *pEnd;
      pBegin = adt_bytearray_data(bytearray);
      pEnd = pBegin + adt_bytearray_length(bytearray);
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

adt_str_t *adt_str_new_utf8(void)
{
   adt_str_t *self = adt_str_new();
   if (self != 0)
   {
      adt_str_setEncoding(self, ADT_STR_ENCODING_UTF8);
   }
   return self;
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
      self->s32Size = 0;
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
      int32_t s32Size = other->s32Cur;
      if (s32Size > 0)
      {
         adt_error_t result;
         adt_str_reset(self);
         result = adt_str_reserve(self, s32Size);
         if (result == ADT_NO_ERROR)
         {
            assert(self->pStr != 0);
            assert(self->s32Size > s32Size);
            memcpy(self->pStr, other->pStr, s32Size);
            self->s32Cur = s32Size;
            self->encoding = other->encoding;
         }
      }
   }
   return retval;
}

/**
 * Overwrites this string with data from a bounded string (bstr)
 */
adt_error_t adt_str_set_bstr(adt_str_t *self, const uint8_t *pBegin, const uint8_t *pEnd)
{
   adt_error_t retval = ADT_NO_ERROR;

   if( (self == 0) || (pBegin == 0) || (pEnd == 0) || (pEnd < pBegin) )
   {
      retval = ADT_INVALID_ARGUMENT_ERROR;
   }
   else
   {
      int32_t s32Size = (int32_t) (pEnd-pBegin);
      if (s32Size > 0)
      {
         adt_error_t result;
         adt_str_encoding_t encoding = adt_utf8_checkEncoding(pBegin, s32Size);
         adt_str_reset(self);
         result = adt_str_reserve(self, s32Size);
         if (result == ADT_NO_ERROR)
         {
            assert(self->pStr != 0);
            assert(self->s32Size > s32Size);
            memcpy(self->pStr, pBegin, s32Size);
            self->s32Cur = s32Size;
            self->encoding = encoding;
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
      int32_t s32Size = 0;
      adt_str_encoding_t encoding = adt_utf8_checkEncodingAndSize((const uint8_t*) cstr, &s32Size);
      if ( (encoding != ADT_STR_ENCODING_UNKNOWN) && (s32Size > 0) )
      {
         adt_error_t result;
         adt_str_reset(self);
         result = adt_str_reserve(self, s32Size);
         if (result == ADT_NO_ERROR)
         {
            assert(self->pStr != 0);
            assert(self->s32Size > s32Size);
            memcpy(self->pStr, cstr, s32Size);
            self->s32Cur = s32Size;
            self->encoding = encoding;
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
            assert(self->s32Size > newLen);
            assert(self->pStr != 0);
            assert(other->pStr != 0);
            memcpy(self->pStr+self->s32Cur, other->pStr, other->s32Cur);
            self->s32Cur = newLen;
            if ( (self->encoding == ADT_STR_ENCODING_ASCII) && (other->encoding == ADT_STR_ENCODING_UTF8) )
            {
               self->encoding = other->encoding;
            }
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
adt_error_t adt_str_append_bstr(adt_str_t *self, const uint8_t *pBegin, const uint8_t *pEnd)
{
   adt_error_t retval = ADT_NO_ERROR;
   if( (self == 0) || (pBegin == 0) || (pEnd == 0) || (pEnd < pBegin) )
   {
      retval = ADT_INVALID_ARGUMENT_ERROR;
   }
   else
   {
      int32_t s32Size = (int32_t) (pEnd-pBegin);
      if (s32Size > 0)
      {
         adt_str_encoding_t encoding = adt_utf8_checkEncoding(pBegin, s32Size);
         if ( (encoding == ADT_STR_ENCODING_ASCII) || (encoding == ADT_STR_ENCODING_UTF8) )
         {
            adt_error_t result;
            int32_t newLen = self->s32Cur + s32Size;
            result = adt_str_reserve(self, newLen);
            if (result == ADT_NO_ERROR)
            {
               assert(self->pStr != 0);
               assert(self->s32Size > newLen);
               memcpy(self->pStr + self->s32Cur, pBegin, s32Size);
               self->s32Cur = newLen;
               if ( (self->encoding == ADT_STR_ENCODING_ASCII) && (encoding == ADT_STR_ENCODING_UTF8) )
               {
                  self->encoding = encoding;
               }
            }
            else
            {
               retval = result;
            }
         }
         else
         {
            retval = ADT_UNKNOWN_ENCODING_ERROR;
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
      int32_t s32Size = 0;
      adt_str_encoding_t encoding = adt_utf8_checkEncodingAndSize((const uint8_t*) cstr, &s32Size);
      if ( (encoding != ADT_STR_ENCODING_UNKNOWN) && (s32Size > 0) )
      {
         adt_error_t result;
         int32_t newSize = self->s32Cur+s32Size;
         result = adt_str_reserve(self, newSize);
         if (result == ADT_NO_ERROR)
         {
            assert(self->s32Size > newSize);
            memcpy(self->pStr+self->s32Cur, cstr, s32Size);
            self->s32Cur = newSize;
            if ( (self->encoding == ADT_STR_ENCODING_ASCII) && (encoding == ADT_STR_ENCODING_UTF8) )
            {
               self->encoding = encoding;
            }
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
void adt_str_prepend_bstr(adt_str_t *self, const uint8_t *pBegin, const uint8_t *pEnd);
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
            assert(self->s32Cur < self->s32Size);
            self->pStr[self->s32Cur++] = (uint8_t) c;
            assert(self->s32Cur < self->s32Size);
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
         assert(self->s32Cur < self->s32Size);
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

adt_error_t adt_str_bstr(adt_str_t *self, const char **ppBegin, const char **ppEnd)
{
   (void) self;
   (void) ppBegin;
   (void) ppEnd;
   return ADT_NOT_IMPLEMENTED_ERROR;
}

adt_bytearray_t *adt_str_bytearray(adt_str_t *self)
{
   adt_bytearray_t *retval = (adt_bytearray_t*) 0;
   if (self != 0)
   {
      retval = adt_bytearray_new(ADT_BYTE_ARRAY_NO_GROWTH);
      if (retval != 0)
      {
         adt_error_t result;
         uint32_t u32Size = (uint32_t) self->s32Cur;
         result = adt_bytearray_resize(retval, u32Size);
         if (result == ADT_NO_ERROR)
         {
            assert(adt_bytearray_length(retval) == u32Size);
            memcpy(adt_bytearray_data(retval), self->pStr, (size_t) self->s32Cur);
         }
         else
         {
            adt_bytearray_delete(retval);
            retval = (adt_bytearray_t*) 0;
         }
      }
   }
   return retval;
}

struct adt_bytes_tag *adt_str_bytes(adt_str_t *self)
{
   adt_bytes_t *retval = (adt_bytes_t*) 0;
   if (self != 0)
   {
      retval = adt_bytes_new(self->pStr, (uint32_t) self->s32Cur);
   }
   return retval;
}



/* utility */

void adt_str_setEncoding(adt_str_t *self, adt_str_encoding_t newEncoding)
{
   if ( (self != 0) && ( (newEncoding == ADT_STR_ENCODING_ASCII) || (newEncoding == ADT_STR_ENCODING_UTF8) ))
   {
      self->encoding = newEncoding;
   }
}

adt_str_encoding_t adt_str_getEncoding(adt_str_t *self)
{
   if (self != 0)
   {
      return self->encoding;
   }
   return ADT_STR_ENCODING_UNKNOWN;
}

int32_t adt_str_length(const adt_str_t *self)
{
   int32_t retval = -1;
   if (self != 0)
   {
      if (self->encoding == ADT_STR_ENCODING_ASCII)
      {
         retval = self->s32Cur;
      }
      else if (self->encoding == ADT_STR_ENCODING_UTF8)
      {
         const uint8_t *p = self->pStr;
         int32_t remain = self->s32Cur;
         retval = 0;
         while(remain > 0)
         {
            int codePoint;
            int32_t size = adt_utf8_readCodePoint(p, remain, &codePoint);
            retval++;
            remain-=size;
            p+=size;
            if (remain < 0)
            {
               retval = -1;
               break;
            }
         }
      }
      else
      {
         //not implemented
      }
   }
   return retval;
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
      if (self->s32Size < s32NewLen)
      {
         uint8_t *pStr;
         int32_t s32Size;

         s32Size = adt_str_calcSize(self->s32Size, s32NewLen);
         pStr = (uint8_t*) malloc(s32Size);
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
            self->s32Size = s32Size;
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
      self->encoding = ADT_STR_ENCODING_ASCII;
   }
}

bool adt_str_equal(const adt_str_t *self, const adt_str_t* other);
bool adt_str_equal_bstr(const adt_str_t *self, const char *pBegin, const char *pEnd);
bool adt_str_equal_cstr(const adt_str_t *self, const char *cstr)
{
   if ( (self != 0) && (cstr != 0))
   {
      if ( (*cstr == 0) && (self->s32Cur == 0) )
      {
         return true;
      }
      else if (self->s32Cur > 0 && self->pStr != 0)
      {
         const uint8_t *pStr = (const uint8_t*) cstr;
         const uint8_t *pNext = self->pStr;
         const uint8_t *pEnd = self->pStr + self->s32Cur;
         while(pNext < pEnd)
         {
            uint8_t c = *pNext;
            uint8_t d = *pStr;
            if ( (c != d) || (d == 0) )
            {
               break;
            }
            ++pNext;
            ++pStr;
         }
         if ( (*pStr == 0) && (pNext == pEnd) )
         {
            return true;
         }
      }
   }
   return false;
}

/*
 * Returns 1 if left string is lexicographically less than than right string.
 * Returns 0 otherwise.
 * Retuns -1 in error.
 */
int adt_str_lt(const adt_str_t *self, const adt_str_t *other)
{
   if ( (self != 0) && (other != 0) )
   {
      if ( (self->encoding == ADT_STR_ENCODING_ASCII) && (other->encoding == ADT_STR_ENCODING_ASCII) )
      {
         return adt_str_lt_ascii(self, other);
      }
      else
      {
         assert( (self->encoding == ADT_STR_ENCODING_ASCII) || (self->encoding == ADT_STR_ENCODING_UTF8) );
         assert( (other->encoding == ADT_STR_ENCODING_ASCII) || (other->encoding == ADT_STR_ENCODING_UTF8) );
         return adt_str_lt_utf8(self, other);
      }
   }
   return -1;
}


int adt_str_vlt(const void *a, const void *b)
{
   return adt_str_lt ((const adt_str_t*) a, (const adt_str_t*) b);
}

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

static int32_t adt_str_calcSize(int32_t s32CurSize, int32_t s32NewSize)
{

   if(s32CurSize>=s32NewSize)
   {
      return s32CurSize;
   }

   if(s32NewSize > (INT_MAX - LARGE_BLOCK_SIZE) )
   {
      return INT_MAX;
   }
   else if( s32NewSize>=LARGE_BLOCK_SIZE )
   {
      //upgrade s32CurLen to LARGE_BLOCK_SIZE
      if( s32CurSize < LARGE_BLOCK_SIZE )
      {
         s32CurSize = LARGE_BLOCK_SIZE;
      }
      while(s32CurSize<s32NewSize) s32CurSize+=LARGE_BLOCK_SIZE;
   }
   else if(s32NewSize >= MEDIUM_BLOCK_SIZE)
   {
      //upgrade s32CurLen to MEDIUM_BLOCK_SIZE
      if( s32CurSize < MEDIUM_BLOCK_SIZE )
      {
         s32CurSize = MEDIUM_BLOCK_SIZE;
      }
      while(s32CurSize<s32NewSize) s32CurSize*=2;
   }
   else
   {
      //upgrade s32CurLen to MIN_BLOCK_SIZE
      if( s32CurSize < MIN_BLOCK_SIZE )
      {
         s32CurSize = MIN_BLOCK_SIZE;
      }
      while(s32CurSize<s32NewSize) s32CurSize*=4;
   }
   return s32CurSize;
}

/**
 * Goes through characters in string buffer and attempts to determine its encoding
 *
 */
DYN_STATIC adt_str_encoding_t adt_utf8_checkEncoding(const uint8_t *strBuf, int32_t bufLen)
{
   int32_t i;
   for(i=0; i<bufLen; i++)
   {
      if ( strBuf[i] > 127)
      {
         uint8_t c = strBuf[i] & 0xf8;
         if ( (c == 0xf0) || ( (c & 0xf0) == 0xe0) || ( (c & 0xe0) == 0xc0) )
         {
            return ADT_STR_ENCODING_UTF8;
         }
         return ADT_STR_ENCODING_UTF16; //this is a guess
      }
   }
   return ADT_STR_ENCODING_ASCII;
}

/**
 * Returns both encoding and size of the string. The input string must be null-terminated.
 */
DYN_STATIC adt_str_encoding_t adt_utf8_checkEncodingAndSize(const uint8_t *strBuf, int32_t *strSize)
{
   if ( (strBuf == 0) || (strSize == 0) )
   {
      return ADT_STR_ENCODING_UNKNOWN;
   }
   const uint8_t *p = strBuf;
   uint8_t c = 0;
   adt_str_encoding_t retval = ADT_STR_ENCODING_ASCII;
   int32_t size = 0;
   while(1)
   {
      c = *p++;
      if (c == 0)
      {
         break;
      }
      else if ( (retval == ADT_STR_ENCODING_ASCII) && (c > 127) )
      {
         if ( ( (c & 0xf8) == 0xf0) || ( (c & 0xf0) == 0xe0) || ( (c & 0xe0) == 0xc0) )
         {
            retval = ADT_STR_ENCODING_UTF8;
         }
         else
         {
            retval = ADT_STR_ENCODING_UTF16; //this is a guess
         }
      }
      size++;
   }
   *strSize = size;
   return retval;
}

DYN_STATIC int32_t adt_utf8_readCodePoint(const uint8_t *strBuf, int32_t bufLen, int *codePoint)
{
   int tmp = 0;
   int32_t retval = 0;
   if ( (strBuf == 0) || (bufLen < 0) || (codePoint == 0) )
   {
      return ADT_UTF8_INVALID_ARGUMENT;
   }
   if (bufLen > 0)
   {
      int32_t expectedLen = 0;
      uint8_t c = (uint8_t) *strBuf++;
      if (c < 128u)
      {
         expectedLen = 1;
         tmp = c;
      }
      else if ( ( (c & 0xe0) == 0xc0) )
      {
         expectedLen = 2;
         tmp = c & 0x1F;

      }
      else if ( ( (c & 0xf0) == 0xe0) )
      {
         expectedLen = 3;
         tmp = c & 0x0F;
      }
      else if ( ( (c & 0xf8) == 0xf0) )
      {
         expectedLen = 4;
         tmp = c & 0x07;
      }
      else
      {
         retval = ADT_UTF8_INVALID_ENCODING;
      }
      if ( expectedLen > 0 )
      {
         if (bufLen >= expectedLen)
         {
            retval = expectedLen--;
            while(expectedLen > 0)
            {
               c = (uint8_t) *strBuf++;
               if ( ( (c & 0xc0) != 0x80) )
               {
                  retval = ADT_UTF8_INVALID_ENCODING;
                  break;
               }
               tmp <<= 6;
               tmp |= c & 0x3F;
               --expectedLen;
            }
         }
      }
      else
      {
         retval = 0; //not enough bytes in buffer, try again later
      }
   }
   if (retval > 0)
   {
      *codePoint = tmp;
   }
   return retval;
}

static int adt_str_lt_ascii(const adt_str_t *self, const adt_str_t *other)
{
   const uint8_t *strLeft = self->pStr;
   const uint8_t *strRight = other->pStr;
   int32_t remainLeft = self->s32Cur;
   int32_t remainRight = other->s32Cur;

   while( remainLeft > 0 )
   {
      if ( remainRight == 0 )
      {
         return 1;
      }
      else
      {
         uint8_t leftChr = *strLeft;
         uint8_t rightChr = *strRight;
         if ( leftChr < rightChr )
         {
            return 1;
         }
         else if ( leftChr > rightChr )
         {
            return 0;
         }
         else
         {
            strLeft++;
            strRight++;
            remainLeft--;
            remainRight--;
         }
      }
   }
   return 0;
}

static int adt_str_lt_utf8(const adt_str_t *self, const adt_str_t *other)
{
   const uint8_t *strLeft = self->pStr;
   const uint8_t *strRight = other->pStr;
   int32_t remainLeft = self->s32Cur;
   int32_t remainRight = other->s32Cur;

   while( remainLeft > 0 )
   {
      if ( remainRight == 0 )
      {
         return 1;
      }
      else
      {
         int32_t leftSize;
         int32_t rightSize;
         int leftCodePoint;
         int rightCodePoint;
         leftSize = adt_utf8_readCodePoint(strLeft, remainLeft, &leftCodePoint);
         rightSize = adt_utf8_readCodePoint(strRight, remainRight, &rightCodePoint);
         if ( (leftSize <= 0) || (rightSize <= 0) )
         {
            return -1; //failed to read a code point
         }
         if ( leftCodePoint < rightCodePoint )
         {
            return 1;
         }
         else if ( leftCodePoint > rightCodePoint )
         {
            return 0;
         }
         else
         {
            assert(leftSize <= remainLeft);
            assert(rightSize <= remainRight);
            strLeft += leftSize;
            strRight += rightSize;
            remainLeft -= leftSize;
            remainRight -= rightSize;
         }
      }
   }
   return 0;
}
