# ADT

Abstract data types for the C language.

# General Information

## Virtual destructors

Most ADT structures manages objects (pointers to void). Generally you would want to delete all objects contained in a data structure when the data structure itself is destroyed.
This is accomplished in ADT by using what I call virtual destructor functions. Even though they are not the same as virtual destructors from C++, they sort of serve the same purpose.

In ADT (and projects using ADT) you will note that most objects has both a _delete-function and a _vdelete-function. 
The _vdelete-functions are virtual destructor and can generally be handed over as a function pointer to allow any ADT data structure to automatically delete its children when the data structure itself is destroyed.
In case you just want to maintain weak references in your ADT data structure, just hand it a NULL-pointer for the destructor argument in the ADT constructor.

In special situations, some ADT structures allow temporarily enabling/disabling of automatic object destruction. Look for _destructor_enable-functions on ADT data stuctures that support this mechanism.

## ADT in embedded projects

Some data structures in ADT are used in embedded development and does not require heap memory. Look for the column labeled "Requires malloc/free" to find out which ones you can use in your project.


# Arrays

Arrays are lists with C-compatible memory layout. They allow both index-based access and the four traditional list operations (push, pop, shift, unshift).
They are used when access takes place at either end of the list.

| Operation name  | Meaning                                         |
|-----------------|-------------------------------------------------|
| push            | Appends an element to the end of the list       |
| pop             | Removes the last element from the list          |
| shift           | Removes the first element from the list         |
| unshift         | Appends an element to the beginning of the list |

## Available data structures

| Name            | Header          | Storage type        | Requires malloc/free |
|-----------------|-----------------|---------------------|----------------------|
| adt_ary_t       | adt_ary.h       | Objects (void*)     | yes                  |
| adt_bytearray_t | adt_bytearray.h | Bytes (uint8_t)     | yes                  |

## Examples

### ADT Array

``` c
adt_ary_t *pArray = adt_ary_new(free);
adt_ary_push(pArray,strdup("elem1"));
char *pElem = (char*) adt_ary_pop(pArray);
adt_ary_delete(pArray);
free(pElem);
```

# Strings

ADT provides a string type which manages the memory of the string data. You can access the raw data as a C-string (pointer) at any time.

## Avaliable data structures

| Name            | Header          | Storage type        | Requires malloc/free |
|-----------------|-----------------|---------------------|----------------------|
| adt_str_t       | adt_str.h       | Characters (char*)  | yes                  |

## Examples

### ADT String

``` C
adt_str_t *str = adt_str_new();
adt_str_push(str,'a');
adt_str_push(str,'b');
adt_str_push(str,'c');
const char *result = adt_str_cstr(str); //returns "abc"
adt_str_delete(str);
```

# Maps

Maps are key-value pair containers. The key is usually a string which is converted to an integer using a hashing algorithm.

| Name            | Header          | Key type      | Storage type        | Requires malloc/free |
|-----------------|-----------------|---------------|---------------------|----------------------|
| adt_hash_t      | adt_hash.h      | String        | Objects (void*)     | yes                  |
| adt_u16Map_t    | adt_u16Map.h    | uint16_t      | Objects (void*)     | no                   |

## Examples

### ADT Hash
``` C
adt_hash_t *pHash = adt_hash_new(free);
adt_hash_set(pHash,"first",0,"The");
adt_hash_set(pHash,"second",0,"quick");
adt_hash_set(pHash,"third",0,"brown");
adt_hash_set(pHash,"fourth",0,"fox");
const char *pVal = adt_hash_get(pHash,"third",0);
```

# Linked Lists

Linked lists are used when insertions/deletions of elements occur in the middle of the list.

## Available data structures

| Name            | Header          | Storage type        | Requires malloc/free |
|-----------------|-----------------|---------------------|----------------------|
| adt_list_t      | adt_list.h      | Objects (void*)     | yes                  |
| adt_u32List_t   | adt_list.h      | Values (uint32_t)   | yes                  |


# Stacks

Stacks provides a first in, first out (FIFO) queue. It supports the traditional operations push, pop (and top).

## Avaliable data structures

| Name            | Header          | Storage type        | Requires malloc/free |
|-----------------|-----------------|---------------------|----------------------|
| adt_stack_t     | adt_stack.h     | Objects (void*)     | yes                  |

##Examples

### ADT Stack

``` C
adt_stack_t *pStack = adt_stack_new(free);
adt_stack_push(pStack,strdup("The"));
adt_stack_push(pStack,strdup("quick"));
adt_stack_push(pStack,strdup("brown"));
adt_stack_push(pStack,strdup("fox"));
const char *pVal = adt_stack_top(pStack);
adt_stack_delete(pStack);
```

# Sets

Sets are unordered containers of values. Each value can only exist once in the set.
Note that ADT sets are severly under-developed and lacks general set-theory operations at the moment.

## Avaliable data structures

| Name            | Header          | Storage type        | Requires malloc/free |
|-----------------|-----------------|---------------------|----------------------|
| adt_u32Set_t    | adt_set.h       | Values (uint32_t)   | yes                  |


# Heaps

Heaps are tree-based data structures. ADT supports them for the sole purpose of allowing implementation of priority queues.

## Avaible data structures

| Name            | Header          | Storage type        | Requires malloc/free |
|-----------------|-----------------|---------------------|----------------------|
| adt_heap_t      | adt_heap.h      | Objects (void*)     | yes                  |


# Ring Buffers

Ring buffers (or circular buffers) are FIFO-queues (First In First Out). They store elements (usually struct containers) inside its internal buffer.
Each element must be of the same size (an argument given in the ringbuffer constructor). Note that ADT ring buffers copies entire data blocks (elements) instead of just pointers to objects.
Ring buffer are especially suitable for use in embedded projects where (memory is a concern).

## Avaliable data structures
| Name            | Header          | Storage type        | Requires malloc/free |
|-----------------|-----------------|---------------------|----------------------|
| adt_rbfh_t      | adt_ringbuf.h   | Elements (uint8_t*) | yes                  |
| adt_rbfs_t      | adt_ringbuf.h   | Elelemts (uint8_t*) | no                   |
| adt_rbfu16_t    | adt_ringbuf.h   | Values (uint16_t)   | no                   |
