.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticmatrix_swap:

swap
====

.. cpp:namespace:: libsemigroups::StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>

.. cpp:function::  void swap(StaticMatrix& that) noexcept

   Swaps the contents of ``this`` with the contents of ``that``. 

   :param that: the matrix to swap contents with

   :returns: (None)
   
   :complexity: Constant
   
   :exceptions: 
     this function is ``noexcept`` and is guaranteed never to throw.   
