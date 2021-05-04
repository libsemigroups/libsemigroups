.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticrowview_size:

size
====

.. cpp:namespace:: libsemigroups::StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>

.. cpp:function:: size_t size() const noexcept

   Returns the size of the underlying row.
   
   :parameters: (None)

   :returns: A value of type ``size_t``.
      
   :exceptions: 
     This function is ``noexcept`` and is guaranteed never to throw.   
   
   :complexity: Constant
