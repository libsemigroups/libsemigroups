.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

rows
====

.. cpp:namespace:: libsemigroups::matrix_helpers

This page contains details of the function templates for finding the rows of a
matrix in the namespace ``matrix_helper``.

For dynamic matrices
--------------------

.. cpp:function:: template <typename T,                                      \
                            typename = std::enable_if_t<IsDynamicMatrix<T>>> \ 
                  std::vector<typename T::RowView>                           \
                  rows(T const& x)                      
                  
   Returns a vector of row views into all of the rows of the dynamic matrix
   ``x``. 

   :param x: the matrix.

   :returns: A vector of ``T::RowView`` of size ``x.number_of_rows()``.

   :exceptions: Throws if ``T::rows`` throws, or ``std::bad_alloc`` is thrown. 

   :complexity:
     :math:`O(m)` where :math:`m` is the number of rows in the matrix ``x``.

For static matrices
-------------------
   
.. cpp:function:: template <typename T, typename = std::enable_if_t<IsStaticMatrix<T>>> \
                  detail::StaticVector1<typename T::RowView, T::number_of_rows> rows(T const& x)
   
   Returns a static vector of row views into all of the rows of the static matrix
   ``x``. Static vectors have the same interface as ``std::vector`` but their
   capacity is defined at compile time. 

   :param x: the matrix

   :returns: A static vector of ``T::RowView`` of size ``x.number_of_rows()``.

   :exceptions: Throws if ``T::rows`` throws, or ``std::bad_alloc`` is thrown. 

   :complexity:
     :math:`O(m)` where :math:`m` is the number of rows in the matrix ``x``.
