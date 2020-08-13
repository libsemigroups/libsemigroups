.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_run_coords:

coords
======

.. cpp:namespace:: libsemigroups::DynamicMatrix<Semiring, Scalar>

.. cpp:function:: std::pair<scalar_type, scalar_type> \
                  coords(const_iterator it) const 

   Returns a pair containing the row and columns corresponding to an iterator.
   
   :param it: the iterator 

   :returns:
     A value of type ``std::pair<scalar_type, scalar_type>``.
   
   :complexity: Constant
   
   :exceptions: 
     this function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 
