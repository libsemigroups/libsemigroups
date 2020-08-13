.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Product
=======

.. cpp:namespace:: libsemigroups

.. cpp:struct:: template <typename T> \
                Product<T, std::enable_if_t<IsMatrix<T>>>

   Defined in ``matrix.hpp``.
   
   Specialization of the adapter :cpp:any:`Product` for types ``T`` such
   that the value of :cpp:any:`IsMatrix\<T>` is ``true``. 
   
   :tparam: T the type of matrices.
   
   .. cpp:function:: inline void operator()(T& xy, T const& x, T const& y, size_t = 0) const 

      Replaces the value of ``xy`` by the product of the matrices ``x`` and ``y``.

      :param xy: a reference to a matrix of type ``T``.
      :param x: a const reference to a matrix of type ``T``.
      :param y: a const reference to a matrix of type ``T``.

      :returns: (None)

      :exceptions:
        This function guarantees not to throw a
        :cpp:any:`LibsemigroupsException`.

      :complexity:
       :math:`O(m ^ 2)` where :math:`m` is the number of rows of the matrix ``x``.
     
      .. warning:: 
         This function only works for square matrices.
        
      .. seealso:: 
        For further details see: :cpp:any:`void
        StaticMatrix::product_inplace(StaticMatrix const& A, StaticMatrix
        const& B)`, :cpp:any:`void DynamicMatrix::product_inplace(DynamicMatrix
        const& A, DynamicMatrix const& B)`.
