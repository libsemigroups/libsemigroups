.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_compile_rows:

rows
====

.. cpp:namespace:: libsemigroups::DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>

.. cpp:function:: template <typename T> void rows(T& x) const 
   
   Add row views for every row in the matrix to the container ``T``.
   
   :param x: a container

   :returns: (None)
   
   :exceptions: 
     this function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 
   
   :complexity:
     :math:`O(m)` where :math:`m` is the template parameter :code:`R` 
