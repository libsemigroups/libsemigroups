.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

DynamicMatrix member types
==========================

.. cpp:namespace:: libsemigroups::DynamicMatrix<Semiring, Scalar>

This page contains information about the member types of the
:cpp:any:`DynamicMatrix` class.

.. _dynamicmatrix_run_scalar_type:
.. cpp:type:: scalar_type = Scalar
    
   The type of the entries in the matrix.

.. _dynamicmatrix_run_scalar_reference:
.. cpp:type:: scalar_reference = Scalar&
    
   The type of references to the entries in the matrix (might not be
   :code:`Scalar&`).

.. _dynamicmatrix_run_scalar_const_reference:
.. cpp:type:: scalar_const_reference = Scalar const&
    
   The type of references to the entries in the matrix (might not be
   :code:`Scalar const&`).

.. _dynamicmatrix_run_row_type:
.. cpp:type:: Row = DynamicMatrix    

   The type of a row of a :cpp:any:`DynamicMatrix`. 

.. _dynamicmatrix_run_rowview_type:
.. cpp:type:: RowView = DynamicRowView<Semiring, Scalar>
    
   The type of a row view into a :cpp:any:`DynamicMatrix`. 

.. _dynamicmatrix_run_semiring_type:
.. cpp:type:: semiring_type = Semiring
    
   The type of the semiring over which the matrix is defined, coincides with
   the template parameter ``Semiring``.
