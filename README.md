# ADT


Abstract data types for the C language

# Overview


| Name            | Header          | Type of data structure      | Storage type        | Requires malloc/free |
|-----------------|-----------------|-----------------------------|---------------------|----------------------|
| adt_ary_t       | adt_ary.h       | Array                       | Objects (void*)     | yes                  |
| adt_bytearray_t | adt_bytearray.h | Array                       | Bytes (uint8_t)     | yes                  |
| adt_hash_t      | adt_hash.h      | Key-value storage           | Objects (void*)     | yes                  |
| adt_heap_t      | adt_heap.h      | (Min/Max) Heap              | Objects (void*)     | yes                  |
| adt_list_t      | adt_list.h      | Linked list                 | Objects (void*)     | yes                  |
| adt_rbfh_t      | adt_ringbuf.h   | Ring buffer (heap memory)   | Elements (uint8_t*) | yes                  |
| adt_rbfs_t      | adt_ringbuf.h   | Ring buffer (static memory) | Elelemts (uint8_t*) | no                   |
| adt_rbfu16_t    | adt_ringbuf.h   | Ring buffer (static memory) | Values (uint16_t)   | no                   |
| adt_u32Set_t    | adt_set.h       | Set                         | Values (uint32_t)   | yes                  |
| adt_stack_t     | adt_stack.h     | Stack                       | Objects (void*)     | yes                  |
| adt_str_t       | adt_str.h       | String                      | Characters (char*)  | yes                  |
| adt_u16Map_t    | adt_u16Map.h    | Key-value storage           | Objects (void*)     | no                   |

# Example Usage


## ADT Array
``` c
adt_ary_t *pArray = adt_ary_new(free);
adt_ary_push(pArray,strdup("elem1"));
char *pElem = (char*) adt_ary_pop(pArray);
adt_ary_delete(pArray);
free(pElem);
```

## ADT Hash
``` C
adt_hash_t *pHash = adt_hash_new(free);
adt_hash_set(pHash,"first",0,"The");
adt_hash_set(pHash,"second",0,"quick");
adt_hash_set(pHash,"third",0,"brown");
adt_hash_set(pHash,"fourth",0,"fox");
const char *pVal = adt_hash_get(pHash,"third",0);
```

## ADT Stack
``` C
adt_stack_t *pStack = adt_stack_new(free);
adt_stack_push(pStack,strdup("The"));
adt_stack_push(pStack,strdup("quick"));
adt_stack_push(pStack,strdup("brown"));
adt_stack_push(pStack,strdup("fox"));
const char *pVal = adt_stack_top(pStack);
adt_stack_delete(pStack);
```

## ADT String
``` C
adt_str_t *str = adt_str_new();
adt_str_push(str,'a');
adt_str_push(str,'b');
adt_str_push(str,'c');
const char *result = adt_str_cstr(str); //returns "abc"
adt_str_delete(str);
```
