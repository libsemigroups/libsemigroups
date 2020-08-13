.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

DynamicMatrix member types
==========================

.. cpp:namespace:: libsemigroups::DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>

This page contains information about the member types of the
:cpp:any:`DynamicMatrix` class.

.. _dynamicmatrix_compile_scalar_type:
.. cpp:type:: scalar_type = Scalar
    
   The type of the entries in the matrix.

.. _dynamicmatrix_compile_scalar_reference:
.. cpp:type:: scalar_reference = Scalar&
    
   The type of references to the entries in the matrix (might not be
   :code:`Scalar&`).

.. _dynamicmatrix_compile_scalar_const_reference:
.. cpp:type:: scalar_const_reference = Scalar const&
    
   The type of references to the entries in the matrix (might not be
   :code:`Scalar const&`).

.. _dynamicmatrix_compile_row_type:
.. cpp:type:: Row = DynamicMatrix    

   The type of a row of a :cpp:any:`DynamicMatrix` . 

.. _dynamicmatrix_compile_rowview_type:
.. cpp:type:: RowView = DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>
    
   The type of a row view into a :cpp:any:`DynamicMatrix`. 

.. _dynamicmatrix_compile_plus:
.. cpp:type:: Plus = PlusOp
    
   The template parameter ``PlusOp``.

.. _dynamicmatrix_compile_prod:
.. cpp:type:: Prod = ProdOp
    
   The template parameter ``ProdOp``.

.. _dynamicmatrix_compile_zero:
.. cpp:type:: Zero = ZeroOp
    
   The template parameter ``ZeroOp``.

.. _dynamicmatrix_compile_one:
.. cpp:type:: One = OneOp
    
   The template parameter ``OneOp``.

.. _dynamicmatrix_compile_semiring_type:
.. cpp:type:: semiring_type = void
    
   The type of the semiring over which the matrix is defined is ``void``
   because there's no semiring object, the arithmetic is defined by the
   template paramaters.
