.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

row_basis
=========

.. cpp:namespace:: libsemigroups::matrix_helpers

This page contains details of the function templates for finding the row basis
of certain types of matrix in the namespace ``matrix_helper``.

.. cpp:function:: template <typename Mat,                               \
                            typename Container,                         \
                            typename = std::enable_if_t<IsMatrix<Mat>>> \
                  void row_basis(Mat const& x, Container& result)

   Appends a basis (consisting of row views) for the row space of the matrix
   ``x`` to the container ``result``. 

   :tparam Mat: 
     a type such that the value of :cpp:any:`IsMatrix<Mat>` is ``true``.

   :tparam Container: 
     a container type (such as ``std::vector``, for example).

   :param x: the matrix.

   :param result: the container to append the basis to the row space to.

   :returns: (None)

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`.
  
   :complexity: 
      :math:`O(r ^ 2 c)` where :math:`r` is the number of rows in ``x`` and
      :math:`c` is the number of columns in ``x``. 

.. cpp:function:: template <typename Mat,                                      \
                            typename = std::enable_if_t<IsDynamicMatrix<Mat>>> \
                  std::vector<typename Mat::RowView> row_basis(Mat const& x)
   
   Returns a vector (consisting of row views) that is a basis for the row
   space of the non-static matrix ``x``. 

   :tparam Mat: 
     a type such that the value of :cpp:any:`IsDynamicMatrix<Mat>` is ``true``.

   :param x: the matrix.

   :returns:
     A ``std::vector`` of :cpp:any:`Mat::RowView`.

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`.
   
   :complexity: 
      :math:`O(r ^ 2 c)` where :math:`r` is the number of rows in ``x`` and
      :math:`c` is the number of columns in ``x``. 
    

.. cpp:function:: template <typename Mat,                                     \
                            typename = std::enable_if_t<IsStaticMatrix<Mat>>> \
                  detail::StaticVector1<typename Mat::RowView, Mat::nr_rows>  \
                  row_basis(Mat const& x)
   
   Returns a static vector (consisting of row views) that is a basis for the row
   space of the static matrix ``x``. 

   :tparam Mat: 
     a type such that the value of :cpp:any:`IsStaticMatrix<Mat>` is ``true``.

   :param x: the matrix.

   :returns:
     A static vector of :cpp:any:`Mat::RowView`.

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`.
    
   :complexity: 
      :math:`O(r ^ 2 c)` where :math:`r` is the number of rows in ``x`` and
      :math:`c` is the number of columns in ``x``. 


.. cpp:function:: template <typename Mat, typename Container> \
                  void row_basis(Container& views, Container& result) 

   Appends a basis for the space spanned by the row 
   views or bit sets in ``views`` to the container ``result``. 
   
   :tparam Mat: 
     a type such that the value of :cpp:any:`IsMaxPlusTruncMat\<Mat>` or 
     :cpp:any:`IsBMat\<Mat>` is ``true``.

   :tparam Container: 
     a container type (such as ``std::vector``, for example). The
     ``Container::value_type`` must be ``Mat::RowView``, or if ``Mat`` is
     ``BMat<N>`` for some ``N``, then  ``Container::value_type`` can
     additionally be ``BitSet<M>`` or ``std::bitset<M>`` where ``M`` is greater
     than or equal to ``N``. 

   :param views:  container of spanning row views or bit sets

   :param result: container for the resulting row basis

   :returns: (None)

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`.
    
   :complexity: 
      :math:`O(r ^ 2 c)` where :math:`r` is the size of ``views`` and
      :math:`c` is the size of each row view or bit set in ``views``. 
