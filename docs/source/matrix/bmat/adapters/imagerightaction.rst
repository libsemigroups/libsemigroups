.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.


ImageRightAction
================

.. cpp:namespace:: libsemigroups

.. cpp:struct:: template <typename Container, typename Mat> \
                ImageRightAction<Mat, Container>
  
   This struct is a specialisation of :cpp:any:`ImageRightAction` for boolean
   matrices and containers of bitsets. 

   :tparam Container: 
     a container type where :cpp:any:`IsBitSet\<typename Container::value_type>`
     is ``true``. For example, ``Container`` might be
     ``detail::StaticVector1<BitSet<N>, N>`` or ``std::vector<BitSet<N>>`` for
     some value of ``N`` with :math:`0\leq N\leq 64`. 
   
   :tparam Mat: 
     a type such that :cpp:any:`IsBMat<Mat>` is ``true``. 

   .. cpp:function:: void operator()(Container& res,      \
                                     Container const& pt, \ 
                                     Mat const& x) const
      
      Given a row basis ``pt`` and a boolean matrix ``x``, this function
      computes the row basis of the space obtained by right multiplication of
      the rows in ``pt`` by the matrix ``x``, and modifies ``res`` to contain
      the resulting row basis.

      :param res: container to store the result.

      :param pt:  the container on which to act.

      :param x:   the matrix which is acting. 

      :returns: (None). 

      :complexity: 
         :math:`O(r ^ 2 c)` where :math:`r` is the number of rows in ``pt`` and
         :math:`c` is the number of columns in ``x``. 

.. cpp:struct:: template <size_t N, typename Mat> \
                ImageRightAction<Mat, BitSet<N>> 

   This struct is a specialisation of :cpp:any:`ImageRightAction` for boolean
   matrices and bitsets. 

   :tparam N: 
     a value that exceeds the number of rows in any instance of ``Mat`` passed
     to the call operator. 

   :tparam Mat: 
     a type such that :cpp:any:`IsBMat<Mat>` is ``true``. 

   .. cpp:type:: result_type = BitSet<N>
      
      Alias for the type of the result of the call operator. 

   .. cpp:function:: void operator()(result_type& res,      \
                                     result_type const& pt, \ 
                                     Mat const& x) const
      
      Given a bitset ``pt`` (representing a row of a boolean matrix) and a
      boolean matrix ``x``, this function computes the row obtained by right
      multiplying ``pt`` by the matrix ``x``, and modifies ``res`` to contain
      the resulting bitset.

      :param res: bitset to contain the result.

      :param pt:  the bitset on which to act.

      :param x:   the matrix which is acting. 

      :returns: (None). 

      :complexity: 
         :math:`O(r ^ 2)` where :math:`r` is the number of rows in ``x``. 
