Integer Set (adt_set)
=====================

.. c:type:: adt_u32Set_t

   Set container for unique 32-bit unsigned integers (``uint32_t``).

The **adt_set** module provides a set container that stores unique 32-bit unsigned integers (``uint32_t``) in ascending sorted order.

Key features:

* **Automatic Deduplication**: Duplicate values are ignored upon insertion (returning ``ADT_NO_ERROR``), ensuring each integer appears at most once.
* **Sorted Invariant**: Elements are maintained in sorted ascending order, enabling early-exit optimizations when searching or removing elements.
* **Dual Lifecycle Management**: Supports both stack/in-place initialization (:c:func:`adt_u32Set_create` / :c:func:`adt_u32Set_destroy`) and dynamic heap allocation (:c:func:`adt_u32Set_new` / :c:func:`adt_u32Set_delete` / :c:func:`adt_u32Set_vdelete`).
* **Container Compatibility**: Provides :c:func:`adt_u32Set_vdelete` for seamless cleanup when sets are nested inside generic ADT pointer containers (such as :c:type:`adt_ary_t` or :c:type:`adt_hash_t`).

Code Example
------------

.. code-block:: c

   #include <stdio.h>
   #include <stdint.h>
   #include "adt_set.h"

   void example_u32_set(void)
   {
       adt_u32Set_t set;
       adt_u32Set_create(&set);

       // Insert values (duplicates return ADT_NO_ERROR without adding)
       adt_u32Set_insert(&set, 42);
       adt_u32Set_insert(&set, 10);
       adt_u32Set_insert(&set, 99);
       adt_u32Set_insert(&set, 42); // Duplicate: returns ADT_NO_ERROR

       printf("Set contains %d elements\n", adt_u32Set_length(&set)); // Prints 3

       // Membership testing
       if (adt_u32Set_contains(&set, 10))
       {
           printf("10 is in the set\n");
       }

       if (!adt_u32Set_contains(&set, 50))
       {
           printf("50 is not in the set\n");
       }

       // Remove an element
       if (adt_u32Set_remove(&set, 10))
       {
           printf("Removed 10 from set\n");
       }

       // Check if empty
       printf("Set empty? %s\n", adt_u32Set_is_empty(&set) ? "yes" : "no");

       // Clean up
       adt_u32Set_destroy(&set);
   }

Lifecycle Functions
-------------------

.. doxygenfunction:: adt_u32Set_create

.. doxygenfunction:: adt_u32Set_destroy

.. doxygenfunction:: adt_u32Set_new

.. doxygenfunction:: adt_u32Set_delete

.. doxygenfunction:: adt_u32Set_vdelete

Set Operations & Queries
------------------------

.. doxygenfunction:: adt_u32Set_insert

.. doxygenfunction:: adt_u32Set_remove

.. doxygenfunction:: adt_u32Set_contains

.. doxygenfunction:: adt_u32Set_length

.. doxygenfunction:: adt_u32Set_is_empty

.. doxygenfunction:: adt_u32Set_clear
