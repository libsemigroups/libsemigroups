.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

BMat\<N>
========

This page describes the functionality for :math:`m \times n` boolean matrices
for arbitrary dimensions :math:`m` and :math:`n`. There are two types of such
boolean matrices those whose dimension is known at compile-time, and those
where it is not.  Both types can be accessed via the alias template
:cpp:any:`BMat\<R, C>`: if ``R`` or ``C`` has value ``0``, then the dimensions
can be set at run time, otherwise the dimensions are ``R`` and ``C``. The default
value of ``R`` is ``0`` and of ``C`` is ``R``. 

The alias :cpp:any:`BMat\<R, C>` is either :cpp:any:`StaticMatrix` or
:cpp:any:`DynamicMatrix`, please refer to the documentation of these class
templates for more details. The only substantial difference in the interface
of :cpp:any:`StaticMatrix` and :cpp:any:`DynamicMatrix` is that the former can
be default constructed and the latter should be constructed using the
dimensions. 

**Example**

.. code-block:: cpp

   BMat<3> m;       // default construct an uninitialized 3 x 3 static matrix
   BMat<>  m(4, 4); // construct an uninitialized 4 x 4 dynamic matrix


.. cpp:struct:: BooleanPlus

    This is a stateless struct with a single call operator of signature:
    ``bool operator()(bool const x, bool const y) const noexcept``
    which returns ``x || y``; representing addition in the boolean semiring.

.. cpp:struct:: BooleanProd

    This is a stateless struct with a single call operator of signature:
    ``bool operator()(bool const x, bool const y) const noexcept``
    which returns ``x && y``; representing multiplication in the boolean
    semiring.

.. cpp:struct:: BooleanZero

    This is a stateless struct with a single call operator of signature: ``bool
    operator()() const noexcept`` which returns ``false``; representing the
    additive identity of the boolean semiring.


.. cpp:struct:: BooleanOne

    This is a stateless struct with a single call operator of signature: ``bool
    operator()() const noexcept`` which returns ``true``; representing the
    multiplicative identity of the boolean semiring.


.. cpp:type:: DynamicBMat \
      = DynamicMatrix<BooleanPlus, BooleanProd, BooleanZero, BooleanOne, int>

   Alias for the type of dynamic boolean matrices where the dimensions of the
   matrices can be defined at run time. 


.. cpp:type::  template <size_t R, size_t C>          \
               StaticBMat = StaticMatrix<BooleanPlus, \
                                         BooleanProd, \
                                         BooleanZero, \
                                         BooleanOne,  \
                                         N,           \
                                         N,           \
                                         int>

   Alias for the type of static boolean matrices where the dimensions of the
   matrices are defined at compile time. 


.. cpp:type:: template <size_t R = 0, size_t C = R> \
      BMat = std::conditional_t<R == 0 || C == 0, DynamicBMat, StaticBMat<R, C>>
   
   Alias template for boolean matrices.

   :tparam R: 
     the number of rows of the matrices. A value of ``0`` (the default value)
     indicates that the dimensions will be set at run time.
   
   :tparam C: 
     the number of columns of the matrices. A value of ``0``
     indicates that the dimensions will be set at run time (the default value
     is ``R``).


.. cpp:var:: template <typename T> \
             static constexpr bool IsBMat

   This variable has value ``true`` if the template parameter ``T`` is the same
   as :cpp:any:`BMat\<R, C>` for some values of ``R`` and ``C``.
