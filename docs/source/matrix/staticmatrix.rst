.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

StaticMatrix
============

.. highlight:: cpp

Defined in ``matrix.hpp``.

.. cpp:class:: template <typename PlusOp, \
                         typename ProdOp, \
                         typename ZeroOp, \
                         typename OneOp,  \
                         size_t R,        \
                         size_t C,        \
                         typename Scalar> \
   StaticMatrix final
   :tparam-line-spec:

   This is a class for matrices where both the arithmetic operations in the
   underlying semiring, and the dimensions of the matrices are known at compile
   time.

   :tparam PlusOp: 
      a stateless type with a call operator of signature :code:`scalar_type
      operator()(scalar_type, scalar_type)` implementing the addition of the
      semiring

   :tparam ProdOp: 
      a stateless type with a call operator of signature :code:`scalar_type
      operator()(scalar_type, scalar_type)` implementing the multiplication of
      the semiring

   :tparam ZeroOp: 
      a stateless type with a call operator of signature :code:`scalar_type
      operator()()` returning the zero of the semiring (the additive identity
      element)

   :tparam OneOp: 
      a stateless type with a call operator of signature :code:`scalar_type
      operator()()` returning the one of the semiring (the multiplicative
      identity element)

   :tparam R: the number of rows of the matrices

   :tparam C: the number of columns of the matrices

   :tparam Scalar: 
      the type of the entries in the matrices (the type of elements in the
      underlying semiring)

   .. note::
      Certain member functions only work for square matrices and some only work
      for rows. 

.. toctree::
   :hidden:

   staticmatrix/constructors
   staticmatrix/begin
   staticmatrix/end
   staticmatrix/number_of_rows
   staticmatrix/call_operator
   staticmatrix/operator_plus_equals
   staticmatrix/operator_prod_equals
   staticmatrix/operator_plus
   staticmatrix/operator_prod
   staticmatrix/comparison_ops
   staticmatrix/coords
   staticmatrix/swap
   staticmatrix/transpose
   staticmatrix/rows
   staticmatrix/row
   staticmatrix/types
   staticmatrix/staticmemfn

Member types
------------

.. list-table:: 
   :widths: 50 50
   :header-rows: 1

   * - Member type
     - Definition
   * - :ref:`scalar_type<staticmatrix_scalar_type>`
     - the type of scalars contained in the matrix (:code:`Scalar`)
   * - :ref:`scalar_reference<staticmatrix_scalar_reference>`
     - the type of references to scalars contained in the matrix
   * - :ref:`scalar_const_reference<staticmatrix_scalar_const_reference>`
     - the type of const references to scalars contained in the matrix
   * - :ref:`Row<staticmatrix_row_type>`
     - type of a row of the matrix 
   * - :ref:`RowView<staticmatrix_rowview_type>`
     - type of a row view int the matrix 
   * - :ref:`Plus<staticmatrix_plus>`
     - the template parameter :cpp:any:`PlusOp`
   * - :ref:`Prod<staticmatrix_prod>`
     - the template parameter :cpp:any:`ProdOp`
   * - :ref:`Zero<staticmatrix_zero>`
     - the template parameter :cpp:any:`ZeroOp`
   * - :ref:`One<staticmatrix_one>`
     - the template parameter :cpp:any:`OneOp`
   * - :ref:`semiring_type<staticmatrix_semiring_type>`
     - ``void``

Member functions
----------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`(constructor)<staticmatrix_constructors>`
     - constructs the matrix
   * - :ref:`number_of_rows/number_of_cols<staticmatrix_number_of_rows>`
     - the number of rows/columns in the matrix

Static member functions
-----------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`make<staticmatrix_make>`
     - constructs a matrix and checks that its entries are valid
   * - :ref:`identity<staticmatrix_identity>`
     - constructs an identity matrix

Iterators
---------

.. list-table:: 
   :widths: 50 50

   * - :ref:`begin/cbegin<staticmatrix_begin>`
     - returns an iterator to the beginning
   * - :ref:`end/cend<staticmatrix_end>`
     - returns an iterator to the end
   * - :ref:`coords<staticmatrix_coords>`
     - returns the coordinates corresponding to an iterator

Accessors
---------

.. list-table:: 
   :widths: 50 50
   
   * - :ref:`operator()<staticmatrix_call_operator>`
     - access specified element
   * - :ref:`row<staticmatrix_row>`
     - access specified row
   * - :ref:`rows<staticmatrix_row>`
     - add all rows to a container

Arithmetic operators
--------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`operator+=<staticmatrix_operator_plus_equals>`
     - add a (matrix, scalar, or row view) to the matrix in place
   * - :ref:`operator\*=<staticmatrix_operator_prod_equals>`
     - multiply the matrix by a matrix or scalar in place
   * - :ref:`operator+<staticmatrix_operator_plus>`
     - add a (matrix, scalar, or row view) to the matrix
   * - :ref:`operator*<staticmatrix_operator_prod>`
     - multiply the matrix by a matrix or scalar

Comparison operators
--------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`operator==<staticmatrix_operator_equals>`
     - compare matrices
   * - :ref:`operator!=<staticmatrix_operator_not_equals>`
     - compare matrices
   * - :ref:`operator\<<staticmatrix_operator_less>`
     - compare matrices
   * - :ref:`operator\><staticmatrix_operator_more>`
     - compare matrices
   
Modifiers
---------

.. list-table:: 
   :widths: 50 50

   * - :ref:`swap<staticmatrix_swap>`
     - swaps the contents
   * - :ref:`transpose<staticmatrix_transpose>`
     - transposes the matrix
