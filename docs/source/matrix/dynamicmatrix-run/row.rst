.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_run_row:

row
===

.. cpp:namespace:: libsemigroups::DynamicMatrix<Semiring, Scalar>

.. cpp:function::  RowView row(size_t i) const
   
   Returns a row view into the specified row.

   :param i: the row 

   :returns: A value of type :cpp:any:`RowView`.
   
   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 
   
   :complexity: Constant
