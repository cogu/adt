Stack (adt_stack)
==================

.. c:type:: adt_stack_t

   A dynamically resizable LIFO (Last-In First-Out) stack for generic object pointers (``void*``).
   It supports traditional stack operations (push, top, pop) and can automatically free contained elements upon destruction using an optional destructor callback (see :ref:`memory-destructors`).

Memory Management
-----------------

Stack / Embedded Allocation
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When embedding an ``adt_stack_t`` instance inside another struct or allocating it on the stack, initialize it with :c:func:`adt_stack_create` and release it with :c:func:`adt_stack_destroy`.

.. doxygenfunction:: adt_stack_create

.. doxygenfunction:: adt_stack_destroy

.. doxygenfunction:: adt_stack_clear

Example:

.. code-block:: c

   #include <stdlib.h>
   #include <string.h>
   #include "adt_stack.h"

   void example_stack(void)
   {
       adt_stack_t stack;
       adt_stack_create(&stack, free);

       adt_stack_push(&stack, strdup("First"));
       adt_stack_push(&stack, strdup("Second"));

       // Destroys remaining strings using free(), then frees internal storage
       adt_stack_destroy(&stack);
   }

Heap Allocation
~~~~~~~~~~~~~~~

When allocating both the structure and its element buffer on the heap, use :c:func:`adt_stack_new` and free it with :c:func:`adt_stack_delete`.

.. doxygenfunction:: adt_stack_new

.. doxygenfunction:: adt_stack_delete

Stack Operations
----------------

.. doxygenfunction:: adt_stack_push

.. doxygenfunction:: adt_stack_top

.. doxygenfunction:: adt_stack_pop

Capacity & Sizing
-----------------

.. doxygenfunction:: adt_stack_size

.. doxygenfunction:: adt_stack_reserve

.. doxygenfunction:: adt_stack_resize
