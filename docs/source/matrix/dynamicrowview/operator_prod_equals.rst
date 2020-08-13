.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicrowview_operator_prod_equals:

operator*=
==========

.. cpp:function:: void operator*=(scalar_type const a)

   Multiplies every entry of the row by a scalar in-place.

   :param a: the scalar to add to every entry of ``this``.
    
   :returns: (None)

   :throws: 
      If the implementation of the semiring multiplication throws.
   
   :complexity: 
      :math:`O(m)` where :math:`m` is :cpp:any:`size`
