Range List (adt_range_list)
===========================

.. c:type:: adt_u16_range_list_t

   Sorted, non-overlapping range list container using 16-bit unsigned integer offsets (``uint16_t``).
   Tracks disjoint intervals :math:`[\text{start}, \text{start} + \text{length})` up to 65,535 bytes in size.

.. c:type:: adt_u32_range_list_t

   Sorted, non-overlapping range list container using 32-bit unsigned integer offsets (``uint32_t``).
   Tracks disjoint intervals :math:`[\text{start}, \text{start} + \text{length})` up to :math:`2^{32}-1` bytes in size.

The **adt_range_list** module provides sorted, non-overlapping interval containers for tracking sparse data ranges, dirty buffer segments, and memory slices. It maintains an array of disjoint intervals sorted strictly by start offset:

.. math::

   R_0.\text{start} < R_1.\text{start} < \dots < R_{n-1}.\text{start}

When new ranges are inserted, the container automatically merges overlapping, adjacent, or nearby ranges, coalescing multiple fragmented updates into compact contiguous spans.

Key features:

* **Disjoint Range Tracking**: Stores intervals as ``(start, length)`` pairs (:c:type:`adt_u16_range_t` / :c:type:`adt_u32_range_t`). Each range covers the half-open interval :math:`[\text{start},\, \text{start} + \text{length})`.
* **Automatic Range Merging**: Inserting a range (:c:func:`adt_u16_range_list_insert` / :c:func:`adt_u32_range_list_insert`) automatically checks for overlaps or adjacency with existing ranges and merges them into a single span.
* **Configurable Gap Coalescing (``max_gap``)**: Supports merging disjoint ranges separated by a gap up to ``max_gap`` bytes. This is particularly valuable for network protocols (such as APX RemoteFile), where sending small unchanged gaps inside a larger frame is more bandwidth-efficient than paying the framing and packet header overhead for multiple disjoint packets. Setting ``max_gap = 0`` strictly merges only touching or overlapping ranges.
* **Multi-Range Bridging**: If an inserted range spans across multiple existing disjoint ranges, all swallowed ranges are automatically collapsed and coalesced into a single consolidated range in :math:`O(N)` time with contiguous ``memmove``.
* **Fast Binary Search Lookup**: Mapping any byte offset to its containing range index (:c:func:`adt_u16_range_list_find` / :c:func:`adt_u32_range_list_find`) and testing coverage (:c:func:`adt_u16_range_list_contains` / :c:func:`adt_u32_range_list_contains`) execute in :math:`O(\log N)` time.
* **Dual Lifecycle Management**: Supports stack or embedded initialization (:c:func:`adt_u16_range_list_create` / :c:func:`adt_u16_range_list_destroy`) and heap allocation (:c:func:`adt_u16_range_list_new` / :c:func:`adt_u16_range_list_delete`).
* **Zero-Allocation Batch Reuse**: Calling :c:func:`adt_u16_range_list_clear` / :c:func:`adt_u32_range_list_clear` resets the active range count to zero without freeing allocated capacity, eliminating dynamic heap allocations across repeated transaction cycles.
* **Container Compatibility**: Provides virtual destructors (:c:func:`adt_u16_range_list_vdelete` / :c:func:`adt_u32_range_list_vdelete`) matching ``void (*)(void*)`` for nesting within generic ADT containers.

Architecture & Range Coalescing
-------------------------------

A range list represents an ordered sequence of :math:`N` disjoint half-open intervals over a byte address space:

.. code-block:: text

   Offset:  0        10   15       30        50   60              100
            |--------|    |--------|         |----|               |---|
   Ranges:  [  R0    ]    [  R1    ]         [ R2 ]               [R3 ]

When inserting a new range :math:`N`, four scenarios can occur:

