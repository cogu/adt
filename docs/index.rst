Welcome to ADT's Documentation
==============================

**ADT** provides platform-independent and compiler-independent abstract data types for the C programming language (C99 and later).

It offers a comprehensive suite of container data structures ranging from generic pointer containers (``adt_ary_t``, ``adt_hash_t``, ``adt_list_t``, ``adt_stack_t``) to byte buffers (``adt_bytearray_t``, ``adt_bytes_t``), strings (``adt_str_t``), and memory-constrained structures optimized for embedded systems.

.. toctree::
   :maxdepth: 2
   :hidden:
   :caption: API Reference

   adt_bytearray
   adt_bytes
   adt_ary
   adt_str
   adt_hash
   adt_stack
   adt_list
   adt_heap
   adt_ringbuf
   adt_u16Map


Data Structures Catalog
=======================

Below is a summary of all data structures provided by the ADT library:

.. list-table::
   :header-rows: 1
   :widths: 20 20 25 15 20

   * - Name
     - Header
     - Storage Type
     - Requires Heap
     - Description
   * - :c:type:`adt_ary_t`
     - ``adt_ary.h``
     - Objects (``void*``)
     - Yes
     - Resizable pointer array with C-contiguous layout and O(1) random access.
   * - :c:type:`adt_bytearray_t`
     - ``adt_bytearray.h``
     - Bytes (``uint8_t``)
     - Yes
     - Mutable, resizable byte array with configurable growth chunking.
   * - :c:type:`adt_bytes_t`
     - ``adt_bytes.h``
     - Bytes (``uint8_t``)
     - Yes
     - Immutable byte array buffer.
   * - :c:type:`adt_str_t`
     - ``adt_str.h``
     - Characters (``char*``)
     - Yes
     - Dynamic string container managing UTF-8 and ASCII text.
   * - :c:type:`adt_hash_t`
     - ``adt_hash.h``
     - Objects (``void*``)
     - Yes
     - Hash table mapping null-terminated string keys to generic pointers.
   * - :doc:`adt_u16Map <adt_u16Map>`
     - ``adt_u16Map.h``
     - Objects (``void*``)
     - No
     - Sorted-array key/value map using 16-bit integer keys.
   * - :c:type:`adt_list_t`
     - ``adt_list.h``
     - Objects (``void*``)
     - Yes
     - Doubly-linked list for fast mid-sequence insertions and removals.
   * - :c:type:`adt_u32List_t`
     - ``adt_list.h``
     - Values (``uint32_t``)
     - Yes
     - Specialized linked list for unsigned 32-bit integers.
   * - :c:type:`adt_stack_t`
     - ``adt_stack.h``
     - Objects (``void*``)
     - Yes
     - LIFO (Last-In First-Out) stack for generic pointers.
   * - :doc:`adt_heap <adt_heap>`
     - ``adt_heap.h``
     - Objects (``void*``)
     - Yes
     - Binary heap priority queue algorithms operating on :c:type:`adt_ary_t`.
   * - :doc:`adt_rbfh_t <adt_ringbuf>`
     - ``adt_ringbuf.h``
     - Elements (``uint8_t*``)
     - Yes
     - Circular FIFO buffer with heap-allocated storage.
   * - :doc:`adt_rbfs_t <adt_ringbuf>`
     - ``adt_ringbuf.h``
     - Elements (``uint8_t*``)
     - No
     - Static circular FIFO buffer requiring zero heap allocation.
   * - :doc:`adt_rbfu16_t <adt_ringbuf>`
     - ``adt_ringbuf.h``
     - Values (``uint16_t``)
     - No
     - Embedded circular buffer for 16-bit integer values.
   * - ``adt_u32Set_t``
     - ``adt_set.h``
     - Values (``uint32_t``)
     - Yes
     - Unordered set of unique 32-bit unsigned integers.


Embedded & Zero-Heap Support
----------------------------

