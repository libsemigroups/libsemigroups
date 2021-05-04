.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticrowview_call_operator:

operator()
==========

.. cpp:namespace:: libsemigroups::StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>

.. cpp:function:: scalar_reference operator()(size_t const i) 

   Returns a reference to the specified entry of the matrix. 

   :param i: the index of the entry
    
   :returns: A value of type :cpp:any:`scalar_reference`.

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 

   :complexity: 
     Constant

   .. warning::
      No checks on the validity of the parameter ``i`` is performed.

.. cpp:function:: scalar_const_reference operator()(size_t const i) const 

   Returns a const reference to the specified entry of the matrix. 

   :param i: the index of the entry
    
   :returns: A value of type :cpp:any:`scalar_const_reference`.

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 

   :complexity: 
     Constant

   .. warning::
      No checks on the validity of the parameter ``i`` is performed.
