.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

pow
===

.. cpp:namespace:: libsemigroups::matrix_helpers

.. cpp:function:: template <typename Mat> \
                  Mat pow(Mat const& x, typename Mat::scalar_type e) 

   Returns the matrix ``x`` to the power ``e``.  This function template is
   defined in namespace :cpp:any:`matrix_helpers`.

   :param x: the matrix   (must have equal number of rows and columns)
   :param e: the exponent (must be :math:`\geq 0`)

   :returns: 
     A value of type ``Mat``. If ``e`` is ``0``, then the identity matrix is
     returned; if ``e`` is ``1``, then a copy of the parameter ``x`` is
     returned. 

   :throws: 
     :cpp:any:`LibsemigroupsException` if ``typename Mat::scalar_type`` is a
     signed type and the parameter ``e`` is less than ``0``. 

     This function does not to throw any other
     :cpp:any:`LibsemigroupsException` s.  It might throw ``std::bad_alloc`` if
     the algorithm cannot allocate enough memory.

   :complexity: 
     :math:`O(m\log_2(e))` where :math:`m` is the dimension of the matrix ``x``
     and :math:`e` is the parameter ``e``. 

   **Example** 

   .. code-block::  

      auto x == ProjMaxPlusMat<>::make({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}));
      matrix_helpers::pow(x, 100); // returns {{-1, 0, -1}, {-2, -1, -2}, {-1, 0, -1}} 
