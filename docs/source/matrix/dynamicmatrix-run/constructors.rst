.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_run_constructors:

DynamicMatrix
=============

.. cpp:namespace:: libsemigroups::DynamicMatrix<Semiring, Scalar>

This page contains information about the constructors for the
:cpp:any:`DynamicMatrix` class.

Default constructors
--------------------

.. cpp:function:: DynamicMatrix() = delete
   
   The default constructor for this variant of ``DynamicMatrix`` is deleted
   because a valid pointer to a semiring object is required to define the
   arithmetic at runtime.

.. cpp:function:: DynamicMatrix(DynamicMatrix const&) = default
   
   Default copy constructor.

.. cpp:function:: DynamicMatrix(DynamicMatrix&&) = default
   
   Default move constructor.

.. cpp:function:: DynamicMatrix& \
                  operator=(DynamicMatrix const&) = default

   Default copy assignment operator.

.. cpp:function:: DynamicMatrix& \
                  operator=(DynamicMatrix&&) = default

   Default move assignment operator.

Matrix constructors
-------------------

.. cpp:function:: DynamicMatrix( \ 
                  Semiring const* semiring, size_t r, size_t c)

   Construct a matrix over the semiring ``semiring``.
   
   :param semiring: a pointer to const semiring object
   :param r: the number of rows in the matrix being constructed
   :param c: the number of columns in the matrix being constructed
    
   :exceptions: 
     This function guarantees not to throw a 
     :cpp:any:`LibsemigroupsException`. 
    
   :complexity: Constant
  
.. cpp:function:: DynamicMatrix( \
        Semiring const* semiring,               \ 
        std::initializer_list<std::initializer_list<scalar_type>> m)

   Construct a matrix over the semiring ``semiring``.
    
   :param semiring: a pointer to const semiring object
   :param m: the values to be copied into the matrix.
    
   :exceptions: 
     This function guarantees not to throw a 
     :cpp:any:`LibsemigroupsException`. 
    
   :complexity: 
     :math:`O(mn)` where :math:`m` is the template parameter :code:`R` and
     :math:`n` is the template parameter :code:`C`. 

.. cpp:function:: DynamicMatrix( \
        Semiring const* semiring,               \ 
        std::vector<std::vector<scalar_type>> const& m)
    
   Construct a matrix over the semiring ``semiring``.
    
   :param semiring: a pointer to const semiring object
   :param m: the values to be copied into the matrix.
    
   :exceptions: 
     This function guarantees not to throw a 
     :cpp:any:`LibsemigroupsException`. 
    
   :complexity: 
     :math:`O(mn)` where :math:`m` is the template parameter :code:`R` and
     :math:`n` is the template parameter :code:`C`. 

Row constructors
----------------

.. cpp:function:: DynamicMatrix( \
                  Semiring const* semiring,     \ 
                  std::initializer_list<scalar_type> c)

   Construct a row over the semiring ``semiring``.
   
   :param semiring: a pointer to const semiring object
   :param c: the values to be copied into the row.
  
   :exceptions: 
     This function guarantees not to throw a 
     :cpp:any:`LibsemigroupsException`. 
   
   :complexity: 
     :math:`O(n)` where :math:`n` is the size of the row being
     constructed.
   
.. cpp:function:: DynamicMatrix(RowView const& rv)

   Construct a row from a row view.
   
   :param rv: the row view.
   
   :exceptions: 
     This function guarantees not to throw a 
     :cpp:any:`LibsemigroupsException`. 
   
   :complexity: 
     :math:`O(n)` where :math:`n` is the size of the row being
     constructed.
