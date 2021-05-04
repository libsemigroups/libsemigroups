.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

StaticMatrix member types
=========================

.. cpp:namespace:: libsemigroups::StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>

This page contains information about the member types of the
:cpp:any:`StaticMatrix` class.

.. _staticmatrix_scalar_type:
.. cpp:type:: scalar_type = Scalar
    
   The type of the entries in the matrix.

.. _staticmatrix_scalar_reference:
.. cpp:type:: scalar_reference = Scalar&
    
   The type of references to the entries in the matrix (might not be
   :code:`Scalar&`).

.. _staticmatrix_scalar_const_reference:
.. cpp:type:: scalar_const_reference = Scalar const&
    
   The type of references to the entries in the matrix (might not be
   :code:`Scalar const&`).

.. _staticmatrix_row_type:
.. cpp:type:: Row = StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, 1, C, Scalar>
    
   The type of a row of a :cpp:any:`StaticMatrix`. 

.. _staticmatrix_rowview_type:
.. cpp:type:: RowView = StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>
    
   The type of a row view into a :cpp:any:`StaticMatrix`. 

.. _staticmatrix_plus:
.. cpp:type:: Plus = PlusOp
    
   The template parameter ``PlusOp``.

.. _staticmatrix_prod:
.. cpp:type:: Prod = ProdOp
    
   The template parameter ``ProdOp``.

.. _staticmatrix_zero:
.. cpp:type:: Zero = ZeroOp
    
   The template parameter ``ZeroOp``.

.. _staticmatrix_one:
.. cpp:type:: One = OneOp
    
   The template parameter ``OneOp``.

.. _staticmatrix_semiring_type:
.. cpp:type:: semiring_type = void
    
   The type of the semiring over which the matrix is defined is ``void``
   because there's no semiring object, the arithmetic is defined by the
   template paramaters.
