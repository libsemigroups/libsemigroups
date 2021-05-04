.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticmatrix_number_of_rows:

number_of_rows, number_of_cols
==============================

.. cpp:namespace:: libsemigroups::StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>

.. cpp:function:: size_t number_of_rows()

   Returns the number of rows of the matrix.

   :returns:
     A value of type ``size_t``.
   
   :complexity: Constant
   
   :exceptions: 
     this function is ``noexcept`` and is guaranteed never to throw.   

   :parameters: (None)


.. cpp:function:: size_t number_of_cols()

   Returns the number of columns of the matrix.

   :returns:
     A value of type ``size_t``.
   
   :complexity: Constant
   
   :exceptions: 
     this function is ``noexcept`` and is guaranteed never to throw.   

   :parameters: (None)
   
