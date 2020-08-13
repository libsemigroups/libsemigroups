.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicrowview_operator_plus:

operator+
=========

.. cpp:function:: Row operator+(StaticRowView const& that)

   Sums a row view with another row view and returns a newly allocated Row. 

   :param that: the row view to add to ``this``.
    
   :returns: A value of type :cpp:any:`Row`

   :throws: 
      If the implementation of the semiring addition throws or
      ``std::bad_alloc`` is thrown.

   :complexity: 
      :math:`O(m)` where :math:`m` is :cpp:any:`size`

   .. warning::
      The two row views must be of the same size, although this is not
      verified by the implementation.
