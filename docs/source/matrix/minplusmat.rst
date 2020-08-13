.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Min-plus matrices
=================

Defined in ``matrix.hpp``.

This page describes the functionality for :math:`n \times n`  matrices over the
min-plus semiring for arbitrary dimension :math:`n`. There are two types of
such matrices those whose dimension is known at compile-time, and those where
it is not.  Both types can be accessed via the alias template
:cpp:any:`MinPlusMat\<N, Scalar>`: if ``N`` has value ``0``, then the dimensions
can be set at run time, otherwise ``N`` is the dimension. The default value of
``N`` is ``0``. 

The alias :cpp:any:`MinPlusMat\<N, Scalar>` is either :cpp:any:`StaticMatrix` or
:cpp:any:`DynamicMatrix`, please refer to the documentation of these class
templates for more details. The only substantial difference in the interface
of :cpp:any:`StaticMatrix` and :cpp:any:`DynamicMatrix` is that the former can
be default constructed and the latter should be constructed using the
dimensions. 

**Example**

.. code-block:: cpp

   MinPlusMat<3> m;       // default construct an uninitialized 3 x 3 static matrix
   MinPlusMat<>  m(4, 4); // construct an uninitialized 4 x 4 dynamic matrix


.. cpp:struct:: template <typename Scalar> \
                MinPlusPlus

    This is a stateless struct with a single call operator of signature:
    ``Scalar operator()(Scalar const x, Scalar const y) const noexcept``
    that returns :math:`x \oplus y` which is defined by 

    .. math::

       x\oplus y = 
       \begin{cases}
       \min\{x, y\}   & \text{if } x \neq \infty\text{ and }y \neq \infty \\
       \infty & \text{if } x = \infty \text{ or }y = \infty; \\
       \end{cases}

    representing addition in the min-plus semiring.

.. cpp:struct:: template <typename Scalar> \
                MinPlusProd

    This is a stateless struct with a single call operator of signature:
    ``Scalar operator()(Scalar const x, Scalar const y) const noexcept``
    that returns :math:`x \otimes y` which is defined by 

    .. math::

       x\otimes y = 
       \begin{cases}
       x + y  & \text{if } x \neq \infty\text{ and }y \neq \infty \\
       \infty & \text{if } x = \infty \text{ or }y = \infty; \\
       \end{cases}

    representing multiplication in the min-plus semiring.


.. cpp:struct:: template <typename Scalar> \
                MinPlusZero

    This is a stateless struct with a single call operator of signature: 
    ``Scalar operator()() const noexcept`` which returns :math:`\infty`;
    representing the additive identity of the min-plus semiring.


.. cpp:type:: template <typename Scalar>           \
              DynamicMinPlusMat                    \
              = DynamicMatrix<MinPlusPlus<Scalar>, \
                              MinPlusProd<Scalar>, \
                              MinPlusZero<Scalar>, \
                              IntegerZero<Scalar>, \ 
                              Scalar>

   Alias for the type of dynamic min-plus matrices where the dimensions of the
   matrices can be defined at run time. 
   
   :tparam Scalar: 
     The type of the entries in the matrix. 


.. cpp:type::  template <size_t R, size_t C, typename Scalar>       \
               StaticMinPlusMat = StaticMatrix<MinPlusPlus<Scalar>, \
                                               MinPlusProd<Scalar>, \
                                               MinPlusZero<Scalar>, \
                                               IntegerZero<Scalar>, \
                                               R,                   \
                                               C,                   \
                                               Scalar>

   Alias for static min-plus matrices whose arithmetic and dimensions are
   defined at compile-time.

   :tparam R: the number of rows.  

   :tparam C: the number of columns.  

   :tparam Scalar: 
     The type of the entries in the matrix. 

.. cpp:type:: template <size_t R = 0,                                     \ 
                        size_t C = R,                                     \
                        Scalar = int>                                     \
              MinPlusMat = std::conditional_t<R == 0 || C == 0,           \
                                          DynamicMinPlusMat<Scalar>,      \
                                          StaticMinPlusMat<R, C, Scalar>>
   
   Alias template for min-plus matrices.

   :tparam R: 
     the number of rows.  A value of ``0`` indicates that the value will be set
     at run time (default: ``0``).

   :tparam C: 
     the number of columns.  A value of ``0`` indicates that the value will be
     set at run time (default: ``R``).

   :tparam Scalar: 
     The type of the entries in the matrix (default: ``int``). 


.. cpp:var:: template <typename T> \
             static constexpr bool IsMinPlusMat

   This variable has value ``true`` if the template parameter ``T`` is the same
   as :cpp:any:`MinPlusMat\<N, Scalar>` for some value of ``N`` and ``Scalar``.
