.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Projective max-plus matrices
============================

Defined in ``matrix.hpp``.

This page describes the functionality for :math:`n \times n` projective
matrices over the max-plus semiring.
Projective max-plus matrices belong to the quotient of the monoid of all
max-plus matrices by the congruence where two matrices are related if they
differ by a scalar multiple; see :cpp:any:`MaxPlusMat`.


There are two types of such matrices those whose dimension is known at
compile-time, and those where it is not.  Both types can be accessed via the
alias template :cpp:any:`ProjMaxPlusMat\<R, C, Scalar>`: if ``R`` or ``C`` has
value ``0``, then the dimensions can be set at run time, otherwise ``R`` and
``C`` are the dimensions.  The default value of ``R`` is ``0``, ``C`` is ``R``,
and ``Scalar`` is ``int``.

Matrices in both these classes are modified when constructed to be in a normal
form which is obtained by subtracting the maximum finite entry in the matrix
from the every finite entry.

The alias :cpp:any:`ProjMaxPlusMat\<T, R, C, Scalar>` is neither
:cpp:any:`StaticMatrix` nor :cpp:any:`DynamicMatrix`, but has the same
interface as each of these types.  An instance of :cpp:any:`ProjMaxPlusMat\<T,
R, C, Scalar>` wraps a :cpp:any:`MaxPlusMat`.  

.. note::
   The types :cpp:any:`RowView` and :cpp:any:`Row` are the same as those in the
   wrapped matrix. This means that a :cpp:any:`Row` object for a projective
   max-plus matrix is never normalised, because if they were  they would be
   normalised according to their entries, and this might not correspond to the
   normalised entries of the matrix.

Please refer to the documentation of these class templates for more details.
The only substantial difference in the interface of :cpp:any:`StaticMatrix` and
:cpp:any:`DynamicMatrix` is that the former can be default constructed and the
latter should be constructed using the dimensions. 

**Example**

.. code-block:: cpp

   ProjMaxPlusMat<3> m;       // default construct an uninitialized 3 x 3 static matrix
   ProjMaxPlusMat<>  m(4, 4); // construct an uninitialized 4 x 4 dynamic matrix
  

.. cpp:type:: template <size_t R, size_t C, typename Scalar> \
              StaticProjMaxPlusMat                           \
              = detail::ProjMaxPlusMat<StaticMaxPlusMat<R, C, Scalar>>
   
   Alias for static projective max-plus matrices whose arithmetic and
   dimensions are defined at compile-time.
   
   :tparam R: the number of rows.  

   :tparam C: the number of columns.  

   :tparam Scalar: The type of the entries in the matrix.

.. cpp:type:: template <typename Scalar> \
              DynamicProjMaxPlusMat      \
              = detail::ProjMaxPlusMat<DynamicMaxPlusMat<Scalar>>
   
   Alias for the type of dynamic projective max-plus matrices where the
   dimensions of the matrices can be defined at run time. 

   :tparam Scalar: The type of the entries in the matrix.

.. cpp:type:: template <size_t R = 0, size_t C = R, typename Scalar = int> \
              ProjMaxPlusMat =                                             \
      std::conditional_t<R == 0 || C == 0,                                 \
                                DynamicProjMaxPlusMat<Scalar>,             \
                                StaticProjMaxPlusMat<R, C, Scalar>>

   Alias template for projective max-plus matrices.

   :tparam R: 
     the number of rows.  A value of ``0`` indicates that the value will be set
     at run time (default: ``0``).

   :tparam C: 
     the number of columns.  A value of ``0`` indicates that the value will be
     set at run time (default: ``R``).

   :tparam Scalar: 
     The type of the entries in the matrix (default: ``int``). 
  
.. cpp:var:: template <typename T> \
             static constexpr bool IsProjMaxPlusMat

   This variable has value ``true`` if the template parameter ``T`` is the same
   as :cpp:any:`ProjMaxPlusMat\<R, C, Scalar>` for some values of ``R``, ``C``,
   and ``Scalar``; and ``false`` if it is not. 
