Hash Table (adt_hash)
======================

.. c:type:: adt_hash_t

   A hash table container mapping null-terminated string keys to generic object pointers (``void*``).
   It supports fast key-based insertions, lookups, removals, iteration, and automatic element destruction upon cleanup (see :ref:`memory-destructors`).

Memory Management
-----------------

Stack / Embedded Allocation
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When embedding an ``adt_hash_t`` instance inside another struct or on the stack, initialize it with :c:func:`adt_hash_create` and release it with :c:func:`adt_hash_destroy`.

.. doxygenfunction:: adt_hash_create

.. doxygenfunction:: adt_hash_destroy

Example:

.. code-block:: c

   #include <stdlib.h>
   #include <string.h>
   #include "adt_hash.h"

   void example_hash_stack(void)
   {
       adt_hash_t table;
       adt_hash_create(&table, free);

       adt_hash_set(&table, "hostname", strdup("localhost"));
       adt_hash_set(&table, "port", strdup("8080"));

       const char *host = (const char*) adt_hash_value(&table, "hostname");

       // Automatically calls free() on each value and releases table memory
       adt_hash_destroy(&table);
   }

Heap Allocation
~~~~~~~~~~~~~~~

When allocating both the structure and its nodes on the heap, use :c:func:`adt_hash_new` and free it with :c:func:`adt_hash_delete`. For nested containers, :c:func:`adt_hash_vdelete` is available as a type-erased destructor.

.. doxygenfunction:: adt_hash_new

.. doxygenfunction:: adt_hash_delete

.. doxygenfunction:: adt_hash_vdelete

Clearing
~~~~~~~~

.. doxygenfunction:: adt_hash_clear

Key-Value Accessors
-------------------

.. doxygenfunction:: adt_hash_set

.. doxygenfunction:: adt_hash_insert

.. doxygenfunction:: adt_hash_get

.. doxygenfunction:: adt_hash_value

.. doxygenfunction:: adt_hash_remove

.. doxygenfunction:: adt_hash_erase

.. doxygenfunction:: adt_hash_exists

Iteration
---------

To iterate over all key-value pairs stored in the hash table, call :c:func:`adt_hash_iter_init` followed by repeated calls to :c:func:`adt_hash_iter_next`:

.. code-block:: c

   const char *key = NULL;
   void **val_ptr = NULL;

   adt_hash_iter_init(&table);
   while ((val_ptr = adt_hash_iter_next(&table, &key)) != NULL)
   {
       void *value = *val_ptr;
       // Process key and value...
   }

.. doxygenfunction:: adt_hash_iter_init

.. doxygenfunction:: adt_hash_iter_next

Alternatively, :c:func:`adt_hash_foreach` can be used to iterate over a map (including a ``const adt_hash_t*``) using a callback:

.. doxygenfunction:: adt_hash_foreach

Utility & Bulk Extraction
-------------------------

Both :c:func:`adt_hash_keys` and :c:func:`adt_hash_values` populate the destination array with non-owning (borrowed) weak references.
The destination ``adt_ary_t`` must not have an active element destructor enabled (or must have it disabled via :c:func:`adt_ary_destructor_enable`).

.. doxygenfunction:: adt_hash_length

.. doxygenfunction:: adt_hash_is_empty

.. doxygenfunction:: adt_hash_keys

.. doxygenfunction:: adt_hash_values
