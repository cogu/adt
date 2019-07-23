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

#ifdef UNIT_TEST
# define DYN_STATIC
#else
# define DYN_STATIC static
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static int32_t adt_str_calcSize(int32_t s32CurSize, int32_t s32NewSize);
DYN_STATIC adt_str_encoding_t adt_utf8_check_encoding(const uint8_t *strBuf, int32_t bufLen);
DYN_STATIC int32_t adt_utf8_readCodePoint(const uint8_t *strBuf, int32_t bufLen, int *codePoint);


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
adt_error_t adt_str_set_bstr(adt_str_t *self, const char *pBegin, const char *pEnd)
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
         adt_str_encoding_t encoding = adt_utf8_check_encoding((const uint8_t*) pBegin, s32Size);
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
      int32_t s32Size = (int32_t) strlen(cstr);
      if (s32Size > 0)
      {
         adt_error_t result;
         adt_str_encoding_t encoding = adt_utf8_check_encoding((const uint8_t*) cstr, s32Size);
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
            assert(self->s32Size > newLen);
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
            assert(self->s32Size > newLen);
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

adt_bytearray_t *adt_str_bytearray(adt_str_t *self);
adt_error_t adt_str_bstr(adt_str_t *self, const char **ppBegin, const char **ppEnd);

/* utility */

void adt_str_setEncoding(adt_str_t *self, adt_str_encoding_t newEncoding)
{
   if ( (self != 0) && ( (newEncoding == ADT_STR_ENCODING_ASCII) || (newEncoding == ADT_STR_ENCODING_UTF8) ))
   {
      self->encoding = newEncoding;
   }
}

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
   }
}

bool adt_str_equal(const adt_str_t *self, const adt_str_t* other);
bool adt_str_equal_bstr(const adt_str_t *self, const char *pBegin, const char *pEnd);
bool adt_str_equal_cstr(const adt_str_t *self, const char *cstr);

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
DYN_STATIC adt_str_encoding_t adt_utf8_check_encoding(const uint8_t *strBuf, int32_t bufLen)
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
         return ADT_STR_ENCODING_UNKNOWN;
      }
   }
   return ADT_STR_ENCODING_ASCII;
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
