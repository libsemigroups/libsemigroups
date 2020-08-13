.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

bitset_row_basis
================

.. cpp:namespace:: libsemigroups::matrix_helpers

This page contains details of the function templates for finding the row basis
of a boolean matrix represented as bitsets that belong to the namespace
``matrix_helper``.

.. cpp:function:: template <typename Mat, typename Container> \
   void bitset_row_basis(Container&& rows, std::decay_t<Container>& result) 

   Appends a basis for the space spanned by the bitsets in ``rows`` to the
   container ``result``. 
   
   :tparam Mat: 
     a type such that :cpp:any:`IsBMat\<Mat>` is ``true``.

   :tparam Container: 
     a container type (such as ``detail::StaticVector1``, for example). The
     ``Container::value_type`` must be ``BitSet<M>`` or ``std::bitset<M>``
     for some ``M``.

   :param rows:  container of spanning rows represented by bitsets.

   :param result: container for the resulting rowbasis

   :returns: (None)

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`.
    
   :complexity: 
      :math:`O(r ^ 2 c)` where :math:`r` is the size of ``rows`` and
      :math:`c` is the size of each bitset in ``rows``. 

Helper functions
----------------

.. cpp:function:: template <typename Mat, typename Container> \
   std::decay_t<Container> bitset_row_basis(Container&& rows) 

   Returns a basis for the space spanned by the bitsets in ``rows``.

   :tparam Mat: 
     a type such that :cpp:any:`IsBMat\<Mat>` is ``true``.

   :tparam Container: 
     a container type (such as ``detail::StaticVector1``, for example). The
     ``Container::value_type`` must be ``BitSet<M>`` or ``std::bitset<M>``
     for some ``M``.

   :param rows:  container of spanning rows represented by bitsets.

   :returns: 
     A container of type ``std::decay_t<Container>`` containing the row basis
     consisting of bitsets.

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`.
    
   :complexity: 
      :math:`O(r ^ 2 c)` where :math:`r` is the size of ``rows`` and
      :math:`c` is the size of each bitset in ``rows``. 
    
.. cpp:function:: template <typename Mat,                                  \
                            size_t M = detail::BitSetCapacity<Mat>::value> \
   detail::StaticVector1<BitSet<M>, M> bitset_row_basis(Mat const& x)

   Returns a basis for the space spanned by the rows of the boolean matrix
   ``x``.

   :tparam Mat: 
     a type such that :cpp:any:`IsBMat\<Mat>` is ``true``.

   :tparam M: 
     an upper bound for the dimensions of the returned container. If
     :cpp:any:`IsStaticMatrix\<Mat>` is ``true``, then ``M`` is the number of
     rows (or columns) in the square matrix ``x``. Otherwise, if
     :cpp:any:`IsDynamicMatrix\<Mat>` is ``true``, then ``M`` is
     :cpp:any:`BitSet<1>::max_size()`. 

   :param x: the boolean matrix.

   :returns: 
     A container of type ``detail::StaticVector1<BitSet<M>, M>>`` containing
     the row basis of ``x`` consisting of bitsets.

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`.
    
   :complexity: 
      :math:`O(r ^ 2 c)` where :math:`r` is the number of rows in ``x`` and
      :math:`c` is the number of columns in ``x``. 

    
.. cpp:function:: template <typename Mat, typename Container> \
   void bitset_row_basis(Mat const& x, Container& result) 
   
   Appends a basis for the rowspace of the boolean matrix ``x`` to the
   container ``result``. 
   
   :tparam Mat: 
     a type such that :cpp:any:`IsBMat\<Mat>` is ``true``.

   :tparam Container: 
     a container type (such as ``detail::StaticVector1``, for example). The
     ``Container::value_type`` must be ``BitSet<M>`` or ``std::bitset<M>``
     for some ``M``.

   :param x: the boolean matrix.

   :param result: container for the resulting rowbasis

   :returns: (None)

   :exceptions: 
     This function guarantees not to throw a :cpp:any:`LibsemigroupsException`.
    
   :complexity: 
      :math:`O(r ^ 2 c)` where :math:`r` is the number of rows in ``x`` and
      :math:`c` is the number of columns in ``x``. 
