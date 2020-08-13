.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_run_operator_plus_equals:

operator+=
==========

.. cpp:namespace:: libsemigroups::DynamicMatrix<Semiring, Scalar>

.. cpp:function:: void operator+=(DynamicMatrix const& that)

   Redefines ``this`` to be the sum of ``*this`` and ``that``. 

   :param that: the matrix to add to ``this``.
    
   :returns: (None)

   :throws: 
      if the implementation of the semiring plus throws.

   :complexity: 
      :math:`O(mn)` where :math:`m` is :cpp:any:`number_of_rows`
      and :math:`m` is :cpp:any:`number_of_cols`

   .. warning::
      The matrices must be of the same dimensions, although this is not
      verified by the implementation.

.. cpp:function:: void operator+=(RowView const& that)

   Redefines ``this`` to be the sum of ``*this`` and ``that``. 

   :param that: the matrix to add to ``this``.
    
   :returns: (None)

   :throws: 
      if the implementation of the semiring plus throws.

   :complexity: 
      :math:`O(mn)` where :math:`m` is :cpp:any:`number_of_rows`
      and :math:`m` is :cpp:any:`number_of_cols`

   .. warning::
      This function only works if ``this`` has a single row, i.e. the template
      parameter ``R`` is ``1``.

.. cpp:function:: void operator+=(scalar const a)

   Adds a scalar to every entry of the matrix in-place.

   :param a: the scalar to add to ``this``.
    
   :returns: (None)

   :throws: 
      if the implementation of the semiring plus throws.

   :complexity: 
      :math:`O(mn)` where :math:`m` is :cpp:any:`number_of_rows`
      and :math:`m` is :cpp:any:`number_of_cols`

   .. warning::
      This function only works if ``this`` has a single row, i.e. the template
      parameter ``R`` is ``1``.

