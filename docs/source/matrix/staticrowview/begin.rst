.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticrowview_begin:

begin, cbegin
=============

.. cpp:namespace:: libsemigroups::StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>

.. cpp:function:: iterator begin()

   Returns a (random access) iterator pointing at the first entry in the
   row.

   :parameters: (None)

   :returns: A value of type :cpp:any:`iterator`.
   
   :complexity: Constant
   
   :exceptions: 
     This function is ``noexcept`` and is guaranteed never to throw.   

.. cpp:function:: const_iterator cbegin()

   Returns a const (random access) iterator pointing at the first entry in the
   row.

   :parameters: (None)

   :returns: A value of type :cpp:any:`const_iterator`.
   
   :complexity: Constant
   
   :throws: 
     This function is ``noexcept`` and is guaranteed never to throw.   
