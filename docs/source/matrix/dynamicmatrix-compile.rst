.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

DynamicMatrix (with compile time arithmetic)
============================================

.. highlight:: cpp

Defined in ``matrix.hpp``.

.. cpp:class:: template <typename PlusOp, \
                         typename ProdOp, \
                         typename ZeroOp, \
                         typename OneOp,  \
                         typename Scalar> \
   DynamicMatrix final
   :tparam-line-spec:

   This is a class for matrices where the arithmetic operations in the
   underlying semiring are known at compile time, and the dimensions of the
   matrix can be set at run time.

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

   :tparam Scalar: 
      the type of the entries in the matrices (the type of elements in the
      underlying semiring)

   .. note::
      Certain member functions only work for square matrices and some only work
      for rows. 

.. toctree::
   :hidden:

   dynamicmatrix-compile/begin
   dynamicmatrix-compile/call_operator
   dynamicmatrix-compile/comparison_ops
   dynamicmatrix-compile/constructors
   dynamicmatrix-compile/coords
   dynamicmatrix-compile/end
   dynamicmatrix-compile/number_of_rows
   dynamicmatrix-compile/operator_plus
   dynamicmatrix-compile/operator_plus_equals
   dynamicmatrix-compile/operator_prod
   dynamicmatrix-compile/operator_prod_equals
   dynamicmatrix-compile/row
   dynamicmatrix-compile/rows
   dynamicmatrix-compile/staticmemfn
   dynamicmatrix-compile/swap
   dynamicmatrix-compile/transpose
   dynamicmatrix-compile/types

Member types
------------

.. list-table:: 
   :widths: 50 50
   :header-rows: 1

   * - Member type
     - Definition
   * - :ref:`scalar_type<dynamicmatrix_compile_scalar_type>`
     - the type of scalars contained in the matrix (:code:`Scalar`)
   * - :ref:`scalar_reference<dynamicmatrix_compile_scalar_reference>`
     - the type of references to scalars contained in the matrix
   * - :ref:`scalar_const_reference<dynamicmatrix_compile_scalar_const_reference>`
     - the type of const references to scalars contained in the matrix
   * - :ref:`Row<dynamicmatrix_compile_row_type>`
     - type of a row of the matrix 
   * - :ref:`RowView<dynamicmatrix_compile_rowview_type>`
     - type of a row view int the matrix 
   * - :ref:`Plus<dynamicmatrix_compile_plus>`
     - the template parameter :cpp:any:`PlusOp`
   * - :ref:`Prod<dynamicmatrix_compile_prod>`
     - the template parameter :cpp:any:`ProdOp`
   * - :ref:`Zero<dynamicmatrix_compile_zero>`
     - the template parameter :cpp:any:`ZeroOp`
   * - :ref:`One<dynamicmatrix_compile_one>`
     - the template parameter :cpp:any:`OneOp`
   * - :ref:`semiring_type<dynamicmatrix_compile_semiring_type>`
     - ``void``


Member functions
----------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`(constructor)<dynamicmatrix_compile_constructors>`
     - constructs the matrix
   * - :ref:`number_of_rows/number_of_cols<dynamicmatrix_compile_number_of_rows>`
     - the number of rows/columns in the matrix

Static member functions
-----------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`make<dynamicmatrix_compile_make>`
     - constructs a matrix and checks that its entries are valid
   * - :ref:`identity<dynamicmatrix_compile_identity>`
     - constructs an identity matrix

Iterators
---------

.. list-table:: 
   :widths: 50 50

   * - :ref:`begin/cbegin<dynamicmatrix_compile_begin>`
     - returns an iterator to the beginning
   * - :ref:`end/cend<dynamicmatrix_compile_end>`
     - returns an iterator to the end
   * - :ref:`coords<dynamicmatrix_compile_coords>`
     - returns the coordinates corresponding to an iterator

Accessors
---------

.. list-table:: 
   :widths: 50 50
   
   * - :ref:`operator()<dynamicmatrix_compile_call_operator>`
     - access specified element
   * - :ref:`row<dynamicmatrix_compile_row>`
     - access specified row
   * - :ref:`rows<dynamicmatrix_compile_rows>`
     - add all rows to a container

Arithmetic operators
--------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`operator+=<dynamicmatrix_compile_operator_plus_equals>`
     - add a (matrix, scalar, or row view) to the matrix in place
   * - :ref:`operator\*=<dynamicmatrix_compile_operator_prod_equals>`
     - multiply the matrix by a matrix or scalar in place
   * - :ref:`operator+<dynamicmatrix_compile_operator_plus>`
     - add a (matrix, scalar, or row view) to the matrix
   * - :ref:`operator*<dynamicmatrix_compile_operator_prod>`
     - multiply the matrix by a matrix or scalar

Comparison operators
--------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`operator==<dynamicmatrix_compile_operator_equals>`
     - compare matrices
   * - :ref:`operator!=<dynamicmatrix_compile_operator_not_equals>`
     - compare matrices
   * - :ref:`operator\<<dynamicmatrix_compile_operator_less>`
     - compare matrices
   * - :ref:`operator\><dynamicmatrix_compile_operator_more>`
     - compare matrices
   
Modifiers
---------

.. list-table:: 
   :widths: 50 50

   * - :ref:`swap<dynamicmatrix_compile_swap>`
     - swaps the contents
   * - :ref:`transpose<dynamicmatrix_compile_transpose>`
     - transposes the matrix
