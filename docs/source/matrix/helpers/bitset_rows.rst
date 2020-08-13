.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

bitset_rows
===========

.. cpp:namespace:: libsemigroups::matrix_helpers

This page contains details of the function templates for finding the rows of a
boolean matrix as bitsets that belong to the namespace ``matrix_helper``.

The main function is:

:cpp:any:`template \<typename Mat, size_t R, size_t C, typename Container> void bitset_rows(Container&&, detail::StaticVector1\<BitSet\<C>, R>&)`

which converts the rows stored in the first argument to bitsets and appends
them to the second argument. 

There are several helper functions which allow the main function to be called
without specifying the second argument or where a matrix is given instead of a
container of rowviews.

The main function
-----------------

.. cpp:function:: template <typename Mat, size_t R, size_t C, typename Container> \
    void bitset_rows(Container&&                          views,                  \
                     detail::StaticVector1<BitSet<C>, R>& result) 
    
   Converts the rows represented by the items in ``views`` to bit sets.
   
   :tparam Mat: 
     the type of matrix. This must be a type so that :cpp:any:`IsBMat\<Mat>` is
     ``true``.
   :tparam R: 
     an upper bound for the number of rows in ``views``. This value must be at most
     :cpp:any:`BitSet<1>::max_size()`.
   :tparam C: 
     an upper bound for the number of columns in each row represented in
     ``views``. This value must be at most :cpp:any:`BitSet<1>::max_size()`.
   :tparam Container: 
     the type of the container ``views``, should be ``std::vector`` or
     ``detail::StaticVector1``.  

   :param views:  a container of ``Mat::RowView`` or ``std::vector<bool>``.
   :param result: a static vector of bitsets to contain the resulting bit sets. 

   :returns: (None). 

   :exceptions: 
     Throws if ``Container::push_back`` throws, or the :cpp:any:`BitSet`
     constructor throws. 

   :complexity:
     :math:`O(mn)` where :math:`m` is the number of rows in ``views`` and
     and :math:`n` is the number of columns in any vector in ``views``.
    
Helpers
-------

.. cpp:function:: template <typename Mat, size_t R, size_t C, typename Container> \
   auto bitset_rows(Container&& views) 

   Returns a :cpp:any:`detail::StaticVector1\<BitSet\<C>, R>` containing the
   rows in ``views`` converted to :cpp:any:`BitSet\<C>` s.
   
   :tparam Mat: 
     the type of matrix. This must be a type so that :cpp:any:`IsBMat\<Mat>` is
     ``true``.
   :tparam R: 
     an upper bound for the number of rows in ``views``. This value must be at most
     :cpp:any:`BitSet<1>::max_size()`.
   :tparam C: 
     an upper bound for the number of columns in each row represented in
     ``views``. This value must be at most :cpp:any:`BitSet<1>::max_size()`.
   :tparam Container: 
     the type of the container ``views``, should be ``std::vector`` or
     ``detail::StaticVector1``.  
   
   :param views:  a container of ``Mat::RowView`` or ``std::vector<bool>``.

   :returns: A value of type :cpp:any:`detail::StaticVector1\<BitSet\<C>, R>`. 
   
   :exceptions: 
     Throws if 
     :cpp:any:`void bitset_rows(Container&&, detail::StaticVector1\<BitSet\<C>, R>&)`
     throws, or ``std::bad_alloc`` is thrown by the
     :cpp:any:`detail::StaticVector1\<BitSet\<C>, R>` constructor throws. 
   
   :complexity:
     :math:`O(mn)` where :math:`m` is the number of rows in ``views`` and
     and :math:`n` is the number of columns in any vector in ``views``.


.. cpp:function:: template <typename Mat, size_t R, size_t C> \
   void bitset_rows(Mat const&                           x,   \
                    detail::StaticVector1<BitSet<C>, R>& result) 

   Computes the rows of the matrix ``x`` as bitsets and appends them to
   ``result``. 
   
   :tparam Mat: 
     the type of matrix. This must be a type so that :cpp:any:`IsBMat\<Mat>` is
     ``true``.
   :tparam R: 
     an upper bound for the number of rows in ``views``. This value must be at most
     :cpp:any:`BitSet<1>::max_size()`.
   :tparam C: 
     an upper bound for the number of columns in each row represented in
     ``views``. This value must be at most :cpp:any:`BitSet<1>::max_size()`.

   :param x: the boolean matrix.
   :param result: the container to append the rows of ``x`` to.
   
   :returns: (None).

   :exceptions: 
     Throws if 
     :cpp:any:`void bitset_rows(Container&&, detail::StaticVector1\<BitSet\<C>, R>&)`
     throws.
   
   :complexity:
     :math:`O(mn)` where :math:`m` is the number of rows in ``x`` and
     and :math:`n` is the number of columns in ``x``.
    
.. cpp:function:: template <typename Mat> \
                  auto bitset_rows(Mat const& x)

   Computes the rows of the matrix ``x`` as bitsets.
   
   :tparam Mat: 
     the type of matrix. This must be a type so that :cpp:any:`IsBMat\<Mat>` is
     ``true``.
   
   :param x: the boolean matrix.
   :returns: A value of type :cpp:any:`detail::StaticVector1\<BitSet\<C>, R>`. 
   
   :exceptions: 
     Throws if 
     :cpp:any:`void bitset_rows(Container&&, detail::StaticVector1\<BitSet\<C>, R>&)`
     throws, or ``std::bad_alloc`` is thrown by the
     :cpp:any:`detail::StaticVector1\<BitSet\<C>, R>` constructor throws. 

   :complexity:
     :math:`O(mn)` where :math:`m` is the number of rows in ``x`` and
     and :math:`n` is the number of columns in ``x``.
