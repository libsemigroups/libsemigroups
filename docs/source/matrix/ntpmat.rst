.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Matrices over the natural numbers quotiented by (t = t + p) 
===========================================================

Defined in ``matrix.hpp``.

This page describes the functionality for :math:`n \times n` matrices over the
finite quotient of the usual semiring of natural number by the congruence
:math:`t = t + p` for arbitrary :math:`n`, :math:`t`, and :math:`p`. The value
:math:`t` is referred to as the *threshold* and :math:`p` is called the
*period*. The matrices of this type are referred to by the acroynm **ntp**
matrices, for "natural threshold period".  The :cpp:any:`NTPSemiring` has
elements :math:`\{0, 1, ..., t, t +  1, ..., t + p - 1\}` where  :math:`t`, and
:math:`p` are the threshold and period, respectively; addition and
multiplication in the :cpp:any:`NTPSemiring` is defined below.

There are three types of such matrices where:

1. the dimension is known at compile-time;
2. the dimension is to be defined a run time but the arithmetic operations
   are known at compile-time (i.e. the values of :math:`t` and :math:`p` are 
   known at compile time)
3. both the dimension and the arithmetic operations (i.e. :math:`t` and
   :math:`p`) are to be defined a run time.

All three of these types can be accessed via the alias template
:cpp:any:`NTPMat\<T, P, R, C, Scalar>`: if ``T`` and ``P`` have value ``0``,
then the threshold and period can be set at run time, and if ``R`` or ``C`` is
``0``, then the dimension can be set at run time.  The default values of ``T``,
``P``, and ``R`` are ``0``, and the default value of ``C`` is ``R``. 

The alias :cpp:any:`NTPMat\<T, P, R, C, Scalar>` is either
:cpp:any:`StaticMatrix` or :cpp:any:`DynamicMatrix`, please refer to the
documentation of these class templates for more details. The only substantial
difference in the interface of :cpp:any:`StaticMatrix` and
:cpp:any:`DynamicMatrix` is that the former can be default constructed and the
latter should be constructed using the dimensions. 

**Example**

.. code-block:: cpp

   NTPMat<11, 2, 3> m;  // default construct an uninitialized 3 x 3 static matrix with threshold 11, period 2
   NTPMat<11, 2> m(4, 4);  // construct an uninitialized 4 x 4 dynamic matrix with threshold 11, period 2
   NTPSemiring sr(11, 2);  // construct an ntp semiring with threshold 11, period 2
   NTPMat<>  m(sr, 5, 5);  // construct an uninitialized 5 x 5 dynamic matrix with threshold 11, period 2 


.. cpp:struct:: template <size_t T, size_t P, typename Scalar> \
                NTPPlus

    This is a stateless struct with a single call operator of signature:
    ``Scalar operator()(Scalar const x, Scalar const y) const noexcept``
    that returns :math:`x \oplus y` which is defined by 

    .. math::

       x\oplus y = 
       \begin{cases}
       x + y & \text{if } x + y \leq T \\
       T + ((x + y) - T \pmod{P}) & \text{if } x + y > T  
       \end{cases}

    representing addition in the quotient of the semiring natural numbers by
    the congruence :math:`(T = T + P)`.

.. cpp:struct:: template <size_t T, size_t P, typename Scalar> \
                NTPProd

    This is a stateless struct with a single call operator of signature:
    ``Scalar operator()(Scalar const x, Scalar const y) const noexcept``
    that returns :math:`x \otimes y` which is defined by 

    .. math::

       x\otimes y = 
       \begin{cases}
       xy & \text{if } xy \leq T \\
       T + ((xy - T) \pmod{P}) & \text{if } xy > T  
       \end{cases}

    representing multiplication in the quotient of the semiring natural numbers
    by the congruence :math:`(T = T + P)`.

