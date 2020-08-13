.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Max-plus matrices
=================

Defined in ``matrix.hpp``.

This page describes the functionality for :math:`n \times n`  matrices over the
max-plus semiring for arbitrary dimension :math:`n`. There are two types of
such matrices those whose dimension is known at compile-time, and those where
it is not.  Both types can be accessed via the alias template
:cpp:any:`MaxPlusMat\<N, Scalar>`: if ``N`` has value ``0``, then the dimensions
can be set at run time, otherwise ``N`` is the dimension. The default value of
``N`` is ``0``. 

The alias :cpp:any:`MaxPlusMat\<N, Scalar>` is either :cpp:any:`StaticMatrix` or
:cpp:any:`DynamicMatrix`, please refer to the documentation of these class
templates for more details. The only substantial difference in the interface
of :cpp:any:`StaticMatrix` and :cpp:any:`DynamicMatrix` is that the former can
be default constructed and the latter should be constructed using the
dimensions. 

**Example**

.. code-block:: cpp

   MaxPlusMat<3> m;       // default construct an uninitialized 3 x 3 static matrix
   MaxPlusMat<>  m(4, 4); // construct an uninitialized 4 x 4 dynamic matrix


.. cpp:struct:: template <typename Scalar> \
                MaxPlusPlus

    This is a stateless struct with a single call operator of signature:
    ``Scalar operator()(Scalar const x, Scalar const y) const noexcept``
    that returns :math:`x \oplus y` which is defined by 

    .. math::

       x\oplus y = 
       \begin{cases}
       \max\{x, y\}   & \text{if } x \neq -\infty\text{ and }y \neq -\infty \\
       -\infty & \text{if } x = -\infty \text{ or }y = -\infty; \\
       \end{cases}

    representing addition in the max-plus semiring.

.. cpp:struct:: template <typename Scalar> \
                MaxPlusProd

    This is a stateless struct with a single call operator of signature:
    ``Scalar operator()(Scalar const x, Scalar const y) const noexcept``
    that returns :math:`x \otimes y` which is defined by 

    .. math::

       x\otimes y = 
       \begin{cases}
       x + y   & \text{if } x \neq -\infty\text{ and }y \neq -\infty \\
       -\infty & \text{if } x = -\infty \text{ or }y = -\infty; \\
       \end{cases}

    representing multiplication in the max-plus semiring.


.. cpp:struct:: template <typename Scalar> \
                MaxPlusZero

    This is a stateless struct with a single call operator of signature: 
    ``Scalar operator()() const noexcept`` which returns :math:`-\infty`;
    representing the additive identity of the max-plus semiring.


.. cpp:type:: template <typename Scalar>           \
              DynamicMaxPlusMat                    \
              = DynamicMatrix<MaxPlusPlus<Scalar>, \
                              MaxPlusProd<Scalar>, \
                              MaxPlusZero<Scalar>, \
                              IntegerZero<Scalar>, \ 
                              Scalar>

   Alias for the type of dynamic max-plus matrices where the dimensions of the
   matrices can be defined at run time. 
   
   :tparam Scalar: 
     The type of the entries in the matrix.


.. cpp:type::  template <size_t R, size_t C, typename Scalar>       \
               StaticMaxPlusMat = StaticMatrix<MaxPlusPlus<Scalar>, \
                                               MaxPlusProd<Scalar>, \
                                               MaxPlusZero<Scalar>, \
                                               IntegerZero<Scalar>, \
                                               R,                   \
                                               C,                   \
                                               Scalar>

   Alias for static max-plus matrices whose arithmetic and dimensions are
   defined at compile-time.

   :tparam R: the number of rows.  

   :tparam C: the number of columns.  

   :tparam Scalar: 
     The type of the entries in the matrix. 


.. cpp:type:: template <size_t R = 0,                                     \ 
                        size_t C = R,                                     \
                        Scalar = int>                                     \
              MaxPlusMat = std::conditional_t<R == 0 || C == 0,           \
                                          DynamicMaxPlusMat<Scalar>,      \
                                          StaticMaxPlusMat<R, C, Scalar>>
   
   Alias template for max-plus matrices.

   :tparam R: 
     the number of rows.  A value of ``0`` indicates that the value will be set
     at run time (default: ``0``).

   :tparam C: 
     the number of columns.  A value of ``0`` indicates that the value will be
     set at run time (default: ``R``).

   :tparam Scalar: 
     The type of the entries in the matrix (default: ``int``). 

.. cpp:var:: template <typename T> \
             static constexpr bool IsMaxPlusMat

   This variable has value ``true`` if the template parameter ``T`` is the same
   as :cpp:any:`MaxPlusMat\<R, C, Scalar>` for some value of ``N`` and ``Scalar``.
