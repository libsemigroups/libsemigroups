.. Copyleft (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Rank
====

This page contains details of the specialisations of the :ref:`adapters<Adapters>`
:cpp:any:`Rank` and :cpp:any:`RankState` for boolean matrices. 

.. cpp:class:: template <typename Mat> \
               RankState<Mat>

   This class is a specialization of the adapter :cpp:any:`RankState` for
   instances of :cpp:any:`BMat<N>`. The :cpp:any:`RankState` is used as part of
   the computation of the rank of a boolean matrix in :cpp:any:`Rank<Mat>`
   which is used by :cpp:any:`Konieczny`. 
   
   :tparam Mat: 
     a type such that :cpp:any:`IsBMat<Mat>` is ``true``. 

   .. cpp:type:: MaxBitSet = BitSet<BitSet<1>::max_size()>
      
      Type of the maximum possible size of :cpp:any:`BitSet`. 
   
   .. cpp:type:: type = RightAction<Mat,       \
                                    MaxBitSet, \
                                    ImageRightAction<Mat, MaxBitSet>>

      The type of additional data used by :cpp:any:`Rank<Mat>`: the right
      action consisting of all possible rows of matrices belonging to the
      underlying semigroup.
   
   .. cpp:function:: RankState() = default
      
      Default constructor. The other standard constructors (copy constructor,
      move constructor, etc) are deleted. 
    
   .. cpp:function:: template <typename T>      \
                     RankState(T first, T last) 

      Construct from const iterators to the generators of the semigroup.

      :tparam T: type of const iterators to the generators of the semigroup

      :param first: const iterator to the first generator

      :param last:  const iterator pointing one passed the last generator

      :throws: 
        :cpp:any:`LibsemigroupsException` if the distance between
        ``first`` and ``last`` is ``0``. 
  
   .. cpp:function:: type const& get() const 
 
      Returns the fully enumerated row orbit.
 
      :parameters: (None)
 
      :returns: 
        A const reference to the fully enumerated row orbit, a value of type
        :cpp:any:`type`.
 
      :complexity:
        :math:`O(mn)` where :math:`m` is the number of generators added at the
        time of construction (i.e. ``std::distance(first, last)``) and :math:`n`
        is the size of the fully enumerated row orbit.


.. cpp:struct:: template <typename Mat> \
                Rank<Mat, RankState<Mat>>

   Specialization of the adapter :cpp:any:`Rank` for instances of
   :cpp:any:`BMat<N>`. 

   :tparam Mat: 
     a type such that :cpp:any:`IsBMat<Mat>` is ``true``. 
  
   .. cpp:function:: size_t operator()(RankState<Mat> const& state, Mat const& x) const

      Returns the rank of ``x``.

      :param state: a const reference to the `RankState<Mat>`

      :param x: the matrix

      :returns: 
        the rank of the parameter ``x``, which is a value of type ``size_t``.

      :complexity:
        The first call has complexity at worst :math:`O(mn)` where :math:`m` is
        the number of generators added at the time of construction (i.e.
        ``std::distance(first, last)``) and :math:`n` is the size of the fully
        enumerated row orbit, subsequent calls have constant complexity.
