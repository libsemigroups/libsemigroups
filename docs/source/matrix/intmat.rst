.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Integer matrices
================

Defined in ``matrix.hpp``.

This page describes the functionality for :math:`m \times n`  matrices of
integers for arbitrary dimensions :math:`m` and :math:`n`. There are two types
of such matrices those whose dimension is known at compile-time, and those
where it is not.  Both types can be accessed via the alias template
:cpp:any:`IntMat\<R, C, Scalar>`: if ``R`` or ``C`` has value ``0``, then the
dimensions can be set at run time, otherwise ``R`` and ``C`` are the dimension.
The default value of ``R`` is ``0``, and of ``C`` is ``R``. 

The alias :cpp:any:`IntMat\<R, C, Scalar>` is either :cpp:any:`StaticMatrix` or
:cpp:any:`DynamicMatrix`, please refer to the documentation of these class
templates for more details. The only substantial difference in the interface
of :cpp:any:`StaticMatrix` and :cpp:any:`DynamicMatrix` is that the former can
be default constructed and the latter should be constructed using the
dimensions. 

**Example**

.. code-block:: cpp

   IntMat<3> m;       // default construct an uninitialized 3 x 3 static matrix
   IntMat<>  m(4, 4); // construct an uninitialized 4 x 4 dynamic matrix


.. cpp:struct:: template <typename Scalar> \
                IntegerPlus

    This is a stateless struct with a single call operator of signature:
    ``Scalar operator()(Scalar const x, Scalar const y) const noexcept``
    which returns the usual sum ``x + y`` of ``x`` and ``y``; representing
    addition in the integer semiring.

.. cpp:struct:: template <typename Scalar> \
                IntegerProd

    This is a stateless struct with a single call operator of signature:
    ``Scalar operator()(Scalar const x, Scalar const y) const noexcept``
    which returns the usual product ``x * y`` of ``x`` and ``y``; representing
    multiplication in the integer semiring.


.. cpp:struct:: template <typename Scalar> \
                IntegerZero

    This is a stateless struct with a single call operator of signature: 
    ``Scalar operator()() const noexcept`` which returns ``0``; representing the
    additive identity of the integer semiring.


.. cpp:struct:: template <typename Scalar> \
                IntegerOne

    This is a stateless struct with a single call operator of signature: 
    ``Scalar operator()() const noexcept`` which returns ``1``; representing the
    multiplicative identity of the integer semiring.


.. cpp:type:: template <typename Scalar>           \
              DynamicIntMat                        \
              = DynamicMatrix<IntegerPlus<Scalar>, \
                              IntegerProd<Scalar>, \
                              IntegerZero<Scalar>, \
                              IntegerOne<Scalar>,  \ 
                              Scalar>

   Alias for the type of dynamic integer matrices where the dimensions of the
   matrices can be defined at run time. 
   
   :tparam Scalar: 
     The type of the entries in the matrix (default: ``int``). 


.. cpp:type::  template <size_t R, size_t C, typename Scalar>   \
               StaticIntMat = StaticMatrix<IntegerPlus<Scalar>, \
                                           IntegerProd<Scalar>, \
                                           IntegerZero<Scalar>, \
                                           IntegerOne<Scalar>,  \
                                           R,                   \
                                           C,                   \
                                           Scalar>

   :tparam R: 
     the number of rows of the matrices. A value of ``0`` (the default value)
     indicates that the dimensions will be set at run time.
   
   :tparam C: 
     the number of columns of the matrices. A value of ``0``
     indicates that the dimensions will be set at run time (the default value
     is ``R``).

   :tparam Scalar: 
     The type of the entries in the matrix (default: ``int``). 


.. cpp:type:: template <size_t R = 0, size_t C = R, Scalar = int> \
              IntMat = std::conditional_t<R == 0 || C == 0,       \
                                          DynamicIntMat<Scalar>,  \
                                          StaticIntMat<R, C, Scalar>>
   
   Alias template for integer matrices.

   :tparam R: 
     the number of rows of the matrices. A value of ``0`` (the default value)
     indicates that the dimensions will be set at run time.
   
   :tparam C: 
     the number of columns of the matrices. A value of ``0``
     indicates that the dimensions will be set at run time (the default value
     is ``R``).

   :tparam Scalar: 
     the type of the entries in the matrix (default: ``int``). 

.. cpp:var:: template <typename T> \
             static constexpr bool IsIntMat

   This variable has value ``true`` if the template parameter ``T`` is the same
   as :cpp:any:`IntMat\<R, C, Scalar>` for some values of ``R``, ``C``,  and
   ``Scalar``.
