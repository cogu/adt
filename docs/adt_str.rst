String (adt_str)
=================

.. c:type:: adt_str_t

   A mutable dynamic string container.
   It manages an internal buffer of characters, supports ASCII and UTF-8 encodings, and can be accessed as a standard null-terminated C string at any time.

Memory Management
-----------------

Stack / Embedded Allocation
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When embedding an ``adt_str_t`` instance inside another struct or on the stack, initialize it with :c:func:`adt_str_create` and release it with :c:func:`adt_str_destroy`.

.. doxygenfunction:: adt_str_create

.. doxygenfunction:: adt_str_destroy

Example:

.. code-block:: c

   #include <stdio.h>
   #include "adt_str.h"

   void example_str_stack(void)
   {
       adt_str_t str;
       adt_str_create(&str);

       adt_str_set_cstr(&str, "Hello, ");
       adt_str_append_cstr(&str, "world!");

       printf("%s\n", adt_str_cstr(&str)); // prints "Hello, world!"

       adt_str_destroy(&str);
   }

Heap Allocation & Factories
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When allocating strings on the heap, use :c:func:`adt_str_new` (or one of its factory functions) and release it with :c:func:`adt_str_delete`.

.. doxygenfunction:: adt_str_new

.. doxygenfunction:: adt_str_new_utf8

.. doxygenfunction:: adt_str_new_cstr

.. doxygenfunction:: adt_str_new_bstr

.. doxygenfunction:: adt_str_new_bytearray

.. doxygenfunction:: adt_str_clone

.. doxygenfunction:: adt_str_concat

.. doxygenfunction:: adt_str_delete

.. doxygenfunction:: adt_str_vdelete

String Manipulation
-------------------

Assignment
~~~~~~~~~~

.. doxygenfunction:: adt_str_set

.. doxygenfunction:: adt_str_set_cstr

.. doxygenfunction:: adt_str_set_bstr

Appending & Prepending
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: adt_str_append

.. doxygenfunction:: adt_str_append_cstr

.. doxygenfunction:: adt_str_append_bstr

.. doxygenfunction:: adt_str_prepend

.. doxygenfunction:: adt_str_prepend_cstr

.. doxygenfunction:: adt_str_prepend_bstr

Character Operations
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: adt_str_push

.. doxygenfunction:: adt_str_pop

Access & Conversion
-------------------

.. doxygenfunction:: adt_str_cstr

.. doxygenfunction:: adt_str_data

.. doxygenfunction:: adt_str_bstr

.. doxygenfunction:: adt_str_charAt

.. doxygenfunction:: adt_str_bytearray

.. doxygenfunction:: adt_str_bytes

Encoding & Capacity
-------------------

.. doxygenfunction:: adt_str_setEncoding

.. doxygenfunction:: adt_str_getEncoding

.. doxygenfunction:: adt_str_length

.. doxygenfunction:: adt_str_size

.. doxygenfunction:: adt_str_is_empty

.. doxygenfunction:: adt_str_reserve

.. doxygenfunction:: adt_str_clear

.. doxygenfunction:: adt_str_get_last_error

Comparisons & Ordering
----------------------

.. doxygenfunction:: adt_str_equal

.. doxygenfunction:: adt_str_equal_cstr

.. doxygenfunction:: adt_str_equal_bstr

.. doxygenfunction:: adt_str_lt

.. doxygenfunction:: adt_str_vlt
