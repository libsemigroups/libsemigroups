.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_run_semiring:
   
semiring
========

.. cpp:namespace:: libsemigroups::DynamicMatrix<Semiring, Scalar>

.. cpp:function::  Semiring const* semiring() const noexcept
   
   Returns a const pointer to the underlying semiring (if any).

   :parameters: (None)
   
   :returns: A value of type ``Semiring const*``.
   
   :exceptions: 
     This function is ``noexcept`` and guarantees not to throw.
   
   :complexity: Constant
