.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticmatrix_operator_prod:

operator*
=======================

.. cpp:namespace:: libsemigroups::StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>

.. cpp:function:: StaticMatrix operator*(StaticMatrix const& that)

   Returns the product of ``*this`` and ``that``.

   :param that: the matrix to multiply by ``this``.

   :returns:
      a value of type ``StaticMatrix``.

   :throws:
      if the implementation of the semiring multiplication throws, or
      ``std::bad_alloc`` if memory cannot be allocated for the result.

   :complexity:
      :math:`O(mn)` where :math:`m` is :cpp:any:`StaticMatrix::number_of_rows`
      and :math:`m` is :cpp:any:`StaticMatrix::number_of_cols`

   .. warning::
      The matrices must be of the same dimensions, although this is not
      verified by the implementation.
