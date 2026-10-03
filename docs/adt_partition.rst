Partition Array (adt_partition)
===============================

.. c:type:: adt_u16_partition_t

   Contiguous partition index container using 16-bit unsigned integer offsets (``uint16_t``).
   Partitions an interval :math:`[0, \text{total\_size})` into contiguous, non-overlapping segments up to 65,535 bytes in size.

.. c:type:: adt_u32_partition_t

   Contiguous partition index container using 32-bit unsigned integer offsets (``uint32_t``).
   Partitions an interval :math:`[0, \text{total\_size})` into contiguous, non-overlapping segments up to :math:`2^{32}-1` bytes in size.

The **adt_partition** module provides contiguous partition index data structures that divide a linear range :math:`[0, \text{total\_size})` into contiguous, non-overlapping segments. It maintains the start offset of each segment in a dynamic boundary array, enabling fast :math:`O(\log N)` binary search resolution from any byte offset to its corresponding segment index, as well as :math:`O(1)` random access to segment boundaries.

Key features:

* **Contiguous Segment Appending**: Segments are defined sequentially by specifying their individual lengths (:c:func:`adt_u16_partition_append` / :c:func:`adt_u32_partition_append`). Boundary start offsets and total accumulated size are tracked automatically.
* **Fast Binary Search Lookup**: Mapping any byte offset to its containing segment index (:c:func:`adt_u16_partition_find` / :c:func:`adt_u32_partition_find`) executes in :math:`O(\log N)` time using binary search over the sorted boundary offsets.
* **Direct Segment Retrieval**: Querying the start offset and byte length of any segment (:c:func:`adt_u16_partition_get_segment` / :c:func:`adt_u32_partition_get_segment`) is an :math:`O(1)` lookup.
* **Dual Lifecycle Management**: Supports both stack or embedded initialization (:c:func:`adt_u16_partition_create` / :c:func:`adt_u16_partition_destroy`) and dynamic heap allocation (:c:func:`adt_u16_partition_new` / :c:func:`adt_u16_partition_delete`).
* **Pre-allocation & Reuse**: Capacity can be pre-allocated via :c:func:`adt_u16_partition_reserve` / :c:func:`adt_u32_partition_reserve` to eliminate reallocations during bulk ingestion, and cleared with :c:func:`adt_u16_partition_clear` / :c:func:`adt_u32_partition_clear` without deallocating internal memory.
* **Container Compatibility**: Provides virtual destructors (:c:func:`adt_u16_partition_vdelete` / :c:func:`adt_u32_partition_vdelete`) matching ``void (*)(void*)`` for nesting within generic ADT containers such as :c:type:`adt_ary_t` or :c:type:`adt_hash_t`.

Architecture & Memory Model
---------------------------

A partition index represents a sequence of :math:`N` segments over a linear byte space:

.. code-block:: text

   Offset:  0        start_1       start_2             start_N       total_size
            |-----------|-------------|-------------------|--------------|
   Segment: [ Segment 0 |  Segment 1  |       ...         | Segment N-1  ]

Internally, the container stores an array of segment boundary start offsets with size :math:`N + 1`:

* ``start_offsets[0]`` is always ``0``.
* ``start_offsets[i]`` represents the beginning offset of Segment :math:`i`.
* ``start_offsets[i + 1]`` represents the upper boundary of Segment :math:`i`, yielding segment length :math:`\text{length}_i = \text{start\_offsets}[i + 1] - \text{start\_offsets}[i]`.
* Each segment covers the half-open interval :math:`[\text{start\_offsets}[i],\, \text{start\_offsets}[i + 1])`.
* ``start_offsets[N]`` equals ``total_size``.

Comparing 16-bit and 32-bit Partitions
--------------------------------------

.. list-table::
   :header-rows: 1
   :widths: 25 35 40

   * - Feature
     - :c:type:`adt_u16_partition_t`
     - :c:type:`adt_u32_partition_t`
   * - **Offset Type**
     - ``uint16_t``
     - ``uint32_t``
   * - **Max Total Size**
     - 65,535 bytes (``UINT16_MAX``)
     - 4,294,967,295 bytes (``UINT32_MAX``)
   * - **Segment Capacity Limit**
     - :math:`2^{32}-1` segments
     - :math:`2^{32}-1` segments
   * - **Boundary Entry Size**
     - 2 bytes per boundary entry
     - 4 bytes per boundary entry
   * - **Primary Use Cases**
     - Microcontroller buffers, APX port mappings, CAN frames
     - File partition indexing, large memory buffers, network frames

Status & Error Codes
--------------------

Mutation and query functions report standard ADT error codes (:c:type:`adt_error_t`):

* ``ADT_NO_ERROR`` (``0``): Operation completed successfully.
* ``ADT_INVALID_ARGUMENT_ERROR`` (``1``): NULL pointer passed for ``self``.
* ``ADT_MEM_ERROR`` (``2``): Dynamic memory allocation or reallocation failed.
* ``ADT_INDEX_OUT_OF_BOUNDS_ERROR`` (``3``): Segment index passed to ``*_get_segment`` exceeds ``num_segments - 1``.
* ``ADT_ARRAY_TOO_LARGE_ERROR`` (``5``): Capacity calculation caused integer overflow.
* ``ADT_OVERFLOW_ERROR`` (``9``): Appending the segment would exceed the maximum representable total size (``UINT16_MAX`` for 16-bit, ``UINT32_MAX`` for 32-bit).

