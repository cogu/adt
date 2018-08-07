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

#define ADT_LIST_VERSION 0.2.0

#define ADT_LIST_ELEM_HEAD(struct_name, item_type_name) \
      struct struct_name *pNext;\
      struct struct_name *pPrev;\
      item_type_name item;\

#define ADT_LIST_HEAD(type_name) \
      type_name *pFirst;\
      type_name *pLast;\


typedef struct adt_list_elem_tag
{
   struct adt_list_elem_tag *pNext;
   struct adt_list_elem_tag *pPrev;
   void *pItem;
}adt_list_elem_t;

typedef struct adt_u32List_elem_tag
{
   ADT_LIST_ELEM_HEAD(adt_u32List_elem_tag, uint32_t)
}adt_u32List_elem_t;

typedef struct adt_list_tag{
   ADT_LIST_HEAD(adt_list_elem_t)
   void (*pDestructor)(void*);
   bool destructorEnable;
} adt_list_t;

typedef struct adt_u32List_tag{
   ADT_LIST_HEAD(adt_u32List_elem_t)
} adt_u32List_t;


/***************** Public Function Declarations *******************/

/******* adt_list API *************/
void  adt_list_create(adt_list_t *self, void (*pDestructor)(void*));
void  adt_list_destroy(adt_list_t *self);
adt_list_t*  adt_list_new(void (*pDestructor)(void*));
void  adt_list_delete(adt_list_t *self);
void  adt_list_vdelete(void *arg);
void  adt_list_destructor_enable(adt_list_t *self, bool enable);
void adt_list_insert(adt_list_t *self, void *pItem);
void adt_list_insert_before(adt_list_t *self, adt_list_elem_t *pIter, void *pItem);
void adt_list_insert_after(adt_list_t *self, adt_list_elem_t *pIter, void *pItem);
void adt_list_insert_unique(adt_list_t *self, void *pItem);
bool adt_list_remove(adt_list_t *self, void *pItem);
bool adt_list_is_empty(const adt_list_t *self);
void *adt_list_first(const adt_list_t *self);
void *adt_list_last(const adt_list_t *self);
adt_list_elem_t *adt_list_iter_first(adt_list_t *self);
adt_list_elem_t *adt_list_iter_last(adt_list_t *self);
adt_list_elem_t *adt_list_iter_next(adt_list_elem_t *pElem);
adt_list_elem_t *adt_list_iter_prev(adt_list_elem_t *pElem);
int32_t adt_list_length(const adt_list_t *self);
void adt_list_clear(adt_list_t *self);

/******* adt_u32List API *************/
void  adt_u32List_create(adt_u32List_t *self);
void  adt_u32List_destroy(adt_u32List_t *self);
adt_u32List_t*  adt_u32List_new(void);
void  adt_u32List_delete(adt_u32List_t *self);
void  adt_u32List_vdelete(void *arg);
void adt_u32List_insert(adt_u32List_t *self, uint32_t item);
void adt_u32List_insert_before(adt_u32List_t *self, adt_u32List_elem_t *pIter, uint32_t item);
void adt_u32List_insert_after(adt_u32List_t *self, adt_u32List_elem_t *pIter, uint32_t item);
void adt_u32List_erase(adt_u32List_t *self, adt_u32List_elem_t *pIter);
bool adt_u32List_is_empty(const adt_u32List_t *self);
adt_u32List_elem_t *adt_u32List_iter_first(const adt_u32List_t *self);
adt_u32List_elem_t *adt_u32List_iter_last(const adt_u32List_t *self);
adt_u32List_elem_t* adt_u32List_iter_next(adt_u32List_elem_t *pElem);
adt_u32List_elem_t* adt_u32List_iter_prev(adt_u32List_elem_t *pElem);
int32_t adt_u32List_length(const adt_u32List_t *self);
void adt_u32List_clear(adt_u32List_t *self);


#endif //ADT_LIST_H
