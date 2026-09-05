Linked List (adt_list)
=======================

.. c:type:: adt_list_t

   A doubly-linked list for generic object pointers (``void*``).
   It provides efficient :math:`O(1)` insertions and removals at any node, bidirectional iteration, and automatic element cleanup via a destructor callback (see :ref:`memory-destructors`).

Memory Management
-----------------

Stack / Embedded Allocation
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When embedding an ``adt_list_t`` instance inside another struct or on the stack, initialize it with :c:func:`adt_list_create` and release it with :c:func:`adt_list_destroy`.

.. doxygenfunction:: adt_list_create

.. doxygenfunction:: adt_list_destroy

Example:

.. code-block:: c

   #include <stdlib.h>
   #include <string.h>
   #include "adt_list.h"

   void example_list_stack(void)
   {
       adt_list_t list;
       adt_list_create(&list, free);

       adt_list_insert(&list, strdup("alpha"));
       adt_list_insert(&list, strdup("beta"));

       // Automatically calls free() on each element and releases all nodes
       adt_list_destroy(&list);
   }

Heap Allocation
~~~~~~~~~~~~~~~

When allocating both the list structure and its nodes on the heap, use :c:func:`adt_list_new` and free it with :c:func:`adt_list_delete`.

.. doxygenfunction:: adt_list_new

.. doxygenfunction:: adt_list_delete

.. doxygenfunction:: adt_list_vdelete

.. doxygenfunction:: adt_list_destructor_enable

Insertions
----------

.. doxygenfunction:: adt_list_insert

.. doxygenfunction:: adt_list_insert_before

.. doxygenfunction:: adt_list_insert_after

.. doxygenfunction:: adt_list_insert_unique

Removals
--------

.. doxygenfunction:: adt_list_remove

.. doxygenfunction:: adt_list_erase

.. doxygenfunction:: adt_list_clear

Inspection & Access
-------------------

.. doxygenfunction:: adt_list_first

.. doxygenfunction:: adt_list_last

.. doxygenfunction:: adt_list_length

.. doxygenfunction:: adt_list_is_empty

.. doxygenfunction:: adt_list_find

Iteration
---------

Bidirectional iteration through the list is supported using node pointers:

.. code-block:: c

   adt_list_elem_t *iter = adt_list_iter_first(&list);
   while (iter != NULL)
   {
       void *item = iter->pItem;
       // Process item...
       iter = adt_list_iter_next(iter);
   }

.. doxygenfunction:: adt_list_iter_first

.. doxygenfunction:: adt_list_iter_last

.. doxygenfunction:: adt_list_iter_next

.. doxygenfunction:: adt_list_iter_prev


Integer List (adt_u32List)
==========================

.. c:type:: adt_u32List_t

   A specialized doubly-linked list for storing unboxed 32-bit unsigned integers (``uint32_t``) directly in list nodes without generic pointer wrapping or separate heap allocations for elements.

Lifecycle
---------

.. doxygenfunction:: adt_u32List_create

.. doxygenfunction:: adt_u32List_destroy

.. doxygenfunction:: adt_u32List_new

.. doxygenfunction:: adt_u32List_delete

.. doxygenfunction:: adt_u32List_vdelete

Operations
----------

.. doxygenfunction:: adt_u32List_insert

.. doxygenfunction:: adt_u32List_insert_before

.. doxygenfunction:: adt_u32List_insert_after

.. doxygenfunction:: adt_u32List_erase

.. doxygenfunction:: adt_u32List_clear

.. doxygenfunction:: adt_u32List_is_empty

.. doxygenfunction:: adt_u32List_length

.. doxygenfunction:: adt_u32List_find

.. doxygenfunction:: adt_u32List_iter_first

.. doxygenfunction:: adt_u32List_iter_last

.. doxygenfunction:: adt_u32List_iter_next

.. doxygenfunction:: adt_u32List_iter_prev
