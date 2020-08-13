.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Complexity
==========

.. cpp:namespace:: libsemigroups

.. cpp:struct:: template <typename T> \
                Complexity<T, std::enable_if_t<IsMatrix<T>>>

   Defined in ``matrix.hpp``.
   
   Specialization of the adapter :cpp:any:`Complexity` for types ``T`` such
   that the value of :cpp:any:`IsMatrix\<T>` is ``true``. 
   
   :tparam: T the type of matrices.
   
   .. cpp:function:: constexpr size_t operator()(T const& x) const noexcept
      
      Returns ``x.number_of_rows()`` cubed. 

      :param x: a matrix of type ``T``.

      :returns: a value of type ``size_t``.

      :exceptions:
        This function is ``noexcept`` and is guaranteed never to throw.

      :complexity: Constant.
