#ifndef ADT_LIST_H
#define ADT_LIST_H
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

typedef struct adt_list_elem_tag
{
   struct adt_list_elem_tag *pNext;
   struct adt_list_elem_tag *pPrev;
   void *pItem;
}adt_list_elem_t;

typedef struct adt_list_tag{
   adt_list_elem_t *pFirst;
   adt_list_elem_t *pLast;
   adt_list_elem_t *pIter;
   void (*pDestructor)(void*);
   bool destructorEnable;
} adt_list_t;

/***************** Public Function Declarations *******************/
void  adt_list_create(adt_list_t *self, void (*pDestructor)(void*));
void  adt_list_destroy(adt_list_t *self);
adt_list_t*  adt_list_new(void (*pDestructor)(void*));
void  adt_list_delete(adt_list_t *self);
void  adt_list_vdelete(void *arg);
void  adt_list_destructorEnable(adt_list_t *self, bool enable);

void adt_list_insert(adt_list_t *self, void *pItem);
void adt_list_insertBefore(adt_list_t *self, adt_list_elem_t *pIter, void *pItem);
void adt_list_insertAfter(adt_list_t *self, adt_list_elem_t *pIter, void *pItem);
void adt_list_remove(adt_list_t *self, void *pItem);
bool adt_list_is_empty(const adt_list_t *self);
adt_list_elem_t *adt_list_first(const adt_list_t *self);
adt_list_elem_t *adt_list_last(const adt_list_t *self);
void adt_list_iter_init(adt_list_t *self);
adt_list_elem_t *adt_list_iter_next(adt_list_t *self);
int32_t adt_list_length(const adt_list_t *self);
void adt_list_clear(adt_list_t *self);

#endif //ADT_LIST_H
