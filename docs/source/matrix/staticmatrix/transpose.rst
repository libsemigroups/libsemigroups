.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticmatrix_transpose:

transpose
=========

.. cpp:namespace:: libsemigroups::StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>

.. cpp:function::  void transpose() noexcept

   Transposes the matrix in-place. 

   :parameters: (None)

   :returns: (None)
   
   :exceptions: 
     this function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 
   
   :complexity: 
     :math:`O(mn)` where :math:`m` is the template parameter :code:`R` and
     :math:`n` is the template parameter :code:`C`. 

   .. warning:: 
     This only works when the template parameters ``R`` and ``C`` are equal
     (i.e. for square matrices).
