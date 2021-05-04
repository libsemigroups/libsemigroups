.. Copyright (c) 2020, J. D. Mitchell

   Distributed under the terms of the GPL license version 3.

   The full license is in the file LICENSE, distributed with this software.

Boolean matrices
================

The following classes in ``libsemigroups`` represent boolean matrices:

.. toctree::
   :maxdepth: 1
   
   bmat8
   bmat

Boolean matrix helpers
----------------------

The following helper struct can be used to determine the smallest type of
boolean matrix of specified dimension. 

.. toctree::
   :maxdepth: 1
   
   ../../_generated/libsemigroups__fastestbmat

Boolean matrix adapters
-----------------------

The following adapters are implemented to so that :cpp:any:`BMat<N>` can be
used with :cpp:any:`Konieczny`. 

.. toctree::
   :maxdepth: 1
   
   adapters/imagerightaction
   adapters/imageleftaction
   adapters/lambda
   adapters/rho
   adapters/rank
