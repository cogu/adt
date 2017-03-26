adt_bytearray class
===================

The adt_bytearray is similar in fashion to the QByteArray of the Qt framework.

C API
-----

adt_bytearray_create
~~~~~~~~~~~~~~~~~~~~

.. c:function:: void adt_bytearray_create(adt_bytearray_t *self,uint32_t u32GrowSize)

Initializes the adt_bytearray object located in self. The u32GrowSize is an argument which can be used to 
fine-tune how the byte array allocates memory over time. Setting this argument to 0 uses a default growth size
which should be suitable for most uses.

Example:

.. code:: C

   #include "adt_bytearray.h"

   int main(int argc, char **argv)
   {
      adt_bytearray_t bytearray;
      adt_bytearray_create(&bytearray);
   }

adt_bytearray_destroy
~~~~~~~~~~~~~~~~~~~~~

.. c:function:: void adt_bytearray_destroy(adt_bytearray_t *self)

Destroys the adt_byterray object (but does not free its memory). Use it on objects created by **adt_bytearray_create**.

Example:

.. code:: C

   #include "adt_bytearray.h"

   int main(int argc, char **argv)
   {
      adt_bytearray_t bytearray;
      adt_bytearray_create(&bytearray);
      adt_bytearray_destroy(&bytearray);
   }
