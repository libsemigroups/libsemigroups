.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

IncreaseDegree
==============

.. cpp:namespace:: libsemigroups

.. cpp:struct:: template <typename T> \
                IncreaseDegree<T, std::enable_if_t<IsMatrix<T>>> 

   Defined in ``matrix.hpp``.
   
   Specialization of the adapter :cpp:any:`IncreaseDegree` for types ``T`` such
   that the value of :cpp:any:`IsMatrix\<T>` is ``true``.  
   
   :tparam T: the type of matrices.

   .. warning:: 
     It is not possible to increase the degree of any of the types for which
     :cpp:any:`IsMatrix\<T>` is ``true``, and as such the call operator of this
     type does nothing. 
   
   
   .. cpp:function:: constexpr void operator()(T&, size_t) const noexcept

      Returns (None). 
      
      :parameters: (None)

      :returns: Does not do anything, do not use.

      :exceptions:
        This function is ``noexcept`` and is guaranteed never to throw.

      :complexity: Constant.

