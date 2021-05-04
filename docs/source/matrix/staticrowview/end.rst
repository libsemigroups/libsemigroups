.. Copyright (c) 2019-20, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticrowview_end:

end, cend
=========

.. cpp:namespace:: libsemigroups::StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>

.. cpp:function:: iterator end()

   Returns a (random access) iterator pointing one passed the last entry of the
   row.

   :parameters: (None)

   :returns:
     A value of type ``iterator``.
   
   :complexity: Constant
   
   :exceptions: 
     This function is ``noexcept`` and is guaranteed never to throw.   

.. cpp:function:: const_iterator cend()

   Returns a const (random access) iterator pointing one passed the last entry
   of the row.

   :parameters: (None)

   :returns:
     A value of type ``const_iterator``.
   
   :complexity: Constant
   
   :exceptions: 
     This function is ``noexcept`` and is guaranteed never to throw.   
