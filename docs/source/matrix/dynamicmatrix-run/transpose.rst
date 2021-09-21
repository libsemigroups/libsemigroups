.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_run_transpose:

transpose
=========

.. cpp:namespace:: libsemigroups::DynamicMatrix<Semiring, Scalar>

.. cpp:function::  void transpose() noexcept

   Transposes the matrix in-place. 

   :parameters: (None)

   :returns: (None)
   
   :exceptions: 
     this function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 
   
   :complexity: 
     :math:`O(n ^ 2)` where :math:`n` is the number of rows and
     the number of columns in the matrix. 

   .. warning:: 
     This only works for square matrices. 
