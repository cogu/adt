Stream Buffer (adt_streambuffer)
================================

.. c:type:: adt_streambuffer_t

   A rolling multi-slab byte stream buffer designed for zero-copy, low-latency producer-consumer I/O.
   It eliminates :math:`O(N^2)` ``memmove`` churn when parsing byte streams by providing :math:`O(1)` zero-copy consumption,
   guaranteed contiguous reader views, and automatic shrink-on-drain for large payloads.

Architecture & Mechanics
------------------------

* **2-Slab Rolling Model**: Uses an internal ring of 2 slabs (instances of :c:type:`adt_bytearray_t`).
* **Zero-Copy Consumption**: Advancing the read position (:c:func:`adt_streambuffer_read_commit`) is an :math:`O(1)` pointer advance with zero copying or shifting.
* **Guaranteed Contiguous View**: When a slab boundary rollover occurs, any unconsumed remainder is copied to offset 0 of the subsequent slab before new bytes are ingested, ensuring downstream parsers always receive a single contiguous buffer.
* **Shrink-on-Drain**: Slabs expanding beyond ``max_retained_size`` automatically downsize back to ``default_slab_size`` once drained, eliminating permanent heap bloat.

Memory Management
-----------------

Stack / Embedded Allocation
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When ``adt_streambuffer_t`` is allocated on the stack or embedded inside another struct (such as a network socket handle), use :c:func:`adt_streambuffer_create` and :c:func:`adt_streambuffer_destroy`.

.. doxygenfunction:: adt_streambuffer_create

.. doxygenfunction:: adt_streambuffer_destroy

.. doxygenfunction:: adt_streambuffer_clear

Heap Allocation
~~~~~~~~~~~~~~~

When dynamically allocating the structure on the heap, use :c:func:`adt_streambuffer_new` and :c:func:`adt_streambuffer_delete`.

.. doxygenfunction:: adt_streambuffer_new

.. doxygenfunction:: adt_streambuffer_delete

.. doxygenfunction:: adt_streambuffer_vdelete

Producer API (Ingestion)
------------------------

.. doxygenfunction:: adt_streambuffer_append

.. doxygenfunction:: adt_streambuffer_write_begin

.. doxygenfunction:: adt_streambuffer_write_commit

Consumer API (Extraction)
-------------------------

.. doxygenfunction:: adt_streambuffer_read_begin

.. doxygenfunction:: adt_streambuffer_read_commit

Query API
---------

.. doxygenfunction:: adt_streambuffer_length

.. doxygenfunction:: adt_streambuffer_allocated_bytes

.. doxygenfunction:: adt_streambuffer_is_empty
