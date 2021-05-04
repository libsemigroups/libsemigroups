.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticmatrix_end:

end, cend
=========

.. cpp:namespace:: libsemigroups::StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>

.. cpp:function:: iterator end()

   Returns a (random access) iterator pointing one passed the last entry of the
   matrix.

   :parameters: (None)

   :returns:
     A value of type ``iterator``.
   
   :complexity: Constant
   
   :exceptions: 
     This function is ``noexcept`` and is guaranteed never to throw.   

   
   .. warning:: 
     The order in which entries in the matrix are iterated over is not
     specified.


.. cpp:function:: const_iterator cend()

   Returns a const (random access) iterator pointing one passed the last entry
   of the matrix.

   :parameters: (None)

   :returns:
     A value of type ``const_iterator``.
   
   :complexity: Constant
   
   :exceptions: 
     This function is ``noexcept`` and is guaranteed never to throw.   

   .. warning:: 
     The order in which entries in the matrix are iterated over is not
     specified.

