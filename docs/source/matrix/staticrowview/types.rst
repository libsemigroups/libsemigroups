.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

StaticRowView member types
==========================

.. cpp:namespace:: libsemigroups::StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>

This page contains information about the member types of the
:cpp:any:`StaticRowView` class.

.. _staticrowview_scalar_type:
.. cpp:type:: scalar_type = Scalar
    
   The type of the entries in the matrix.

.. _staticrowview_scalar_reference:
.. cpp:type:: scalar_reference = Scalar&
    
   The type of references to the entries in the matrix (might not be
   :code:`Scalar&`).

.. _staticrowview_scalar_const_reference:
.. cpp:type:: scalar_const_reference = Scalar const&
    
   The type of references to the entries in the matrix (might not be
   :code:`Scalar const&`).

.. _staticrowview_mat_type:
.. cpp:type:: matrix_type = StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>
    
   The type of the matrix underlying a StaticRowView. 

.. _staticrowview_row_type:
.. cpp:type:: Row = Mat::Row

   The type of a row of a StaticRowView. 

.. _staticrowview_iterator:
.. cpp:type:: iterator = Mat::iterator
    
   The type of iterators into the row view.

.. _staticrowview_const_iterator:
.. cpp:type:: const_iterator = Mat::const_iterator
    
   The type of const iterators into the row view.
