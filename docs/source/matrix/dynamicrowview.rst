.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

DynamicRowView
==============

.. highlight:: cpp

Defined in ``matrix.hpp``.

This is a class for views into a row  of a matrix over a semirings. An
instance of this class provides access to the elements in a row of a
matrix and is cheap to create and copy. Addition, scalar multiplication,
and other usual vector operations are defined for row views.

There are two types of :cpp:any:`DynamicMatrix` and two types of
:cpp:any:`DynamicRowView` also:

1. those where the arithmetic is defined at compile time;
2. those where the arithmetic is defined at run time.

From the perspective of using these DynamicRowViews there is no
difference in the interface; the distinction is only important if you
want to extend the functionality of ``libsemigroups`` to further
semirings.

.. warning::

  If the underlying matrix is destroyed, then any row views for that matrix
  are invalidated.

.. cpp:class:: template <typename PlusOp, \
                         typename ProdOp, \
                         typename ZeroOp, \
                         typename OneOp,  \
                         typename Scalar> \
   DynamicRowView final
   :tparam-line-spec:
  
   This class is the type of row views into a :cpp:any:`DynamicMatrix` with the
   same template parameters; see the documentation for :cpp:any:`DynamicMatrix`
   for further details.
  
   .. warning::
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

   :tparam Scalar: 
      the type of the entries in the matrices (the type of elements in the
      underlying semiring)


.. cpp:class:: template <typename Semiring, typename Scalar> \
               DynamicRowView final
  
   This class is the type of row views into a :cpp:any:`DynamicMatrix` with the
   same template parameters; see the documentation for :cpp:any:`DynamicMatrix`
   for further details.
  
   .. warning::
     If the underlying matrix is destroyed, then any row views for that matrix
     are invalidated.

   :tparam Semiring: 
      the type of a semiring object which defines the semiring arithmetic (see
      requirements below).

   :tparam Scalar: 
      the type of the entries in the matrices (the type of elements in the
      underlying semiring)

.. toctree::
   :includehidden:
   :hidden:
   :maxdepth: 1

   dynamicrowview/begin
   dynamicrowview/call_operator
   dynamicrowview/comparison_ops
   dynamicrowview/constructors
   dynamicrowview/end
   dynamicrowview/operator_plus_equals
   dynamicrowview/operator_plus
   dynamicrowview/operator_prod_equals
   dynamicrowview/operator_prod
   dynamicrowview/size
   dynamicrowview/subscript_operator
   dynamicrowview/types

Member types
------------

.. list-table:: 
   :widths: 50 50
   :header-rows: 1

   * - Member type
     - Definition
   * - :ref:`scalar_type<dynamicrowview_scalar_type>`
     - the type of scalars contained in the row (:code:`Scalar`)
   * - :ref:`scalar_reference<dynamicrowview_scalar_reference>`
     - the type of references to scalars contained in the row
   * - :ref:`scalar_const_reference<dynamicrowview_scalar_const_reference>`
     - the type of const references to scalars contained in the row
   * - :ref:`matrix_type<dynamicrowview_mat_type>`
     - type of the underlying matrix of the row view
   * - :ref:`Row<dynamicrowview_row_type>`
     - type of a row for the row view
   * - :ref:`iterator<dynamicrowview_iterator>`
     - type of iterators to entries in the underlying row
   * - :ref:`const_iterator<dynamicrowview_const_iterator>`
     - type of iterators to entries in the underlying row

Member functions
----------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`(constructor)<dynamicrowview_constructors>`
     - constructs the matrix
   * - :ref:`size<dynamicrowview_size>`
     - the length of the row (the template parameter :cpp:any:`C`)

Iterators
---------

.. list-table:: 
   :widths: 50 50

   * - :ref:`begin/cbegin<dynamicrowview_begin>`
     - returns an iterator to the beginning
   * - :ref:`end/cend<dynamicrowview_end>`
     - returns an iterator to the end

Accessors
---------

.. list-table:: 
   :widths: 50 50
   
   * - :ref:`operator()<dynamicrowview_call_operator>`
     - access specified element
   * - :ref:`operator[]<dynamicrowview_subscript_operator>`
     - access specified element

Arithmetic operators
--------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`operator+=<dynamicrowview_operator_plus_equals>`
     - add a (matrix, scalar, or row view) to the matrix in place
   * - :ref:`operator\*=<dynamicrowview_operator_prod_equals>`
     - multiply the matrix by a matrix or scalar in place
   * - :ref:`operator+<dynamicrowview_operator_plus>`
     - add a (matrix, scalar, or row view) to the matrix
   * - :ref:`operator*<dynamicrowview_operator_prod>`
     - multiply the matrix by a matrix or scalar

Comparison operators
--------------------

.. list-table:: 
   :widths: 50 50

   * - :ref:`operator==<dynamicrowview_operator_equals>`
     - compare matrices
   * - :ref:`operator!=<dynamicrowview_operator_not_equals>`
     - compare matrices
   * - :ref:`operator\<<dynamicrowview_operator_less>`
     - compare matrices
   * - :ref:`operator\><dynamicrowview_operator_more>`
     - compare matrices