1. **Disjoint Insertion**: :math:`N` does not touch or overlap any existing range (and its distance to neighboring ranges exceeds ``max_gap``). :math:`N` is inserted into its sorted position.
2. **Adjacent / Overlapping Extension**: :math:`N` touches or overlaps an existing range :math:`R_i`. :math:`R_i` expands to cover :math:`[\min(R_i.\text{start}, N.\text{start}),\, \max(R_i.\text{end}, N.\text{end}))`.
3. **Multi-Range Bridging**: :math:`N` spans across multiple ranges (e.g. bridging :math:`R_1` and :math:`R_2`). The container expands :math:`R_1` to cover the entire union and shifts the array left with ``memmove`` to delete the swallowed ranges.
4. **Gap Coalescing (``max_gap > 0``)**: If the distance between two ranges :math:`B.\text{start} - A.\text{end} \le \text{max\_gap}`, they coalesce into a single contiguous interval.

Comparing 16-bit and 32-bit Range Lists
---------------------------------------

.. list-table::
   :header-rows: 1
   :widths: 25 35 40

   * - Feature
     - :c:type:`adt_u16_range_list_t`
     - :c:type:`adt_u32_range_list_t`
   * - **Offset & Length Type**
     - ``uint16_t``
     - ``uint32_t``
   * - **Range Struct**
     - :c:type:`adt_u16_range_t` (4 bytes)
     - :c:type:`adt_u32_range_t` (8 bytes)
   * - **Max Offset**
     - 65,535 bytes (``UINT16_MAX``)
     - 4,294,967,295 bytes (``UINT32_MAX``)
   * - **Total Length Return**
     - ``uint32_t``
     - ``uint64_t``
   * - **Primary Use Cases**
     - Small MCU buffers, CAN payloads, micro-transactions
     - Streaming IPC, file buffer dirty tracking, network packet batching

Status & Error Codes
--------------------

Mutation and query functions report standard ADT error codes (:c:type:`adt_error_t`):

* ``ADT_NO_ERROR`` (``0``): Operation completed successfully.
* ``ADT_INVALID_ARGUMENT_ERROR`` (``1``): NULL pointer passed for ``self`` or output pointer.
* ``ADT_MEM_ERROR`` (``2``): Dynamic memory allocation or reallocation failed.
* ``ADT_INDEX_OUT_OF_BOUNDS_ERROR`` (``3``): Range index passed to ``*_get_range`` exceeds ``num_ranges - 1``.
* ``ADT_OVERFLOW_ERROR`` (``9``): Inserting the range would cause ``start + length`` to exceed the integer boundary (``UINT16_MAX`` for 16-bit, ``UINT32_MAX`` for 32-bit).

For offset lookup (:c:func:`adt_u16_range_list_find` / :c:func:`adt_u32_range_list_find`), the return value is the zero-based range index (``0..num_ranges-1``), or ``-1`` if the offset is not contained within any range.

Code Examples
-------------

Example 1: Adjacent Merging (16-bit, Stack Allocation)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   #include <stdio.h>
   #include <stdint.h>
   #include "adt_range_list.h"

   void example_u16_range_list(void)
   {
       adt_u16_range_list_t list;
       adt_u16_range_list_create(&list);

       // Insert [10, 15)
       adt_u16_range_list_insert(&list, 10u, 5u, 0u);

       // Insert adjacent [15, 20) with max_gap = 0 -> automatically merges into [10, 20)
       adt_u16_range_list_insert(&list, 15u, 5u, 0u);

       printf("Total ranges: %u\n", adt_u16_range_list_length(&list)); // 1
       const adt_u16_range_t *r0 = adt_u16_range_list_at(&list, 0u);
       printf("Range 0: start=%u, length=%u\n", r0->start, r0->length); // start=10, length=10

       // Lookup offsets
       printf("Offset 12 in range: %d\n", adt_u16_range_list_find(&list, 12u)); // 0
       printf("Offset 25 in range: %d\n", adt_u16_range_list_find(&list, 25u)); // -1

       adt_u16_range_list_destroy(&list);
   }

