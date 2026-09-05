Ringbuffer (adt_ringbuf)
========================

.. c:type:: adt_buf_err_t

   Error status code returned by ringbuffer operations (``uint8_t``).

.. c:type:: adt_rbfs_t

   Static circular FIFO buffer for elements of fixed byte size. Operates on caller-supplied memory with zero dynamic heap allocation.

.. c:type:: adt_rbfu16_t

   Specialized embedded circular FIFO buffer for ``uint16_t`` values. Operates on caller-supplied ``uint16_t`` storage with zero dynamic heap allocation.

.. c:type:: adt_rbfh_t

   Dynamically resizing heap-managed circular FIFO buffer for fixed-size elements.

The **adt_ringbuf** module provides three distinct circular FIFO (First-In First-Out) buffer implementations tailored for different memory architectures and use cases:

* **Static Element Buffer** (:c:type:`adt_rbfs_t`): Stores elements of arbitrary fixed byte size using caller-provided buffer memory. Requires **zero heap allocation**.
* **Static uint16 Buffer** (:c:type:`adt_rbfu16_t`): Optimized circular buffer storing 16-bit unsigned integers (``uint16_t``) using caller-provided storage. Requires **zero heap allocation**.
* **Heap-Managed Buffer** (:c:type:`adt_rbfh_t`): Stores elements of arbitrary fixed byte size and automatically grows on demand up to a configurable maximum capacity using dynamic heap allocation.

CMake Configuration & Zero-Heap Embedded Support
------------------------------------------------

By default, ``adt_ringbuf.c`` does not compile any ringbuffer variants unless explicitly activated via CMake options:

.. list-table::
   :header-rows: 1
   :widths: 25 25 20 30

   * - CMake Option
     - Usage
     - Storage / Memory
     - Description
   * - ``ADT_RBFS_ENABLE``
     - ``-DADT_RBFS_ENABLE=ON``
     - Static (zero heap)
     - Enables :c:type:`adt_rbfs_t` and its API
   * - ``ADT_RBFU16_ENABLE``
     - ``-DADT_RBFU16_ENABLE=ON``
     - Static (zero heap)
     - Enables :c:type:`adt_rbfu16_t` and its API
   * - ``ADT_RBFH_ENABLE``
     - ``-DADT_RBFH_ENABLE=ON``
     - Heap (dynamic)
     - Enables :c:type:`adt_rbfh_t` and its API

.. note::

   The primary reason for separate CMake options is to allow embedded targets to enable :c:type:`adt_rbfs_t` and :c:type:`adt_rbfu16_t` without enabling, linking, or requiring heap memory functions (``malloc`` and ``free``). In resource-constrained microcontrollers or safety-critical software where dynamic memory allocation is restricted or prohibited, these static buffers guarantee deterministic $O(1)$ performance and bounded memory usage.

Status & Error Codes
--------------------

Ringbuffer operations return one of the following status codes:

* ``BUF_E_OK`` (``0``): Operation succeeded.
* ``BUF_E_NOT_OK`` (``1``): Operation failed due to invalid arguments (e.g. NULL pointer or zero element count) or memory allocation error.
* ``BUF_E_OVERFLOW`` (``2``): Ring buffer is full; insertion rejected.
* ``BUF_E_UNDERFLOW`` (``3``): Ring buffer is empty; remove or peek cannot return an element.

Code Examples
-------------

Example 1: Static Buffer (Zero Heap)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   #include <stdio.h>
   #include <stdint.h>
   #include "adt_ringbuf.h"

   typedef struct {
       uint16_t sensor_id;
       int16_t  reading;
   } sample_t;

   #define QUEUE_CAPACITY 8

   void example_static_ringbuf(void)
   {
       adt_rbfs_t queue;
       sample_t storage[QUEUE_CAPACITY];

       // Initialize using caller-provided buffer (zero heap allocation)
       adt_rbfs_create(&queue, (uint8_t*) storage, QUEUE_CAPACITY, sizeof(sample_t));

       // Enqueue samples
       sample_t s1 = {.sensor_id = 1, .reading = 235};
       sample_t s2 = {.sensor_id = 2, .reading = 108};
       adt_rbfs_insert(&queue, (const uint8_t*) &s1);
       adt_rbfs_insert(&queue, (const uint8_t*) &s2);

       printf("Elements in queue: %u, free slots: %u\n",
              adt_rbfs_size(&queue), adt_rbfs_free(&queue));

       // Dequeue samples
       sample_t out;
       while (adt_rbfs_remove(&queue, (uint8_t*) &out) == BUF_E_OK)
       {
           printf("Sensor %u: reading %d\n", out.sensor_id, out.reading);
       }
   }

