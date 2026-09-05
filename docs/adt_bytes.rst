Bytes (adt_bytes)
==================

.. c:type:: adt_bytes_t

   An immutable byte array container. Once created, the byte contents cannot be modified.
   For a dynamically resizable, mutable byte buffer, see :doc:`adt_bytearray`.

Memory Management
-----------------

Stack / Embedded Allocation
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When embedding an ``adt_bytes_t`` instance inside another struct or on the stack, initialize it with :c:func:`adt_bytes_create` and clean it up with :c:func:`adt_bytes_destroy`.

.. doxygenfunction:: adt_bytes_create

.. doxygenfunction:: adt_bytes_destroy

Example:

.. code-block:: c

   #include "adt_bytes.h"

   void example_stack(const uint8_t *payload, uint32_t len)
   {
       adt_bytes_t bytes;
       if (adt_bytes_create(&bytes, payload, len) == ADT_NO_ERROR)
       {
           // Inspect bytes...
           adt_bytes_destroy(&bytes);
       }
   }

Heap Allocation
~~~~~~~~~~~~~~~

When dynamically allocating the bytes object on the heap, use :c:func:`adt_bytes_new`, :c:func:`adt_bytes_new_cstr`, or :c:func:`adt_bytes_clone`, and free it with :c:func:`adt_bytes_delete`.

.. doxygenfunction:: adt_bytes_new

.. doxygenfunction:: adt_bytes_new_cstr

.. doxygenfunction:: adt_bytes_clone

.. doxygenfunction:: adt_bytes_delete

.. doxygenfunction:: adt_bytes_vdelete

Inspection & Data Access
------------------------

.. doxygenfunction:: adt_bytes_length

.. doxygenfunction:: adt_bytes_const_data

Conversion
----------

.. doxygenfunction:: adt_bytes_bytearray

Comparison
----------

.. doxygenfunction:: adt_bytes_equals