Example 2: Batching & Gap Coalescing (32-bit, Heap Allocation)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   #include <stdio.h>
   #include <stdint.h>
   #include "adt_range_list.h"

   void example_u32_batching(void)
   {
       adt_u32_range_list_t *list = adt_u32_range_list_new();
       if (list == NULL) return;

       // Reserve capacity for expected ranges
       adt_u32_range_list_reserve(list, 64u);

       // Coalesce writes separated by up to 6 bytes (e.g. protocol header threshold)
       const uint32_t max_gap = 6u;

       // Signal A: offset 100, length 4 -> [100, 104)
       adt_u32_range_list_insert(list, 100u, 4u, max_gap);

       // Signal B: offset 108, length 4 -> [108, 112)
       // Gap is 4 bytes (104..107) <= max_gap (6) -> merges into [100, 112)
       adt_u32_range_list_insert(list, 108u, 4u, max_gap);

       // Signal C: offset 200, length 2 -> [200, 202) (gap of 88 bytes > 6 -> remains separate)
       adt_u32_range_list_insert(list, 200u, 2u, max_gap);

       printf("Merged into %u packet(s):\n", adt_u32_range_list_length(list));
       for (uint32_t i = 0; i < adt_u32_range_list_length(list); i++)
       {
           const adt_u32_range_t *r = adt_u32_range_list_at(list, i);
           printf("  Packet %u: offset %u, size %u\n", i, r->start, r->length);
       }
       // Output:
       //   Packet 0: offset 100, size 12
       //   Packet 1: offset 200, size 2

       // Reset range count for next transaction without deallocating buffer
       adt_u32_range_list_clear(list);

       adt_u32_range_list_delete(list);
   }

16-Bit Range List API (adt_u16_range_list_t)
============================================

Lifecycle & Memory Management
-----------------------------

.. doxygenfunction:: adt_u16_range_list_create

.. doxygenfunction:: adt_u16_range_list_destroy

.. doxygenfunction:: adt_u16_range_list_new

.. doxygenfunction:: adt_u16_range_list_delete

.. doxygenfunction:: adt_u16_range_list_vdelete

Capacity & Sizing
-----------------

.. doxygenfunction:: adt_u16_range_list_reserve

.. doxygenfunction:: adt_u16_range_list_clear

.. doxygenfunction:: adt_u16_range_list_length

.. doxygenfunction:: adt_u16_range_list_total_length

.. doxygenfunction:: adt_u16_range_list_is_empty

Operations & Lookup
-------------------

.. doxygenfunction:: adt_u16_range_list_insert

.. doxygenfunction:: adt_u16_range_list_find

.. doxygenfunction:: adt_u16_range_list_contains

.. doxygenfunction:: adt_u16_range_list_get_range

.. doxygenfunction:: adt_u16_range_list_at


32-Bit Range List API (adt_u32_range_list_t)
============================================

Lifecycle & Memory Management
-----------------------------

.. doxygenfunction:: adt_u32_range_list_create

.. doxygenfunction:: adt_u32_range_list_destroy

.. doxygenfunction:: adt_u32_range_list_new

.. doxygenfunction:: adt_u32_range_list_delete

.. doxygenfunction:: adt_u32_range_list_vdelete

Capacity & Sizing
-----------------

.. doxygenfunction:: adt_u32_range_list_reserve

.. doxygenfunction:: adt_u32_range_list_clear

.. doxygenfunction:: adt_u32_range_list_length

.. doxygenfunction:: adt_u32_range_list_total_length

.. doxygenfunction:: adt_u32_range_list_is_empty

Operations & Lookup
-------------------

.. doxygenfunction:: adt_u32_range_list_insert

.. doxygenfunction:: adt_u32_range_list_find

.. doxygenfunction:: adt_u32_range_list_contains

.. doxygenfunction:: adt_u32_range_list_get_range

.. doxygenfunction:: adt_u32_range_list_at