Example 2: Embedded uint16 Buffer (Zero Heap)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   #include <stdio.h>
   #include <stdint.h>
   #include "adt_ringbuf.h"

   #define BUFFER_SIZE 16

   void example_u16_ringbuf(void)
   {
       adt_rbfu16_t rbf;
       uint16_t buffer[BUFFER_SIZE];

       // Initialize uint16 ring buffer
       adt_rbfu16_create(&rbf, buffer, BUFFER_SIZE);

       // Insert values
       adt_rbfu16_insert(&rbf, 0x1001);
       adt_rbfu16_insert(&rbf, 0x2002);

       // Peek at oldest value
       uint16_t val = 0;
       if (adt_rbfu16_peek(&rbf, &val) == BUF_E_OK)
       {
           printf("Oldest value: 0x%04X (length=%u)\n", val, adt_rbfu16_length(&rbf));
       }

       // Remove values
       while (adt_rbfu16_remove(&rbf, &val) == BUF_E_OK)
       {
           printf("Popped: 0x%04X\n", val);
       }
   }

Example 3: Dynamically Growing Heap Buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   #include <stdio.h>
   #include <stdint.h>
   #include "adt_ringbuf.h"

   void example_heap_ringbuf(void)
   {
       // Create heap-managed buffer: elements of uint32_t, min=10, max=1000
       adt_rbfh_t *rbf = adt_rbfh_newEx(sizeof(uint32_t), 10, 1000);
       if (rbf == NULL) return;

       // Insert values; buffer automatically grows dynamically if needed
       for (uint32_t i = 1; i <= 25; i++)
       {
           adt_rbfh_insert(rbf, (const uint8_t*) &i);
       }

       printf("Buffer length: %u\n", adt_rbfh_length(rbf));

       // Remove and process all values
       uint32_t item = 0;
       while (adt_rbfh_remove(rbf, (uint8_t*) &item) == BUF_E_OK)
       {
           printf("%u ", item);
       }
       printf("\n");

       // Clean up
       adt_rbfh_delete(rbf);
   }

Static Element Ringbuffer (adt_rbfs) API
----------------------------------------

.. doxygenfunction:: adt_rbfs_create

.. doxygenfunction:: adt_rbfs_insert

.. doxygenfunction:: adt_rbfs_remove

.. doxygenfunction:: adt_rbfs_peek

.. doxygenfunction:: adt_rbfs_size

.. doxygenfunction:: adt_rbfs_free

.. doxygenfunction:: adt_rbfs_clear

Embedded uint16 Ringbuffer (adt_rbfu16) API
-------------------------------------------

.. doxygenfunction:: adt_rbfu16_create

.. doxygenfunction:: adt_rbfu16_insert

.. doxygenfunction:: adt_rbfu16_remove

.. doxygenfunction:: adt_rbfu16_peek

.. doxygenfunction:: adt_rbfu16_length

Heap-Managed Ringbuffer (adt_rbfh) API
--------------------------------------

.. doxygenfunction:: adt_rbfh_create

.. doxygenfunction:: adt_rbfh_createEx

.. doxygenfunction:: adt_rbfh_destroy

.. doxygenfunction:: adt_rbfh_new

.. doxygenfunction:: adt_rbfh_newEx

.. doxygenfunction:: adt_rbfh_delete

.. doxygenfunction:: adt_rbfh_insert

.. doxygenfunction:: adt_rbfh_remove

.. doxygenfunction:: adt_rbfh_peek

.. doxygenfunction:: adt_rbfh_length

.. doxygenfunction:: adt_rbfh_free

.. doxygenfunction:: adt_rbfh_clear
