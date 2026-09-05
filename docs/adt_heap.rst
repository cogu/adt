Binary Heap / Priority Queue (adt_heap)
========================================

.. c:type:: adt_heap_elem_t

   A heap node structure associating a user-defined payload pointer with a 32-bit unsigned priority / sort value.

.. c:enum:: adt_heap_family

   .. c:enumerator:: ADT_MIN_HEAP

      Min-heap ordering: the lowest value is kept at the root (index ``0``). Ideal for deadline schedulers.

   .. c:enumerator:: ADT_MAX_HEAP

      Max-heap ordering: the highest value is kept at the root (index ``0``). Ideal for highest-priority-first schedulers.

The **adt_heap** module provides binary heap algorithms layered on top of :c:type:`adt_ary_t` as a backing container. Rather than wrapping the array in a separate container struct, the heap algorithms operate directly on an array of heap elements.

Example: Min-Heap Priority Queue
--------------------------------

.. code-block:: c

   #include <stdio.h>
   #include "adt_ary.h"
   #include "adt_heap.h"

   void example_priority_queue(void)
   {
       adt_ary_t heap;
       adt_ary_create(&heap, adt_heap_elem_vdelete);

       // Push elements with priorities (e.g. deadlines)
       adt_heap_elem_t *e1 = adt_heap_elem_new("Task C", 30);
       adt_ary_push(&heap, e1);
       adt_heap_sortUp(&heap, adt_ary_length(&heap) - 1, ADT_MIN_HEAP);

       adt_heap_elem_t *e2 = adt_heap_elem_new("Task A", 10);
       adt_ary_push(&heap, e2);
       adt_heap_sortUp(&heap, adt_ary_length(&heap) - 1, ADT_MIN_HEAP);

       adt_heap_elem_t *e3 = adt_heap_elem_new("Task B", 20);
       adt_ary_push(&heap, e3);
       adt_heap_sortUp(&heap, adt_ary_length(&heap) - 1, ADT_MIN_HEAP);

       // Pop root (lowest value = Task A)
       adt_heap_elem_t *top = (adt_heap_elem_t*) adt_ary_value(&heap, 0);
       printf("Next task: %s (priority %u)\n", (char*) top->pItem, top->u32Value);

       // Replace root with last element and trickle down
       adt_heap_elem_t *last = (adt_heap_elem_t*) adt_ary_pop(&heap);
       if (adt_ary_length(&heap) > 0)
       {
           adt_ary_set(&heap, 0, last);
           adt_heap_sortDown(&heap, 0, ADT_MIN_HEAP);
       }
       adt_heap_elem_delete(top);

       // Clean up remaining elements
       adt_ary_destroy(&heap);
   }

Element Lifecycle
-----------------

.. doxygenfunction:: adt_heap_elem_create

.. doxygenfunction:: adt_heap_elem_destroy

.. doxygenfunction:: adt_heap_elem_new

.. doxygenfunction:: adt_heap_elem_delete

.. doxygenfunction:: adt_heap_elem_vdelete

Heap Invariant Algorithms
-------------------------

.. doxygenfunction:: adt_heap_sortUp

.. doxygenfunction:: adt_heap_sortDown
