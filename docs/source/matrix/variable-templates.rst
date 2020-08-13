.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Variable templates
==================

Defined in ``matrix.hpp``.

This page contains documentation for several variable templates related to
matrices. 

.. cpp:var:: template <typename T> \
             static constexpr bool IsMatrix

   This variable has value ``true`` if the template parameter ``T`` is either
   :cpp:any:`DynamicMatrix` or :cpp:any:`StaticMatrix`; and ``false``
   otherwise. 

.. cpp:var:: template <typename T> \
             static constexpr bool IsStaticMatrix

   This variable has value ``true`` if the template parameter ``T`` is
   :cpp:any:`StaticMatrix`; and ``false`` otherwise. 

.. cpp:var:: template <typename T> \
             static constexpr bool IsDynamicMatrix

   This variable has value ``true`` if the template parameter ``T`` is
   :cpp:any:`DynamicMatrix`; and ``false`` otherwise. 


.. cpp:var:: template <typename T> \
             static constexpr bool IsMatWithSemiring
   
   This variable has value ``true`` if the template parameter ``T`` is
   :cpp:any:`DynamicMatrix\<Semiring, Scalar>`; and ``false`` otherwise. 
