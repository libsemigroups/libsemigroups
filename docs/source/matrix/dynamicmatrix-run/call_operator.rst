.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_run_call_operator:

operator()
==========

.. cpp:namespace:: libsemigroups::DynamicMatrix<Semiring, Scalar>

.. cpp:function:: scalar_reference operator()(size_t const r, size_t const c) 

   Returns a reference to the specified entry of the matrix. 

   :param r: the index of the row of the entry
   :param c: the index of the column of the entry
    
   :returns: A value of type :cpp:any:`scalar_reference`.

   :exceptions: 
     this function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 

   :complexity: 
     Constant

   .. warning::
      No checks on the validity of the parameters ``r`` and ``c`` are
      performed.

.. cpp:function:: scalar_const_reference operator()(size_t const r, size_t const c) const 

   Returns a const reference to the specified entry of the matrix. 

   :param r: the index of the row of the entry
   :param c: the index of the column of the entry
    
   :returns: A value of type :cpp:any:`scalar_const_reference`.

   :exceptions: 
     this function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 

   :complexity: 
     Constant

   .. warning::
      No checks on the validity of the parameters ``r`` and ``c`` are
      performed.
