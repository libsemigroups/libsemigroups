.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_run_operator_prod_equals:

operator*=
==========

.. cpp:namespace:: libsemigroups::DynamicMatrix<Semiring, Scalar>

.. cpp:function:: void operator*=(scalar_type const a)

   Multiplies every entry of the matrix by a scalar in-place.

   :param a: the scalar to multiply every entry of ``this``.
    
   :returns: (None)

   :throws: 
      if the implementation of the semiring multiplication throws.

   :complexity: 
      :math:`O(mn)` where :math:`m` is :cpp:any:`DynamicMatrix::number_of_rows`
      and :math:`m` is :cpp:any:`DynamicMatrix::number_of_cols`

.. cpp:function:: void product_inplace(DynamicMatrix const& A, DynamicMatrix const& B)

    Multiplies ``A`` and ``B`` and stores the result in ``this``.
    
    Redefines ``this`` to be the product of ``A`` and ``B``. This is in-place
    multiplication to avoid allocation of memory for products which do not need
    to be stored for future use.
  
    :param A: the first matrix to multiply
    :param B: the second matrix to multiply
       
    :returns: (None)
      
    :throws: 
      Throws if the implementation of the semiring plus or product throws
      or ``std::bad_alloc``.
      
    :complexity: 
      :math:`O(n ^ 3)` where :math:`n` is :cpp:any:`DynamicMatrix::number_of_rows`
      or :cpp:any:`DynamicMatrix::number_of_cols`
      
    .. warning::

      This function only applies to matrices with the same number of rows
      and columns.