Data structures marked with **Requires Heap: No** (such as ``adt_rbfs_t``, ``adt_rbfu16_t``, and ``adt_u16Map_t``) or those supporting stack initialization (``*_create`` / ``*_destroy`` patterns without dynamic reallocation) can be safely used in memory-constrained and hard real-time embedded environments where dynamic heap allocation (``malloc`` / ``free``) is prohibited.


.. _memory-destructors:

Memory Ownership & Virtual Destructors
======================================

Because C lacks language-level destructors (such as C++ RAII), generic containers holding pointers (`void*`) face an ownership dilemma: who is responsible for freeing the elements when the container itself is cleared or destroyed?

The ADT library solves this cleanly through **destructor function pointers** (often referred to as *virtual destructors*).

The `void (*)(void*)` Callback
------------------------------

When creating a container that holds generic pointers, you pass a destructor callback function matching the signature:

.. code-block:: c

   void destructor(void *ptr);

When the container deletes an element, pops an item, or is destroyed itself, it automatically invokes this callback on each removed object.

The Dual-Destructor Convention
------------------------------

Every ADT type that manages heap memory provides two destructor functions:

1. **Typed Destructor** (e.g. ``adt_bytearray_delete(adt_bytearray_t *self)``):
   Used directly when managing an instance in your own code.
2. **Virtual Destructor Wrapper** (e.g. ``adt_bytearray_vdelete(void *arg)``):
   A type-erased wrapper taking ``void*``, making it a drop-in destructor callback for other ADT containers.

Using Standard Library `free`
-----------------------------

The C standard library ``free(void *ptr)`` exactly matches the destructor callback signature. Any container storing dynamically allocated memory (such as ``malloc``'d structs or strings created with ``strdup``) can directly pass ``free``:

.. code-block:: c

   #include <stdlib.h>
   #include <string.h>
   #include "adt_ary.h"

   void example_owning_strings(void)
   {
       // Array automatically frees all strings on destruction
       adt_ary_t *array = adt_ary_new(free);

       adt_ary_push(array, strdup("alpha"));
       adt_ary_push(array, strdup("beta"));
       adt_ary_push(array, strdup("gamma"));

       // Frees each strdup'd string, then frees the array
       adt_ary_delete(array);
   }

Nested Containers with `vdelete`
--------------------------------

When nesting ADT containers (such as an array of byte arrays or a list of hash tables), supply the inner type's ``vdelete`` function as the destructor:

.. code-block:: c

   #include "adt_ary.h"
   #include "adt_bytearray.h"

   void example_nested_containers(void)
   {
       // Array takes ownership of child byte arrays via adt_bytearray_vdelete
       adt_ary_t *array = adt_ary_new(adt_bytearray_vdelete);

       adt_bytearray_t *ba1 = adt_bytearray_make_cstr("First Buffer", 0);
       adt_bytearray_t *ba2 = adt_bytearray_make_cstr("Second Buffer", 0);

       adt_ary_push(array, ba1);
       adt_ary_push(array, ba2);

       // Deletes both byte arrays and their internal buffers, then frees the array
       adt_ary_delete(array);
   }

Weak / Non-Owning References
----------------------------

If you want a container to maintain weak references without taking ownership of the underlying data, simply pass ``NULL`` as the destructor argument. The container will store the pointers, but destroying the container will leave the referenced memory untouched:

.. code-block:: c

   #include "adt_ary.h"

   typedef struct node {
       int id;
   } node_t;

   void example_non_owning(node_t *stack_nodes, int count)
   {
       // Pass NULL: container will not free any elements
       adt_ary_t *refs = adt_ary_new(NULL);

       for (int i = 0; i < count; ++i)
       {
           adt_ary_push(refs, &stack_nodes[i]);
       }

       // Array structure is freed, but stack_nodes remain valid
       adt_ary_delete(refs);
   }

Temporary Destructor Suspension
-------------------------------

Certain containers (such as :c:type:`adt_ary_t`) allow temporarily suspending the destructor via ``adt_ary_destructor_enable(self, false)``. This is useful when moving elements from one container to another without triggering unintended deallocations.
