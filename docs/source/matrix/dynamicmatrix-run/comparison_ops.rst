.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_run_comparison_ops:

DynamicMatrix comparison operators
==================================

.. cpp:namespace:: libsemigroups::DynamicMatrix<Semiring, Scalar>

.. _dynamicmatrix_run_operator_equals:

.. cpp:function:: bool operator==(DynamicMatrix const& that) const

   Equality operator.

   :param that: matrix for comparison.

   :returns: 
    ``true`` if ``*this`` and ``that`` are equal and ``false`` if they are
    not. 

   :complexity:
     At worst :math:`O(mn)` where :math:`m` is the number of rows and
     :math:`n` is the number of columns of the matrix.

.. cpp:function:: bool operator==(RowView const& that) const

   Equality operator.

   :param that: the row view for comparison.

   :returns: 
     ``true`` if ``*this`` and ``that`` are equal and ``false`` if they are
     not.  In particular, if ``*this`` has more than one row, then ``false``
     is returned.

   :complexity:
     At worst :math:`O(n)` where :math:`n` is the number of columns of the
     matrix. 

.. _dynamicmatrix_run_operator_not_equals:

.. cpp:function:: template <typename T> \
                  bool operator!=(T const& that) const
  
   Inequality operator.

   :tparam T: 
     either :cpp:any:`DynamicMatrix` or :cpp:any:`RowView`
   
   :param that: the matrix or row view for comparison.

   :returns:
     the negation of ``operator==(that)``.
  
   :complexity:
     see :cpp:any:`operator==` 

.. _dynamicmatrix_run_operator_less:

.. cpp:function:: bool operator<(DynamicMatrix const& that) const

   Less than operator.

   This operator defines a total order on the set of matrices of the same type,
   the details of which is implementation specific.

   :param that: the matrix for comparison.

   :returns: 
     ``true`` if ``*this`` is less than ``that`` and ``false`` if it is not.  

   :complexity:
     At worst :math:`O(mn)` where :math:`m` is
     :cpp:any:`number_of_rows`
     and :math:`n` is :cpp:any:`number_of_cols`

.. cpp:function:: bool operator<(RowView const& that) const 

   Less than operator.

   :param that: the row view for comparison.

   :returns: 
      ``true`` if ``*this`` is less than ``that``, and ``false``
      otherwise. In particular, if ``*this`` has more than one row, then 
      ``false`` is returned.
      
   :complexity:
     At worst :math:`O(n)` where :math:`n` is
     :cpp:any:`number_of_cols`

.. _dynamicmatrix_run_operator_more:

.. cpp:function:: bool operator>(DynamicMatrix const& that) const

   Greater than operator.

   This operator defines a total order on the set of matrices of the same type,
   the details of which is implementation specific.

   :param that: the matrix for comparison.

   :returns: 
     ``true`` if ``*this`` is less than ``that`` and ``false`` if it is not.  

   :complexity:
     At worst :math:`O(mn)` where :math:`m` is
     :cpp:any:`number_of_rows`
     and :math:`m` is :cpp:any:`number_of_cols`
