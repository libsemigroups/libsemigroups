.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Validation
==========

.. cpp:namespace:: libsemigroups

The function :cpp:any:`validate_matrix` can be used to check that a matrix is
valid (i.e. that its entries belong to the underlying semiring). 


.. cpp:function:: template <typename Mat> \
                  void validate(Mat const& m)

   Throws an exception if the matrix is invalid.

   The overloaded static member functions :cpp:any:`DynamicMatrix::make` and 
   :cpp:any:`StaticMatrix::make` use this function to verify that the
   constructed matrix is valid. 

   :tparam Mat: a type for which :cpp:any:`IsMatrix<Mat>` is ``true``. 

   :param m: the matrix to validate.
   
   :returns: (None).

   :throws: A :cpp:any:`LibsemigroupsException` if the matrix ``m`` is invalid. 

   :complexity: 
     At worst :math:`O(nk)` where :math:`n` is the number of rows in the matrix
     ``m`` and :math:`k` is the number of columns. 


Threshold and period
====================

.. cpp:function:: template <typename Mat> \
                  typename Mat::scalar_type matrix_threshold(Mat const& m)

   Returns the threshold of a matrix (if any).

   This value is only defined for :cpp:any:`MaxPlusTruncMat`,
   :cpp:any:`MinPlusTruncMat`, and :cpp:any:`NTPMat`. If ``Mat`` is another
   type of matrix, then :cpp:any:`UNDEFINED` is returned. 

   :tparam Mat: a type for which :cpp:any:`IsMatrix<Mat>` is ``true``. 

   :param m: the matrix.
   
   :returns: A value of type ``typename Mat::scalar_type``.

   :exceptions: 
     This function is ``noexcept`` and is guaranteed never to throw.

   :complexity: 
     Constant.


.. cpp:function:: template <typename Mat> \
                  typename Mat::scalar_type matrix_period(Mat const& m)

   Returns the period of a matrix (if any).

   This value is only defined for :cpp:any:`NTPMat`. If ``Mat`` is another
   type of matrix, then :cpp:any:`UNDEFINED` is returned. 

   :tparam Mat: a type for which :cpp:any:`IsMatrix<Mat>` is ``true``. 

   :param m: the matrix.
   
   :returns: A value of type ``typename Mat::scalar_type``.

   :exceptions: 
     This function is ``noexcept`` and is guaranteed never to throw.

   :complexity: 
     Constant.