.. cpp:class:: template <typename Scalar = int> \
               NTPSemiring final 
 
   This class represents the **ntp semiring** consists of the integers
   :math:`\{0, 1, ..., t, t +  1, ..., t + p - 1\}` for some  :math:`t` and
   :math:`p` (called the **threshold** and **period**). Instances of this class
   can be used to define the value of the threshold :math:`t` and period
   :math:`p` at run time. 

   :tparam Scalar: the type of the elements of the semiring.

   .. cpp:function:: NTPSemiring() = delete

      Deleted default constructor. 
   
   .. cpp:function:: NTPSemiring(NTPSemiring const&) = default

      Default copy constructor.
   .. cpp:function:: NTPSemiring(NTPSemiring&&) = default

      Default move constructor.

   .. cpp:function:: NTPSemiring& operator=(NTPSemiring const&) \
                 = default

      Default copy assignment operator.

   .. cpp:function:: NTPSemiring& operator=(NTPSemiring&&) \
                 = default
      
      Default move assignment operator.
    
   .. cpp:function:: explicit NTPSemiring(Scalar const t, Scalar const p)
      
      Construct from threshold and period.

      :param t: the threshold (:math:`t \geq 0`). 
      :param p: the period (:math:`p > 0`). 
     
      :throws:
        :cpp:any:`LibsemigroupsException` if ``t`` is less than zero.
      
      :throws:
        :cpp:any:`LibsemigroupsException` if ``p`` is less than or equal to
        zero.
      
      :complexity: Constant.
    
   .. cpp:function:: Scalar zero() const noexcept 
      
      Returns :math:`0`; representing the additive identity of the
      quotient of the semiring of natural numbers.
      
      :parameters: (None)

      :returns: A value of type ``Scalar``.

      :exceptions: 
        This function is ``noexcept`` and is guaranteed never to throw.
      
      :complexity: Constant.
   
   .. cpp:function:: Scalar one() const noexcept 
       
      Returns :math:`1`; representing the additive identity of the
      quotient of the semiring of natural numbers.

      :parameters: (None)

      :returns: A value of type ``Scalar``.

      :exceptions: 
        This function is ``noexcept`` and is guaranteed never to throw.
      
      :complexity: Constant.

   .. cpp:function:: Scalar plus(Scalar const x, Scalar const y) const noexcept
       
      Returns :math:`x \oplus y` which is defined by 

      .. math::

         x\oplus y = 
         \begin{cases}
         x + y & \text{if } x + y \leq \texttt{threshold()} \\
         \texttt{threshold()} + ((x + y - \texttt{threshold()}) \pmod{\texttt{period()}}) & \text{if } x + y > \texttt{threshold()}
         \end{cases}

      representing the addition in the quotient of the semiring of natural
      numbers.
       
     :param x: scalar (:math:`0\leq x < \texttt{threshold()} + \texttt{period()}`)
     :param y: scalar (:math:`0\leq y < \texttt{threshold()} + \texttt{period()}`)

     :returns: A value of type ``Scalar``.

     :exceptions: 
       This function is ``noexcept`` and is guaranteed never to throw.
     
     :complexity: Constant.

   .. cpp:function:: Scalar prod(Scalar const x, Scalar const y) const noexcept
       
      Returns :math:`x \otimes y` which is defined by 

     .. math::

        x\otimes y = 
        \begin{cases}
        xy & \text{if } xy \leq \texttt{threshold()} \\
        \texttt{threshold()} + ((xy - \texttt{threshold()})\pmod{\texttt{period()}}) & \text{if } xy >
        \texttt{threshold()}  
        \end{cases}

     where :math:`t` is the threshold; representing multiplication in the
     quotient of the semiring of natural numbers.
     
     :param x: scalar (:math:`0\leq x < \texttt{threshold()} + \texttt{period()}`)
     :param y: scalar (:math:`0\leq y < \texttt{threshold()} + \texttt{period}()`)
     
     :returns: A value of type ``Scalar``.

     :exceptions: 
       This function is ``noexcept`` and is guaranteed never to throw.
     
     :complexity: Constant.
     
   .. cpp:function:: Scalar threshold() const noexcept
      
      Returns the threshold value used to construct ``this``. 

      :parameters: (None)

      :returns: A value of type ``Scalar``.

      :exceptions: 
        This function is ``noexcept`` and is guaranteed never to throw.
      
      :complexity: Constant.
   

   .. cpp:function:: Scalar period() const noexcept
      
      Returns the period value used to construct ``this``. 

      :parameters: (None)

      :returns: A value of type ``Scalar``.

      :exceptions: 
        This function is ``noexcept`` and is guaranteed never to throw.
      
      :complexity: Constant.


