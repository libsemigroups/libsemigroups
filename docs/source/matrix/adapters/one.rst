.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

One
======

.. cpp:namespace:: libsemigroups

.. cpp:struct:: template <typename T> \
                One<T, std::enable_if_t<IsMatrix<T>>>

   Defined in ``matrix.hpp``.
   
   Specialization of the adapter :cpp:any:`One` for types ``T`` such
   that the value of :cpp:any:`IsMatrix\<T>` is ``true``. 
   
   :tparam: T the type of matrices.
   
   .. cpp:function:: inline T operator()(T const& x) const 
      
      Returns the identity matrix. 

      :param x: a matrix of type ``T``.

      :returns: a matrix of type ``T``.

      :exceptions:
        This function guarantees not to throw a
        :cpp:any:`LibsemigroupsException`.

     :complexity: 
       :math:`O(m ^ 2)` where :math:`m` is the number of rows of the matrix ``x``.

     .. warning:: 
        This function only works for square matrices.

     .. seealso:: 
       See :cpp:any:`static StaticMatrix StaticMatrix::identity()`, or
       :cpp:any:`static DynamicMatrix DynamicMatrix::identity(size_t n)` for
       further details.
