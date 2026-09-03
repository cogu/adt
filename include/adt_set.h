#ifndef ADT_SET_H
#define ADT_SET_H

/**
* set data type
* This is an early prototype only which is very inefficient.
* Current implementation would need to be replaced by either hash table or tree data structure to boost performance
*/

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "adt_list.h"
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


//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
typedef struct adt_u32Set_tag
{
   adt_u32List_t list; //TODO: replace with a data structure that has better performance
} adt_u32Set_t;


//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

void adt_u32Set_create(adt_u32Set_t *self);
void adt_u32Set_destroy(adt_u32Set_t *self);
adt_u32Set_t*  adt_u32Set_new(void);
void  adt_u32Set_delete(adt_u32Set_t *self);
void  adt_u32Set_vdelete(void *arg);
int32_t adt_u32Set_length(adt_u32Set_t *self);
void adt_u32Set_clear(adt_u32Set_t *self);
void adt_u32Set_insert(adt_u32Set_t *self, uint32_t val);
bool adt_u32Set_remove(adt_u32Set_t *self, uint32_t val);
bool adt_u32Set_contains(adt_u32Set_t *self, uint32_t val);
bool adt_u32Set_is_empty(const adt_u32Set_t *self);

#endif //ADT_SET_H
