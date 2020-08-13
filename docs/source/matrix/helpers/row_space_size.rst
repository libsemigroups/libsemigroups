.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

row_space_size
==============

.. cpp:namespace:: libsemigroups::matrix_helpers

.. cpp:function:: template <typename Mat> \
                  size_t row_space_size(Mat const& x) 

   Returns the size of the row space of the matrix ``x``. This is currently
   only implemented for types of matrix ``Mat`` where :cpp:any:`IsBMat\<Mat>` is
   ``true``. This function template is defined in namespace
   :cpp:any:`matrix_helpers`.

   :param x: the matrix

   :returns: A value of type ``size_t``. 

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`.
     It might throw ``std::bad_alloc`` if the algorithm cannot allocate enough
     memory.

   :complexity: 
     :math:`O(mn)` where :math:`m` is the size of the row basis of ``x`` and
     :math:`n` is the size of the row space. 

     .. warning::

         If :math:`k` is the dimension of the matrix ``x``, then :math:`n` 
         can be as large as :math:`2 ^ k`.

   **Example** 

   .. code-block::  

      auto x = BMat<>::make({{1, 0, 0}, {0, 0, 1}, {0, 1, 0}});
      matrix_helpers::row_space_size(x); // returns 7
