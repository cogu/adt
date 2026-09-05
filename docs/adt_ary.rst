Pointer Array (adt_ary)
========================

.. c:type:: adt_ary_t

   A dynamically resizable, C-contiguous array of generic object pointers (``void*``).
   It provides :math:`O(1)` random access by index, supports negative indices (e.g. ``-1`` for the last element), and includes list operations (push, pop, shift, unshift).

   It can automatically manage the lifecycle of stored elements via a destructor callback (see :ref:`memory-destructors`).

Memory Management
-----------------

Stack / Embedded Allocation
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When embedding an ``adt_ary_t`` instance inside another struct or on the stack, initialize it with :c:func:`adt_ary_create` and release it with :c:func:`adt_ary_destroy`.

.. doxygenfunction:: adt_ary_create

.. doxygenfunction:: adt_ary_destroy

Example:

.. code-block:: c

   #include <stdlib.h>
   #include <string.h>
   #include "adt_ary.h"

   void example_stack_array(void)
   {
       adt_ary_t array;
       adt_ary_create(&array, free);

       adt_ary_push(&array, strdup("Hello"));
       adt_ary_push(&array, strdup("World"));

       // Destroys each string with free(), then releases internal buffer
       adt_ary_destroy(&array);
   }

Heap Allocation
~~~~~~~~~~~~~~~

When allocating both the structure and its element buffer on the heap, use :c:func:`adt_ary_new` or :c:func:`adt_ary_make`, and clean up with :c:func:`adt_ary_delete`.

.. doxygenfunction:: adt_ary_new

.. doxygenfunction:: adt_ary_make

.. doxygenfunction:: adt_ary_delete

.. doxygenfunction:: adt_ary_vdelete

Destructor Control
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: adt_ary_destructor_enable

.. doxygenfunction:: adt_ary_has_destructor

.. doxygenfunction:: adt_ary_destructor_is_enabled

List Operations (Insertion & Removal)
-------------------------------------

The array supports standard double-ended queue operations (push/pop at the back, unshift/shift at the front).

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Operation
     - Description
   * - :c:func:`adt_ary_push`
     - Appends an element to the end of the array.
   * - :c:func:`adt_ary_pop`
     - Removes and returns the last element without invoking its destructor.
   * - :c:func:`adt_ary_shift`
     - Removes and returns the first element without invoking its destructor.
   * - :c:func:`adt_ary_unshift`
     - Inserts an element at the beginning, shifting all existing elements forward.

.. doxygenfunction:: adt_ary_push

.. doxygenfunction:: adt_ary_push_unique

.. doxygenfunction:: adt_ary_pop

.. doxygenfunction:: adt_ary_shift

.. doxygenfunction:: adt_ary_unshift

.. doxygenfunction:: adt_ary_remove

Element Access
--------------

Indices can be positive (zero-based from the beginning) or negative (counting from the end, where ``-1`` is the last element).

.. doxygenfunction:: adt_ary_get

.. doxygenfunction:: adt_ary_set

.. doxygenfunction:: adt_ary_value

.. doxygenfunction:: adt_ary_exists

.. doxygenfunction:: adt_ary_index_of

Sizing & Capacity
-----------------

.. doxygenfunction:: adt_ary_length

.. doxygenfunction:: adt_ary_is_empty

.. doxygenfunction:: adt_ary_resize

.. doxygenfunction:: adt_ary_extend

.. doxygenfunction:: adt_ary_fill

.. doxygenfunction:: adt_ary_clear

.. doxygenfunction:: adt_ary_splice

Fill Element Configuration
--------------------------

.. doxygenfunction:: adt_ary_set_fill_elem

.. doxygenfunction:: adt_ary_get_fill_elem

Sorting & Comparators
---------------------

.. doxygenfunction:: adt_ary_sort

.. doxygenfunction:: adt_i32_vlt

.. doxygenfunction:: adt_u32_vlt
