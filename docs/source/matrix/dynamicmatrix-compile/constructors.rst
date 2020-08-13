.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _dynamicmatrix_compile_constructors:

DynamicMatrix
=============

.. cpp:namespace:: libsemigroups::DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>

This page contains information about the constructors for the
:cpp:any:`DynamicMatrix` class.

Default constructors
--------------------

.. cpp:function:: DynamicMatrix() = default
   
   Default constructor.

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

.. cpp:function:: DynamicMatrix(size_t r, size_t c)

   Construct a matrix.
    
   :param r: the number of rows in the matrix being constructed
   :param c: the number of columns in the matrix being constructed
    
   :exceptions: 
     This function guarantees not to throw a 
     :cpp:any:`LibsemigroupsException`. 
    
   :complexity: Constant
  
   **Example**

   .. code-block:: cpp

      Mat m(2, 3); // construct a 2 x 3 matrix


.. cpp:function:: DynamicMatrix( \
        std::initializer_list<std::initializer_list<scalar_type>> m)

   Construct a matrix.
    
   :param m: the values to be copied into the matrix.
    
   :exceptions: 
     this function guarantees not to throw a 
     :cpp:any:`LibsemigroupsException`. 
    
   :complexity: 
     :math:`O(mn)` where :math:`m` is the number of rows and 
     :math:`n` is the number of columns in the matrix being constructed.
  
   **Example**

   .. code-block:: cpp

      Mat m({{1, 1}, {0, 0}});


.. cpp:function:: DynamicMatrix( \
        std::vector<std::vector<scalar_type>> const& m)
    
   Construct a matrix.
    
   :param m: the values to be copied into the matrix.
    
   :exceptions: 
     this function guarantees not to throw a 
     :cpp:any:`LibsemigroupsException`. 
    
   :complexity: 
     :math:`O(mn)` where :math:`m` is the number of rows and 
     :math:`n` is the number of columns in the matrix being constructed.

Row constructors
----------------

.. cpp:function:: DynamicMatrix( \
                  std::initializer_list<scalar_type> c)

   Construct a row.
   
   :param c: the values to be copied into the row.
  
   :exceptions: 
     this function guarantees not to throw a 
     :cpp:any:`LibsemigroupsException`. 
   
   :complexity: 
     :math:`O(n)` where :math:`n` is the size of the row being
     constructed.
   
.. cpp:function:: DynamicMatrix(RowView const& rv)

   Construct a row from a row view.
   
   :param rv: the row view.
   
   :exceptions: 
     this function guarantees not to throw a 
     :cpp:any:`LibsemigroupsException`. 
   
   :complexity: 
     :math:`O(n)` where :math:`n` is the size of the row being
     constructed.
