.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

DynamicMatrix (with run time arithmetic)
========================================

.. highlight:: cpp

Defined in ``matrix.hpp``.

.. cpp:class:: template <typename Semiring, \
                         typename Scalar>   \
   DynamicMatrix final

   This is a class for matrices where both the arithmetic operations in the
   underlying semiring and the dimensions of the matrix can be set at run time.

   :tparam Semiring: 
      the type of a semiring object which defines the semiring arithmetic (see
      requirements below).

   :tparam Scalar: 
      the type of the entries in the matrices (the type of elements in the
      underlying semiring)

   .. note::
      Certain member functions only work for square matrices and some only work
      for rows. 

   **Semiring requirements**
    
   The template parameter :cpp:any:`Semiring` must have the following member
   functions:

   * ``scalar_type zero()`` that returns the multiplicative zero scalar in
     the semiring
   * ``scalar_type one()`` that returns the multiplicative identity scalar in
     the semiring
   * ``scalar_type plus(scalar_type x, scalar_type y)``
     that returns the sum in the semiring of the scalars ``x`` and ``y``.
   * ``scalar_type prod(scalar_type x, scalar_type y)``
     that returns the product in the semiring of the scalars ``x`` and ``y``.

   See, for example, :cpp:any:`MaxPlusTruncSemiring`. 

.. toctree::
   :hidden:

   dynamicmatrix-run/begin
   dynamicmatrix-run/call_operator
   dynamicmatrix-run/comparison_ops
   dynamicmatrix-run/constructors
   dynamicmatrix-run/coords
   dynamicmatrix-run/end
   dynamicmatrix-run/number_of_rows
   dynamicmatrix-run/operator_plus
   dynamicmatrix-run/operator_plus_equals
   dynamicmatrix-run/operator_prod
   dynamicmatrix-run/operator_prod_equals
   dynamicmatrix-run/row
   dynamicmatrix-run/rows
   dynamicmatrix-run/semiring
   dynamicmatrix-run/staticmemfn
   dynamicmatrix-run/swap
   dynamicmatrix-run/transpose
   dynamicmatrix-run/types

Member types
------------

.. list-table:: 
   :widths: 50 50
   :header-rows: 1

   * - Member type
     - Definition
   * - :ref:`scalar_type<dynamicmatrix_run_scalar_type>`
     - the type of scalars contained in the matrix (:code:`Scalar`)
   * - :ref:`scalar_reference<dynamicmatrix_run_scalar_reference>`
     - the type of references to scalars contained in the matrix
   * - :ref:`scalar_const_reference<dynamicmatrix_run_scalar_const_reference>`
     - the type of const references to scalars contained in the matrix
   * - :ref:`Row<dynamicmatrix_run_row_type>`
     - type of a row of the matrix 
   * - :ref:`RowView<dynamicmatrix_run_rowview_type>`
     - type of a row view int the matrix 
   * - :ref:`semiring_type<dynamicmatrix_run_semiring_type>`
     - the template parameter ``Semiring``.

Member functions
----------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`(constructor)<dynamicmatrix_run_constructors>`
     - constructs the matrix
   * - :ref:`number_of_rows/number_of_cols<dynamicmatrix_run_number_of_rows>`
     - the number of rows/columns in the matrix
   * - :ref:`semiring<dynamicmatrix_run_semiring>`
     - returns a pointer to the semiring defining the arithmetic

Static member functions
-----------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`make<dynamicmatrix_run_make>`
     - constructs a matrix and checks that its entries are valid
   * - :ref:`identity<dynamicmatrix_run_identity>`
     - constructs an identity matrix

Iterators
---------

.. list-table:: 
   :widths: 50 50

   * - :ref:`begin/cbegin<dynamicmatrix_run_begin>`
     - returns an iterator to the beginning
   * - :ref:`end/cend<dynamicmatrix_run_end>`
     - returns an iterator to the end
   * - :ref:`coords<dynamicmatrix_run_coords>`
     - returns the coordinates corresponding to an iterator

Accessors
---------

.. list-table:: 
   :widths: 50 50
   
   * - :ref:`operator()<dynamicmatrix_run_call_operator>`
     - access specified element
   * - :ref:`row<dynamicmatrix_run_row>`
     - access specified row
   * - :ref:`rows<dynamicmatrix_run_rows>`
     - add all rows to a container

Arithmetic operators
--------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`operator+=<dynamicmatrix_run_operator_plus_equals>`
     - add a (matrix, scalar, or row view) to the matrix in place
   * - :ref:`operator\*=<dynamicmatrix_run_operator_prod_equals>`
     - multiply the matrix by a matrix or scalar in place
   * - :ref:`operator+<dynamicmatrix_run_operator_plus>`
     - add a (matrix, scalar, or row view) to the matrix
   * - :ref:`operator*<dynamicmatrix_run_operator_prod>`
     - multiply the matrix by a matrix or scalar

Comparison operators
--------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`operator==<dynamicmatrix_run_operator_equals>`
     - compare matrices
   * - :ref:`operator!=<dynamicmatrix_run_operator_not_equals>`
     - compare matrices
   * - :ref:`operator\<<dynamicmatrix_run_operator_less>`
     - compare matrices
   * - :ref:`operator\><dynamicmatrix_run_operator_more>`
     - compare matrices
   
Modifiers
---------

.. list-table:: 
   :widths: 50 50

   * - :ref:`swap<dynamicmatrix_run_swap>`
     - swaps the contents
   * - :ref:`transpose<dynamicmatrix_run_transpose>`
     - transposes the matrix
