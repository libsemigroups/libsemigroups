.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_run_begin:

DynamicMatrix::begin, DynamicMatrix::cbegin
===========================================

.. cpp:function:: iterator DynamicMatrix::begin()

   Returns a (random access) iterator pointing at the first entry in the
   matrix.

   :returns:
     A value of type ``iterator``.
   
   :complexity: Constant
   
   :throws: 
     this function is ``noexcept`` and is guaranteed never to throw.   

   :parameters: (None)
   
   .. warning:: 
     The order in which entries in the matrix are iterated over is not
     specified.


.. cpp:function:: const_iterator DynamicMatrix::cbegin()

   Returns a const (random access) iterator pointing at the first entry in the
   matrix.

   :returns:
     A value of type ``const_iterator``.
   
   :complexity: Constant
   
   :throws: 
     this function is ``noexcept`` and is guaranteed never to throw.   

   :parameters: (None)

   .. warning:: 
     The order in which entries in the matrix are iterated over is not
     specified.

