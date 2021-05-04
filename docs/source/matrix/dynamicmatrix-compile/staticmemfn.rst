.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

DynamicMatrix static member functions
=====================================

.. cpp:namespace:: libsemigroups::DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>

.. _dynamicmatrix_compile_identity:
.. cpp:function:: static DynamicMatrix identity(size_t n)
   
   Construct the :math:`n \times n` identity matrix.

   :param n: the dimension
   
   :returns: The :math:`n \times n` identity matrix.
   
   :exceptions:
     This function guarantees not to throw a 
     :cpp:any:`LibsemigroupsException`. 
   
   :complexity: 
     :math:`O(n ^ 2)`.

.. _dynamicmatrix_compile_make:
.. cpp:function:: static DynamicMatrix \
      make(std::initializer_list<std::initializer_list<scalar_type>> il)

   Validates the arguments, constructs a matrix and validates it.
   
   :param il: the values to be copied into the matrix.
   
   :returns: the constructed matrix if valid.
   
   :throws: 
     :cpp:any:`LibsemigroupsException` if ``il`` does not represent a
     matrix of the correct dimensions.
   
   :throws: 
     :cpp:any:`LibsemigroupsException` if the constructed matrix
     contains values that do not belong to the underlying semiring.
   
   :complexity:
     :math:`O(mn)` where :math:`m` is the number of rows and
     :math:`n` is the number of columns of the matrix.
      
.. cpp:function:: static DynamicMatrix make(std::initializer_list<scalar_type> il)

   Constructs a row and validates it.
   
   :param il: the values to be copied into the row.
   
   :returns: the constructed row if valid.
   
   :throws: 
     :cpp:any:`LibsemigroupsException` if the constructed row contains
     values that do not belong to the underlying semiring.
   
   :complexity:
     :math:`O(n)` where :math:`n` is the number of columns of the matrix.
   
   .. warning:: 

     This constructor only works for rows, i.e. when the template parameter
     :code:`R` is :code:`1`.
