.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

StaticRowView
=============

.. highlight:: cpp

Defined in ``matrix.hpp``.

.. cpp:class:: template <typename PlusOp, \
                         typename ProdOp, \
                         typename ZeroOp, \
                         typename OneOp,  \
                         size_t C,        \
                         typename Scalar> \
   StaticRowView final
   :tparam-line-spec:
  
   This is a class for views into a row of a matrix over a semiring. An
   instance of this class provides access to the elements in a row of a matrix
   and is cheap to create and copy. Addition, scalar multiplication, and other
   standard vector operations are defined for row views.
  
   This class is the type of row views into a :cpp:any:`StaticMatrix`; see the
   documentation for :cpp:any:`StaticMatrix` for further details.
  
   \warning
   If the underlying matrix is destroyed, then any row views for that matrix
   are invalidated.

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

   :tparam C: the number of columns of the underlying matrix 

   :tparam Scalar: 
      the type of the entries in the matrices (the type of elements in the
      underlying semiring)

.. toctree::
   :hidden:

   staticrowview/begin
   staticrowview/call_operator
   staticrowview/comparison_ops
   staticrowview/constructors
   staticrowview/end
   staticrowview/operator_plus_equals
   staticrowview/operator_plus
   staticrowview/operator_prod_equals
   staticrowview/operator_prod
   staticrowview/size
   staticrowview/subscript_operator
   staticrowview/types

Member types
------------

.. list-table:: 
   :widths: 50 50
   :header-rows: 1

   * - Member type
     - Definition
   * - :ref:`scalar_type<staticrowview_scalar_type>`
     - the type of scalars contained in the row (:code:`Scalar`)
   * - :ref:`scalar_reference<staticrowview_scalar_reference>`
     - the type of references to scalars contained in the row
   * - :ref:`scalar_const_reference<staticrowview_scalar_const_reference>`
     - the type of const references to scalars contained in the row
   * - :ref:`matrix_type<staticrowview_mat_type>`
     - type of the underlying matrix of the row view
   * - :ref:`Row<staticrowview_row_type>`
     - type of a row for the row view
   * - :ref:`iterator<staticrowview_iterator>`
     - type of iterators to entries in the underlying row
   * - :ref:`const_iterator<staticrowview_const_iterator>`
     - type of iterators to entries in the underlying row

Member functions
----------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`(constructor)<staticrowview_constructors>`
     - constructs the matrix
   * - :ref:`size<staticrowview_size>`
     - the length of the row (the template parameter :cpp:any:`C`)

Iterators
---------

.. list-table:: 
   :widths: 50 50

   * - :ref:`begin/cbegin<staticrowview_begin>`
     - returns an iterator to the beginning
   * - :ref:`end/cend<staticrowview_end>`
     - returns an iterator to the end

Accessors
---------

.. list-table:: 
   :widths: 50 50
   
   * - :ref:`operator()<staticrowview_call_operator>`
     - access specified element
   * - :ref:`operator[]<staticrowview_subscript_operator>`
     - access specified element

Arithmetic operators
--------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`operator+=<staticrowview_operator_plus_equals>`
     - add a (matrix, scalar, or row view) to the matrix in place
   * - :ref:`operator\*=<staticrowview_operator_prod_equals>`
     - multiply the matrix by a matrix or scalar in place
   * - :ref:`operator+<staticrowview_operator_plus>`
     - add a (matrix, scalar, or row view) to the matrix
   * - :ref:`operator*<staticrowview_operator_prod>`
     - multiply the matrix by a matrix or scalar

Comparison operators
--------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`operator==<staticrowview_operator_equals>`
     - compare matrices
   * - :ref:`operator!=<staticrowview_operator_not_equals>`
     - compare matrices
   * - :ref:`operator\<<staticrowview_operator_less>`
     - compare matrices
   * - :ref:`operator\><staticrowview_operator_more>`
     - compare matrices
