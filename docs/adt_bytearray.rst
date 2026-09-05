Byte Array (adt_bytearray)
==========================

.. c:type:: adt_bytearray_t

   A mutable, dynamically resizable array of bytes, similar in design to ``QByteArray`` in Qt or ``std::vector<uint8_t>`` in C++.
   It manages an internal buffer of ``uint8_t`` and can automatically grow as data is appended.

Growth Policies
---------------

When initializing or constructing an ``adt_bytearray_t``, the ``u32GrowSize`` argument controls allocation behavior:

* **``0`` (or ``ADT_BYTEARRAY_NO_GROWTH``)**: The array allocates exactly the number of bytes currently required.
* **``> 0``**: When more capacity is needed, the array expands in chunks of ``u32GrowSize`` bytes (default: ``8192`` bytes) to minimize heap reallocations.

Memory Management
-----------------

The ADT library supports two allocation patterns across all types:

Stack / Embedded Allocation
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the ``adt_bytearray_t`` struct is allocated on the stack or embedded inside another struct, use :c:func:`adt_bytearray_create` and :c:func:`adt_bytearray_destroy`.

.. doxygenfunction:: adt_bytearray_create

.. doxygenfunction:: adt_bytearray_destroy

Example:

.. code-block:: c

   #include "adt_bytearray.h"

   void example_stack(void)
   {
       adt_bytearray_t array;
       adt_bytearray_create(&array, ADT_BYTEARRAY_DEFAULT_GROW_SIZE);

       adt_bytearray_push(&array, 0x42);

       adt_bytearray_destroy(&array);
   }

Heap Allocation
~~~~~~~~~~~~~~~

When allocating both the structure and its buffer on the heap, use :c:func:`adt_bytearray_new` and :c:func:`adt_bytearray_delete`.

.. doxygenfunction:: adt_bytearray_new

.. doxygenfunction:: adt_bytearray_delete

.. doxygenfunction:: adt_bytearray_vdelete

Constructors & Factories
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: adt_bytearray_make

.. doxygenfunction:: adt_bytearray_make_cstr

.. doxygenfunction:: adt_bytearray_clone

Capacity & Size
---------------

.. doxygenfunction:: adt_bytearray_length

.. doxygenfunction:: adt_bytearray_resize

.. doxygenfunction:: adt_bytearray_reserve

.. doxygenfunction:: adt_bytearray_grow

.. doxygenfunction:: adt_bytearray_setGrowthSize

Data Access
-----------

.. doxygenfunction:: adt_bytearray_data

.. doxygenfunction:: adt_bytearray_const_data

.. doxygenfunction:: adt_bytearray_bytes

Modification
------------

.. doxygenfunction:: adt_bytearray_append

.. doxygenfunction:: adt_bytearray_push

.. doxygenfunction:: adt_bytearray_trimLeft

.. doxygenfunction:: adt_bytearray_clear

Comparison
----------

.. doxygenfunction:: adt_bytearray_equals

.. doxygenfunction:: adt_bytearray_data_equals
