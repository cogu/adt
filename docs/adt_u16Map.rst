U16 Key-Value Map (adt_u16Map)
==============================

.. c:type:: adt_u16MapElem_t

   A key/value entry pair combining a 16-bit unsigned integer key (``uint16_t``) with a generic pointer value (``void*``).

.. c:type:: adt_u16Map_t

   Sorted-array key/value map container. Maintains its elements in ascending sorted order by key.

The **adt_u16Map** module provides a compact, cache-friendly key-value dictionary using a contiguous sorted array as its underlying storage. It is indexed by 16-bit unsigned integer keys (``uint16_t``) and stores generic object pointers (``void*``).

Key features:

* **Zero-Heap Operation**: Can be initialized with caller-supplied array memory (:c:func:`adt_u16Map_create`), making it completely safe for embedded systems where heap allocation is prohibited.
* **Dynamic Heap Mode**: Can alternatively be allocated on the heap (:c:func:`adt_u16Map_new`) with automatic resizing up to the specified element limit.
* **Fast Binary Search**: Lookups (:c:func:`adt_u16Map_find` and :c:func:`adt_u16Map_findExact`) operate in ``O(log N)`` time using binary search.
* **Duplicate Key Support**: Multiple entries with identical keys but different values can coexist within the map.
* **Memory Ownership & Destructors**: Integrates with the ADT destructor pattern to automatically clean up dynamically allocated values when entries are removed or the map is destroyed.

CMake Configuration
-------------------

``adt_u16Map_t`` is an optional component and is disabled by default to minimize the compiled footprint of the library on small targets.

To enable it in your build:

.. code-block:: sh

   cmake -B build -DADT_U16MAP_ENABLE=ON

In your ``CMakeLists.txt``:

.. code-block:: cmake

   set(ADT_U16MAP_ENABLE ON CACHE BOOL "Enable ADT U16 Map" FORCE)

Code Examples
-------------

Example 1: Static Lookup Table (Zero Heap)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   #include <stdio.h>
   #include <stdint.h>
   #include "adt_u16Map.h"

   #define MAX_ENTRIES 10

   void example_static_map(void)
   {
       adt_u16Map_t map;
       adt_u16MapElem_t storage[MAX_ENTRIES];

       // Initialize using caller-provided storage (zero heap allocation)
       adt_u16Map_create(&map, storage, MAX_ENTRIES, NULL);

       // Insert key/value pairs in arbitrary order
       adt_u16Map_insert(&map, 400, "CAN Service");
       adt_u16Map_insert(&map, 100, "Diagnostics");
       adt_u16Map_insert(&map, 250, "Telemetry");

       // Lookup using binary search (O(log N))
       adt_u16MapElem_t *elem = adt_u16Map_find(&map, 250);
       if (elem != NULL)
       {
           printf("Key %u found: %s\n", elem->key, (const char*) elem->val);
       }

       // Iterate in ascending key order (100 -> 250 -> 400)
       for (adt_u16MapElem_t *it = adt_u16Map_iterInit(&map, NULL);
            it != NULL;
            it = adt_u16Map_iterNext(&map))
       {
           printf("ID %u -> %s\n", it->key, (const char*) it->val);
       }

       // Clean up (does not free storage array)
       adt_u16Map_destroy(&map);
   }

Example 2: Dynamic Allocation with Destructors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   #include <stdio.h>
   #include <stdlib.h>
   #include "adt_str.h"
   #include "adt_u16Map.h"

   void example_dynamic_map(void)
   {
       // Allocate map on the heap with a virtual destructor for adt_str_t values
       adt_u16Map_t *map = adt_u16Map_new(50, adt_str_vdelete);
       if (map == NULL) return;

       // Insert dynamically allocated strings
       adt_u16Map_insert(map, 10, adt_str_new_cstr("Alpha"));
       adt_u16Map_insert(map, 20, adt_str_new_cstr("Beta"));

       printf("Map contains %u entries\n", adt_u16Map_size(map));

       // Deleting the map automatically calls adt_str_vdelete on all remaining values
       adt_u16Map_delete(map);
   }

Lifecycle & Memory Management
-----------------------------

.. doxygenfunction:: adt_u16Map_create

.. doxygenfunction:: adt_u16Map_destroy

.. doxygenfunction:: adt_u16Map_new

.. doxygenfunction:: adt_u16Map_delete

.. doxygenfunction:: adt_u16Map_destructorEnable

Operations & Query
------------------

.. doxygenfunction:: adt_u16Map_insert

.. doxygenfunction:: adt_u16Map_remove

.. doxygenfunction:: adt_u16Map_find

.. doxygenfunction:: adt_u16Map_findExact

.. doxygenfunction:: adt_u16Map_size

.. doxygenfunction:: adt_u16Map_clear

.. doxygenfunction:: adt_u16Map_removeVal

.. doxygenfunction:: adt_u16Map_moveElem

Iteration
---------

.. doxygenfunction:: adt_u16Map_iterInit

.. doxygenfunction:: adt_u16Map_iterNext
