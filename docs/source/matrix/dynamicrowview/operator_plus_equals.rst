.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicrowview_operator_plus_equals:

operator+=
==========

.. cpp:function:: void operator+=(DynamicRowView const& that)

   Sums a row view with another row view in-place.

   :param that: the row view to add to ``this``.
    
   :returns: (None)

   :throws: 
      If the implementation of the semiring addition throws.

   :complexity: 
      :math:`O(m)` where :math:`m` is :cpp:any:`size`

   .. warning::
      The two row views must be of the same size, although this is not
      verified by the implementation.


.. cpp:function:: void operator+=(scalar_type const a)

   Adds a scalar to every entry of the row in-place.
   
   :param a: the scalar to add to ``this``.
   
   :returns: (None)

   :complexity: 
      :math:`O(m)` where :math:`m` is :cpp:any:`size`
   
   :throws: 
      If the implementation of the semiring addition throws.