For offset lookup (:c:func:`adt_u16_partition_find` / :c:func:`adt_u32_partition_find`), the return value is the zero-based segment index (``0..num_segments-1``), or ``-1`` if the offset is beyond the partitioned range (``offset >= total_size``) or if the partition is empty.

Code Examples
-------------

Example 1: Buffer Port Mapping (16-bit, Stack Allocation)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   #include <stdio.h>
   #include <stdint.h>
   #include "adt_partition.h"

   void example_u16_partition(void)
   {
       adt_u16_partition_t part;
       adt_u16_partition_create(&part);

       // Define 3 contiguous segments:
       // Segment 0: 4 bytes -> [0, 4)
       // Segment 1: 2 bytes -> [4, 6)
       // Segment 2: 1 byte  -> [6, 7)
       adt_u16_partition_append(&part, 4u);
       adt_u16_partition_append(&part, 2u);
       adt_u16_partition_append(&part, 1u);

       printf("Total segments: %u\n", adt_u16_partition_length(&part));         // 3
       printf("Total size: %u bytes\n", adt_u16_partition_total_size(&part));    // 7

       // Binary search lookup for offset 5
       int32_t seg = adt_u16_partition_find(&part, 5u);
       if (seg >= 0)
       {
           uint16_t start = 0;
           uint16_t len = 0;
           adt_u16_partition_get_segment(&part, (uint32_t)seg, &start, &len);
           printf("Offset 5 is in segment %d (start: %u, length: %u)\n", seg, start, len);
           // Output: Offset 5 is in segment 1 (start: 4, length: 2)
       }

       // Out of bounds lookup (offset >= 7) returns -1
       int32_t out_of_bounds = adt_u16_partition_find(&part, 7u);
       printf("Offset 7 segment: %d\n", out_of_bounds); // -1

       // Free internal boundary allocations
       adt_u16_partition_destroy(&part);
   }

Example 2: Dynamic Allocation & Capacity Reservation (32-bit)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   #include <stdio.h>
   #include <stdint.h>
   #include "adt_partition.h"

   void example_u32_partition_heap(void)
   {
       // Allocate partition instance on the heap
       adt_u32_partition_t *part = adt_u32_partition_new();
       if (part == NULL) return;

       // Pre-allocate capacity for 500 segments to avoid incremental reallocations
       adt_u32_partition_reserve(part, 500u);

       // Append variable-length segments
       for (uint32_t i = 0; i < 500u; i++)
       {
           uint32_t seg_len = (i % 2u == 0u) ? 64u : 128u;
           adt_u32_partition_append(part, seg_len);
       }

       printf("Partitioned %u segments, total size %u bytes\n",
              adt_u32_partition_length(part),
              adt_u32_partition_total_size(part));

       // Reset segment count while keeping boundary memory intact
       adt_u32_partition_clear(part);
       printf("After clear: length = %u\n", adt_u32_partition_length(part)); // 0

       // Clean up heap instance and internal storage
       adt_u32_partition_delete(part);
   }

16-Bit Partition API (adt_u16_partition_t)
==========================================

Lifecycle & Memory Management
-----------------------------

.. doxygenfunction:: adt_u16_partition_create

.. doxygenfunction:: adt_u16_partition_destroy

.. doxygenfunction:: adt_u16_partition_new

.. doxygenfunction:: adt_u16_partition_delete

.. doxygenfunction:: adt_u16_partition_vdelete

Capacity & Sizing
-----------------

.. doxygenfunction:: adt_u16_partition_reserve

.. doxygenfunction:: adt_u16_partition_clear

.. doxygenfunction:: adt_u16_partition_length

.. doxygenfunction:: adt_u16_partition_total_size

.. doxygenfunction:: adt_u16_partition_is_empty

Operations & Lookup
-------------------

.. doxygenfunction:: adt_u16_partition_append

.. doxygenfunction:: adt_u16_partition_find

.. doxygenfunction:: adt_u16_partition_get_segment


32-Bit Partition API (adt_u32_partition_t)
==========================================

Lifecycle & Memory Management
-----------------------------

.. doxygenfunction:: adt_u32_partition_create

.. doxygenfunction:: adt_u32_partition_destroy

.. doxygenfunction:: adt_u32_partition_new

.. doxygenfunction:: adt_u32_partition_delete

.. doxygenfunction:: adt_u32_partition_vdelete

Capacity & Sizing
-----------------

.. doxygenfunction:: adt_u32_partition_reserve

.. doxygenfunction:: adt_u32_partition_clear

.. doxygenfunction:: adt_u32_partition_length

.. doxygenfunction:: adt_u32_partition_total_size

.. doxygenfunction:: adt_u32_partition_is_empty

Operations & Lookup
-------------------

.. doxygenfunction:: adt_u32_partition_append

.. doxygenfunction:: adt_u32_partition_find

.. doxygenfunction:: adt_u32_partition_get_segment
