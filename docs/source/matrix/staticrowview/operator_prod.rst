.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticrowview_operator_prod:

operator*
=========

.. cpp:namespace:: libsemigroups::StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>

.. cpp:function:: Row operator*(scalar_type const a) const

   Multiplies every entry of the row by a scalar.

   :param a: the scalar.

   :returns: 
     A value of type Row containing the original row multiplied by the scalar
     ``a``.
    
   :complexity: 
      :math:`O(m)` where :math:`m` is :cpp:any:`size`

   :throws: 
      If the implementation of the semiring addition throws or
      ``std::bad_alloc`` is thrown.
