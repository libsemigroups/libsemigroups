.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

DynamicRowView member types
===========================

This page contains information about the member types of the
:cpp:any:`DynamicRowView` class.

.. _dynamicrowview_scalar_type:
.. cpp:type:: scalar_type = Scalar
    
   The type of the entries in the matrix.

.. _dynamicrowview_scalar_reference:
.. cpp:type:: scalar_reference = Scalar&
    
   The type of references to the entries in the matrix (might not be
   :code:`Scalar&`).

.. _dynamicrowview_scalar_const_reference:
.. cpp:type:: scalar_const_reference = Scalar const&
    
   The type of references to the entries in the matrix (might not be
   :code:`Scalar const&`).

.. _dynamicrowview_mat_type:
.. cpp:type:: matrix_type = DynamicMatrix
    
   The type of the matrix underlying a :cpp:any:`DynamicRowView`. 

.. _dynamicrowview_row_type:
.. cpp:type:: Row = Mat::Row

   The type of a row of a DynamicRowView. 

.. _dynamicrowview_iterator:
.. cpp:type:: iterator = Mat::iterator
    
   The type of iterators into the row view.

.. _dynamicrowview_const_iterator:
.. cpp:type:: const_iterator = Mat::const_iterator
    
   The type of const iterators into the row view.
