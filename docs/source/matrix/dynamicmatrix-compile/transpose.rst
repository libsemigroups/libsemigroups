.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_compile_transpose:

transpose
=========

.. cpp:namespace:: libsemigroups::DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>

.. cpp:function::  void transpose() noexcept

   Swaps the contents of ``this`` with the contents of ``that``. 

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