.. cpp:type:: template <typename Scalar> \
              DynamicNTPMatWithSemiring  \
              = DynamicMatrix<NTPSemiring<Scalar>, Scalar>
    
   Alias for ntp matrices with dimensions, threshold, and period defined
   at runtime. 

   :tparam Scalar: 
     The type of the entries in the matrix. 


.. cpp:type:: template <size_t T, size_t P, typename Scalar> \
              DynamicNTPMatWithoutSemiring                   \
              = DynamicMatrix<NTPPlus<T, P, Scalar>,         \
                              NTPProd<T, P, Scalar>,         \
                              IntegerZero<Scalar>,           \
                              IntegerOne<Scalar>,            \
                              Scalar>                        

   Alias for the type of dynamic ntp matrices where the dimension
   is defined at run time, but the threshold and period are defined at
   compile-time.
   
   :tparam T: the threshold. 
   
   :tparam P: the period. 
   
   :tparam Scalar: the type of the entries in the matrix. 


.. cpp:type::  template <size_t T,                         \
                         size_t P,                         \ 
                         size_t R,                         \ 
                         size_t C,                         \
                         typename Scalar>                  \
        StaticNTPMat = StaticMatrix<NTPPlus<T, P, Scalar>, \
                                    NTPProd<T, P, Scalar>, \
                                    IntegerZero<Scalar>,   \
                                    IntegerOne<Scalar>,    \
                                    R,                     \
                                    C,                     \
                                    Scalar>

   Alias for static ntp matrices where the threshold, period, and
   dimensions are defined at compile-time. 

   :tparam T: the threshold. 

   :tparam P: the period. 

   :tparam R: the number of rows.  

   :tparam C: the number of columns.  

   :tparam Scalar: 
     The type of the entries in the matrix (default: ``int``). 


.. cpp:type:: template <size_t T        = 0,                                      \
                        size_t P        = 0,                                      \
                        size_t R        = 0,                                      \
                        size_t C        = R,                                      \
                        typename Scalar = size_t>                                 \
                    NTPMat = std::conditional_t<                                  \ 
                  R == 0 || C == 0,                                               \
                  std::conditional_t<T == 0 && P == 0,                            \
                                     DynamicNTPMatWithSemiring<Scalar>,           \
                                     DynamicNTPMatWithoutSemiring<T, P, Scalar>>, \
                  StaticNTPMat<T, P, R, C, Scalar>>
   
   :tparam T: 
     the threshold. If both ``T`` and ``P`` are ``0``, this indicates that the
     value will be set at run time (default: ``0``).
   
   :tparam P: 
     the period. If both ``T`` and ``P`` are ``0``, this indicates that the
     value will be set at run time (default: ``0``).

   :tparam R: 
     the number of rows.  A value of ``0`` indicates that the value will be set
     at run time (default: ``0``).

   :tparam C: 
     the number of columns.  A value of ``0`` indicates that the value will be
     set at run time (default: ``R``).

   :tparam Scalar: 
     The type of the entries in the matrix (default: ``size_t``). 


.. cpp:var:: template <typename U> \
             static constexpr bool IsNTPMat

   This variable has value ``true`` if the template parameter ``U`` is the same
   as :cpp:any:`NTPMat\<T, P, R, C, Scalar>` for some values of ``T``, ``P``,
   ``R``, ``C``, and ``Scalar``; and ``false`` if it is not. 
