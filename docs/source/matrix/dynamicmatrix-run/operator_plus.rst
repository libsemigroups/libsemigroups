.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_run_operator_plus:

operator+
=========

.. cpp:namespace:: libsemigroups::DynamicMatrix<Semiring, Scalar>

.. cpp:function:: DynamicMatrix operator+(DynamicMatrix const& that)

   Returns the sum of ``*this`` and ``that``. 

   :param that: the matrix to add to ``this``.
    
   :returns:
      a value of type :cpp:any:`DynamicMatrix`.

   :throws: 
      if the implementation of the semiring plus throws, or ``std::bad_alloc``
      if memory cannot be allocated for the result. 

   :complexity: 
      :math:`O(mn)` where :math:`m` is :cpp:any:`number_of_rows`
      and :math:`m` is :cpp:any:`number_of_cols`

   .. warning::
      The matrices must be of the same dimensions, although this is not
      verified by the implementation.
