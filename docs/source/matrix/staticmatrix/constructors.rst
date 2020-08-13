.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

.. _staticmatrix_constructors:

StaticMatrix
============

.. cpp:namespace:: libsemigroups::StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>

This page contains information about the constructors for the
:cpp:any:`StaticMatrix` class.

Default constructors
--------------------

.. cpp:function:: StaticMatrix() = default
   
   Default constructor.

.. cpp:function:: StaticMatrix(StaticMatrix const&) = default
   
   Default copy constructor.

.. cpp:function:: StaticMatrix(StaticMatrix&&) = default
   
   Default move constructor.

.. cpp:function:: StaticMatrix& operator=(StaticMatrix const&) = default

   Default copy assignment operator.

.. cpp:function:: StaticMatrix& operator=(StaticMatrix&&) = default

   Default move assignment operator.


Matrix constructors
-------------------

.. cpp:function:: StaticMatrix( \
        std::initializer_list<std::initializer_list<scalar_type>> m)

   Construct a matrix.
    
   :param m: the values to be copied into the matrix.
    
   :exceptions: 
     this function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 
    
   :complexity: 
     :math:`O(mn)` where :math:`m` is the template parameter :code:`R` and
     :math:`n` is the template parameter :code:`C`. 
  
   **Example**

   .. code-block:: cpp

      Mat m({{1, 1}, {0, 0}});


.. cpp:function:: StaticMatrix( \
        std::vector<std::vector<scalar_type>> const& m)
    
   Construct a matrix.
    
   :param m: the values to be copied into the matrix.
    
   :exceptions: 
     this function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 
    
   :complexity: 
     :math:`O(mn)` where :math:`m` is the template parameter :code:`R` and
     :math:`n` is the template parameter :code:`C`. 
    

Row constructors
----------------

.. cpp:function:: StaticMatrix(std::initializer_list<scalar_type> c)

   Construct a row.
   
   :param c: the values to be copied into the row.
  
   :exceptions: 
     this function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 
   
   :complexity: 
     :math:`O(n)` where :math:`n` is the size of the row being
     constructed.
   
   .. warning:: 

     This constructor only works for rows, i.e. when the template parameter
     :code:`R` is :code:`1`.
   
.. cpp:function:: StaticMatrix(RowView const& rv)

   Construct a row from a row view.
   
   :param rv: the row view.
   
   :exceptions: 
     this function guarantees not to throw a :cpp:any:`LibsemigroupsException`. 
   
   :complexity: 
     :math:`O(n)` where :math:`n` is the size of the row being
     constructed.
   
   .. warning:: 

     This constructor only works for rows, i.e. when the template parameter
     :code:`R` is :code:`1`.
